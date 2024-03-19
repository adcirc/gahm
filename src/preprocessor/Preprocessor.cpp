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
#include <cstdlib>
#include <iterator>
#include <stdexcept>

#include "atcf/AtcfFile.h"
#include "atcf/AtcfIsotach.h"
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
    const double p_min = snap.centralPressure();
    const double p_back = snap.backgroundPressure();
    const double latitude = snap.position().y();

    for (auto &isotach : snap.isotachs()) {
      for (auto &quadrant : isotach.quadrants()) {
        const double isotach_radius = quadrant.isotachRadius();

        const double isotach_speed = quadrant.isotachSpeedAtBoundaryLayer();
        double vmax = quadrant.vmaxAtBoundaryLayer();

        //...Nudge the vmax to be greater than the isotach speed
        // TODO: Confirm with Rick if this is necessary
        if (vmax <= isotach_speed) {
          vmax = isotach_speed + 1.0;
        }

        Gahm::Solver::GahmSolver solver(isotach_radius, isotach_speed, vmax,
                                        p_min, p_back, latitude);
        solver.solve();
        quadrant.setRadiusToMaxWindSpeed(solver.rmax());
        quadrant.setGahmHollandB(solver.gahm_b());
      }
    }
  }
}

/**
 * Fills in missing quadrant data in the Isotach objects
 */
void Preprocessor::fillMissingAtcfData() {
  for (auto &snap : m_atcf->data()) {
    for (auto &isotach : snap.isotachs()) {
      const auto n_missing = Preprocessor::countMissingIsotachRadii(isotach);
      if (n_missing == 1) {
        Preprocessor::computeSingleMissingIsotachRadius(isotach);
      } else if (n_missing == 2) {
        Preprocessor::computeTwoMissingIsotachRadii(isotach);
      } else if (n_missing == 3) {
        Preprocessor::ComputeThreeMissingIsotachRadii(isotach);
      } else if (n_missing == 4) {
        Preprocessor::setAllIsotachRadiiToRmax(snap, isotach);
      }
    }
  }
}

/**
 * Counts the number of missing isotach radii
 * @param isotach Isotach object
 * @return Number of missing radii
 */
auto Preprocessor::countMissingIsotachRadii(Atcf::AtcfIsotach &isotach)
    -> long {
  return std::count_if(isotach.quadrants().begin(),
                       isotach.quadrants().end(),
      [](auto &quad) { return quad.isotachRadius() == 0.0; });
}

/**
 * Sets all isotach radii to the radius of the maximum wind
 * @param snap Snap object
 * @param isotach Isotach object
 */
void Preprocessor::setAllIsotachRadiiToRmax(const Atcf::AtcfSnap &snap,
                                            Atcf::AtcfIsotach &isotach) {
  for (auto &q : isotach.quadrants()) {
    q.setIsotachRadius(snap.radiusToMaxWinds());
  }
}

/**
 * Computes the three missing isotach radii by copying the one that is set
 * @param isotach Isotach object
 */
void Preprocessor::ComputeThreeMissingIsotachRadii(Atcf::AtcfIsotach &isotach) {
  const auto *not_missing =
      std::find_if(isotach.quadrants().begin(), isotach.quadrants().end(),
                   [](auto &q) { return q.isotachRadius() != 0.0; });
  for (auto &q : isotach.quadrants()) {
    if (q.isotachRadius() == 0.0) {
      q.setIsotachRadius(not_missing->isotachRadius());
    }
  }
}

/**
 * Computes the two missing isotach radii by taking the average of the other two
 * @param isotach Isotach object
 */
void Preprocessor::computeTwoMissingIsotachRadii(Atcf::AtcfIsotach &isotach) {
  const auto mean_not_missing = [&]() {
    auto sum = 0.0;
    for (auto &q : isotach.quadrants()) {
      if (q.isotachRadius() != 0.0) {
        sum += q.isotachRadius();
      }
    }
    return sum / 2.0;
  }();
  for (auto &q : isotach.quadrants()) {
    if (q.isotachRadius() == 0.0) {
      q.setIsotachRadius(mean_not_missing);
    }
  }
}

/**
 * Computes the missing isotach radius by taking the average of its neighbors
 * @param isotach Isotach object
 */
void Preprocessor::computeSingleMissingIsotachRadius(
    Atcf::AtcfIsotach &isotach) {
  auto *missing =
      std::find_if(isotach.quadrants().begin(), isotach.quadrants().end(),
                   [](auto &quad) { return quad.isotachRadius() == 0.0; });
  const auto missing_index =
      static_cast<int>(std::distance(isotach.quadrants().begin(), missing));
  const auto left_index = missing_index + 3;
  const auto right_index = missing_index + 1;
  const auto mean = (isotach.quadrants()[left_index].isotachRadius() +
                     isotach.quadrants()[right_index].isotachRadius()) /
                    2.0;
  missing->setIsotachRadius(mean);
}

/*
 * Computes the storm translation velocities
 */
void Preprocessor::computeStormTranslationVelocities() {
  for (auto it = m_atcf->data().begin(); it != m_atcf->data().end(); ++it) {
    if (it == m_atcf->data().begin()) {
      const auto next = std::next(it);
      if (next == m_atcf->data().end()) {
        it->setTranslation({0.0, 0.0});
      } else {
        const auto translation = Preprocessor::getTranslation(*it, *next);
        it->setTranslation(translation);
      }
    } else {
      const auto prev = std::prev(it);
      const auto translation = Preprocessor::getTranslation(*prev, *it);
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
auto Preprocessor::getTranslation(const Gahm::Atcf::AtcfSnap &now,
                                  const Gahm::Atcf::AtcfSnap &next)
    -> Gahm::Atcf::StormTranslation {
  const auto dt =
      static_cast<double>(next.date().toSeconds() - now.date().toSeconds());
  const auto [u_dis, v_dis, uv_dis] = Gahm::Physical::Earth::sphericalDx(
      now.position().x(), now.position().y(), next.position().x(),
      next.position().y());
  auto u_translation = std::abs(u_dis / dt);
  if (next.position().x() - now.position().x() < 0.0) {
    u_translation *= -1.0;
  }

  double v_translation = std::abs(v_dis / dt);
  if (next.position().y() - now.position().y() < 0.0) {
    v_translation *= -1.0;
  }

  double uv_translation = uv_dis / dt;
  double dir = std::atan2(u_translation, v_translation);
  if (dir < 0.0) {
    dir += Gahm::Physical::Constants::twoPi();
  }

  uv_translation = 1.5 * std::pow(uv_translation, 0.63);

  return {uv_translation, dir};
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
auto Preprocessor::removeTranslationVelocity(
    double wind_speed, double vmax_10m, int quadrant,
    const Atcf::StormTranslation &transit, double latitude) -> double {
  //  double uu =
  //      wind_speed *
  //      std::cos(Atcf::AtcfQuadrant::quadrant_angle(quadrant)+Physical::Constants::halfPi());
  //  double vv =
  //      wind_speed *
  //      std::sin(Atcf::AtcfQuadrant::quadrant_angle(quadrant)+Physical::Constants::halfPi());
  //  double tsx =
  //      transit.transitSpeed() * std::sin(transit.translationDirection());
  //  double tsy =
  //      transit.transitSpeed() * std::cos(transit.translationDirection());

  //  auto [uu, vv] = Vortex::decomposeWindVector(
  //      wind_speed, Atcf::AtcfQuadrant::quadrant_angle(quadrant), latitude);
  //  auto [tsx, tsy] = Vortex::computeTranslationVelocityComponents(
  //      wind_speed, vmax_10m, transit);
  // Mark parameters used to suppress warnings
  (void)vmax_10m;
  (void)latitude;
  (void)quadrant;
  (void)transit;

  return wind_speed;
  // return std::sqrt(std::pow(uu - tsx, 2.0) + std::pow(vv - tsy, 2.0));
}

/**
 * @overload
 * @brief Removes the translation speed from the wind speed
 * @param wind_speed Wind speed of the isotach
 * @param vmax_10m Maximum wind speed at 10m
 * @param transit Storm translation object
 * @return Wind speed with translation removed
 */
auto Preprocessor::removeTranslationVelocity(
    double wind_speed, double vmax_10m, const Atcf::StormTranslation &transit)
    -> double {
  const double scaling_factor = std::min(1.0, wind_speed / vmax_10m);
  return wind_speed - (transit.translationSpeed() * scaling_factor);
}

/**
 * Computes the boundary layer wind speeds for each snap
 */
void Preprocessor::computeBoundaryLayerWindspeed() {
  for (auto &snap : m_atcf->data()) {
    double vmax =
        snap.vmax() * Physical::Constants::tenMeterToTopOfBoundaryLayer();
    vmax =
        Preprocessor::removeTranslationVelocity(vmax, vmax, snap.translation());
    snap.setVmaxBoundaryLayer(vmax);

    for (auto &iso : snap.isotachs()) {
      for (auto &quad : iso.quadrants()) {
        double isotach_boundarylayer_speed =
            Preprocessor::removeTranslationVelocity(
                iso.windSpeed(), snap.vmaxBoundaryLayer(),
                quad.quadrantIndex(), snap.translation(),
                snap.position().y());
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
