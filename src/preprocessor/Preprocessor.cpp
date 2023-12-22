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

#include <algorithm>
#include <cmath>
#include <iterator>
#include <stdexcept>

#include "atcf/AtcfFile.h"
#include "atcf/AtcfQuadrant.h"
#include "atcf/AtcfSnap.h"
#include "atcf/StormTranslation.h"
#include "gahm/GahmSolver.h"
#include "physical/Constants.h"
#include "physical/Earth.h"

namespace Gahm {

/**
 * Constructor for the preprocessor class
 * @param atcf Pointer to the AtcfFile object
 */
Preprocessor::Preprocessor(Gahm::Atcf::AtcfFile *atcf, bool do_initialization)
    : m_atcf(atcf), m_isotachsProcessed(false) {
  if (do_initialization) {
    this->prepareAtcfData();
  }
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
    double p_min = snap.centralPressure();
    double p_back = snap.backgroundPressure();
    double latitude = snap.position().y();

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

        Gahm::Solver::GahmSolver solver(isotach_radius, isotach_speed, vmax,
                                        p_min, p_back, latitude);
        solver.solve();
        quadrant.setRadiusToMaxWindSpeed(solver.rmax());
        quadrant.setGahmHollandB(solver.bg());
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
                    2;
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
          return sum / 2;
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
  if (next.position().x() - now.position().x() < 0.0) {
    uu *= -1.0;
  }

  double vv = std::abs(v_dis / dt);
  if (next.position().y() - now.position().y() < 0.0) {
    vv *= -1.0;
  }

  const double uv = uv_dis / dt;
  double dir = std::atan2(uu, vv);
  if (dir < 0.0) {
    dir += Gahm::Physical::Constants::twoPi();
  }

  // uv = 1.5 * std::pow(uv, 0.63);

  return {uv, dir};
}

/**
 * Removes the translation speed from the wind speed
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
double Preprocessor::removeTranslationVelocityScalar(
    double wind_speed, double vmax_10m, const Atcf::StormTranslation &transit) {
  const double storm_motion = 1.5 * std::pow(transit.translationSpeed(), 0.63);
  return wind_speed - storm_motion;
}

double Preprocessor::removeTranslationVelocityComponents(
    double wind_speed, double vmax_10m, int quadrant,
    const Atcf::StormTranslation &transit) {
  const double scaling_factor = std::min(1.0, wind_speed / vmax_10m);
  const double storm_motion =
      1.5 * std::pow(transit.translationSpeed(), 0.63) * scaling_factor;
  const double storm_motion_u =
      storm_motion * std::sin(transit.translationDirection());
  const double storm_motion_v =
      storm_motion * std::cos(transit.translationDirection());

  const double quadrant_angle = Atcf::AtcfQuadrant::quadrant_angle(quadrant) +
                                Physical::Constants::halfPi();

  const double u = wind_speed * std::cos(quadrant_angle);
  const double v = wind_speed * std::sin(quadrant_angle);

  return std::sqrt(std::pow(u - storm_motion_u, 2) +
                   std::pow(v - storm_motion_v, 2));
}

/**
 * Computes the boundary layer wind speeds for each snap
 */
void Preprocessor::computeBoundaryLayerWindspeed() {
  for (auto &snap : m_atcf->data()) {
    double vmax =
        snap.vmax() * Physical::Constants::tenMeterToTopOfBoundaryLayer();
    vmax = Preprocessor::removeTranslationVelocityScalar(vmax, vmax,
                                                         snap.translation());
    snap.setVmaxBoundaryLayer(vmax);

    for (auto &iso : snap.getIsotachs()) {
      for (auto &quad : iso.getQuadrants()) {
        double isotach_boundarylayer_speed =
            Preprocessor::removeTranslationVelocityComponents(
                iso.getWindSpeed(), snap.vmaxBoundaryLayer(),
                quad.getQuadrantIndex(), snap.translation());

        isotach_boundarylayer_speed *=
            Physical::Constants::tenMeterToTopOfBoundaryLayer();
        quad.setIsotachSpeedAtBoundaryLayer(isotach_boundarylayer_speed);
        quad.setVmaxAtBoundaryLayer(snap.vmaxBoundaryLayer());
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
