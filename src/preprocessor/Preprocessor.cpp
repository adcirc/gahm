// GNU General Public License v3.0
//
// This file is part of the GAHM model (https://github.com/adcirc/gahm).
// Copyright (c) 2023 ADCIRC Development Group.
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, version 3.
//
// This program is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program. If not, see <http://www.gnu.org/licenses/>.
//
// Author: Zach Cobell
// Contact: zcobell@thewaterinstitute.org
//
#include "Preprocessor.h"

#include "gahm/GahmSolver.h"
#include "physical/Earth.h"

namespace Gahm {

/**
 * Constructor for the preprocessor class
 * @param atcf Pointer to the AtcfFile object
 */
Preprocessor::Preprocessor(Gahm::Atcf::AtcfFile *atcf, bool do_initialization)
    : m_atcf(atcf), m_isotachsProcessed(false) {
  if (do_initialization) this->prepareAtcfData();
}

/*
 * Prepares the ATCF data for the solver
 */
void Preprocessor::prepareAtcfData() {
  if (!m_isotachsProcessed) {
    this->orderIsotachs();
    this->fillMissingAtcfData();
    this->computeStormTranslationVelocities();
    this->computeBoundaryLayerWindspeed();
    this->processIsotachRadii();
    this->m_isotachsProcessed = true;
  }
}

/*
 * Calculates the radius to maximum wind speed and GAHM B for each quadrant
 */
void Preprocessor::solve() {
  if (!m_isotachsProcessed) {
    throw std::runtime_error(
        "Isotach radii have not been processed. Please call "
        "Preprocessor::prepareAtcfData() before calling "
        "Preprocessor::solve().");
  }

  for (auto &snap : m_atcf->data()) {
    for (auto &isotach : snap.getIsotachs()) {
      for (auto &quadrant : isotach.getQuadrants()) {
        double isotach_radius = quadrant.getIsotachRadius();

        double isotach_speed = quadrant.getIsotachSpeedAtBoundaryLayer();
        double vmax = quadrant.getVmaxAtBoundaryLayer();

        //...Nudge the vmax to be greater than the isotach speed
        // TODO: Confirm with Rick if this is necessary
        if (vmax <= isotach_speed) {
          vmax = isotach_speed + 1.0;
        }

        double p_min = snap.centralPressure();
        double p_back = snap.backgroundPressure();
        double latitude = snap.position().y();
        Gahm::Solver::GahmSolver solver(isotach_radius, isotach_speed, vmax,
                                        p_min, p_back, latitude);
        solver.solve();

        double solution_gahm_rmax = solver.rmax();
        double solution_gahm_b = solver.bg();

        assert(solution_gahm_rmax > 0.0);
        assert(solution_gahm_b > 0.0);

        quadrant.setRadiusToMaxWindSpeed(solution_gahm_rmax);
        quadrant.setGahmHollandB(solution_gahm_b);
      }
    }
  }
}

/**
 * Fills in missing quadrant data in the Isotach objects
 */
void Preprocessor::fillMissingAtcfData() {
  for (auto &snap : m_atcf->data()) {
    for (auto &isotach : snap.getIsotachs()) {
      const auto n_missing = [&]() {
        return std::count_if(
            isotach.getQuadrants().begin(), isotach.getQuadrants().end(),
            [](auto &q) { return q.getIsotachRadius() == 0.0; });
      }();
      if (n_missing == 1) {
        // We find the missing one and take the average of its neighbors
        auto missing = std::find_if(
            isotach.getQuadrants().begin(), isotach.getQuadrants().end(),
            [](auto &q) { return q.getIsotachRadius() == 0.0; });
        auto missing_index = static_cast<int>(
            std::distance(isotach.getQuadrants().begin(), missing));
        auto left_index = missing_index + 3;
        auto right_index = missing_index + 1;
        auto mean = (isotach.getQuadrants()[left_index].getIsotachRadius() +
                     isotach.getQuadrants()[right_index].getIsotachRadius()) /
                    2.0;
        missing->setIsotachRadius(mean);
      } else if (n_missing == 2) {
        // We need to find the two missing quadrants and take the average of the
        // other two
        auto mean_not_missing = [&]() {
          auto sum = 0.0;
          for (auto &q : isotach.getQuadrants()) {
            if (q.getIsotachRadius() != 0.0) {
              sum += q.getIsotachRadius();
            }
          }
          return sum / 2.0;
        }();
        for (auto &q : isotach.getQuadrants()) {
          if (q.getIsotachRadius() == 0.0) {
            q.setIsotachRadius(mean_not_missing);
          }
        }
      } else if (n_missing == 3) {
        // We need to find the quadrant that is set and copy its value to the
        // missing three
        auto not_missing = std::find_if(
            isotach.getQuadrants().begin(), isotach.getQuadrants().end(),
            [](auto &q) { return q.getIsotachRadius() != 0.0; });
        for (auto &q : isotach.getQuadrants()) {
          if (q.getIsotachRadius() == 0.0) {
            q.setIsotachRadius(not_missing->getIsotachRadius());
          }
        }
      } else if (n_missing == 4) {
        // We set all four quadrants to the radius of the maximum wind
        for (auto &q : isotach.getQuadrants()) {
          q.setIsotachRadius(snap.radiusToMaxWinds());
        }
      }
    }
  }
}

/*
 * Computes the storm translation velocities
 */
void Preprocessor::computeStormTranslationVelocities() {
  for (auto it = m_atcf->data().begin(); it != m_atcf->data().end(); ++it) {
    if (it == m_atcf->data().begin()) {
      auto next = std::next(it);
      if (next == m_atcf->data().end()) {
        it->setTranslation({0.0, 0.0});
      } else {
        auto translation = Preprocessor::getTranslation(*it, *next);
        it->setTranslation(translation);
      }
    } else {
      auto prev = std::prev(it);
      auto translation = Preprocessor::getTranslation(*prev, *it);
      it->setTranslation(translation);
    }
  }
}

/*
 * Returns the translation object for two snaps
 * @param now Current snap
 * @param next Next snap
 * @return Translation object
 */
Gahm::Atcf::StormTranslation Preprocessor::getTranslation(
    const Gahm::Atcf::AtcfSnap &now, const Gahm::Atcf::AtcfSnap &next) {
  const auto dt =
      static_cast<double>(next.date().toSeconds() - now.date().toSeconds());
  const auto [u_dis, v_dis, uv_dis] = Gahm::Physical::Earth::sphericalDx(
      now.position().x(), now.position().y(), next.position().x(),
      next.position().y());
  auto uu = std::abs(u_dis / dt);
  if (next.position().x() - now.position().x() < 0.0) uu *= -1.0;

  double vv = std::abs(v_dis / dt);
  if (next.position().y() - now.position().y() < 0.0) vv *= -1.0;

  double uv = uv_dis / dt;
  double dir = std::atan2(vv, uu);
  if (dir < 0.0) dir += Gahm::Physical::Constants::twoPi();

  return {uv, dir};
}

/*
 * Computes the simple relative isotach wind speed
 * @param wind_speed Wind speed of the isotach
 * @param transit Storm translation object
 * @param quadrant Quadrant number
 *
 * Take the quadrant angle (45, 135, 225, 315) and add the storm translation
 * direction. Then, generate the u and v components of the wind speed. Add the
 * storm translation u and v components to the u and v components of the wind
 * speed.
 *
 */
double Preprocessor::computeSimpleRelativeIsotachWindspeed(
    double wind_speed, Atcf::StormTranslation transit, int quadrant) {
  double theta = Atcf::AtcfQuadrant::quadrant_angle(quadrant) +
                 transit.translationDirection();
  double u = wind_speed * std::cos(theta) - transit.transitSpeedU();
  double v = wind_speed * std::sin(theta) - transit.transitSpeedV();
  return std::sqrt(u * u + v * v) * Physical::Constants::windReduction();
}

/*
 * Computes the boundary layer wind speeds for each snap
 */
void Preprocessor::computeBoundaryLayerWindspeed() {
  for (auto &snap : m_atcf->data()) {
    for (auto &iso : snap.getIsotachs()) {
      for (auto &quad : iso.getQuadrants()) {
        quad.setIsotachSpeedAtBoundaryLayer(
            Preprocessor::computeSimpleRelativeIsotachWindspeed(
                iso.getWindSpeed(), snap.translation(),
                quad.getQuadrantIndex()));
        quad.setVmaxAtBoundaryLayer(
            Preprocessor::computeSimpleRelativeIsotachWindspeed(
                snap.vmax(), snap.translation(), quad.getQuadrantIndex()));
      }
    }
  }
}

/**
 * Orders the isotachs by wind speed. This makes them
 * easier to work with later on.
 */
void Preprocessor::orderIsotachs() {
  for (auto &snap : m_atcf->data()) {
    snap.orderIsotachs();
  }
}

/**
 * Compute a prebuilt array for radii in each quadrant
 */
void Preprocessor::processIsotachRadii() {
  for (auto &snap : m_atcf->data()) {
    snap.processIsotachRadii();
  }
}

}  // namespace Gahm