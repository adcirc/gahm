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
#ifndef GAHM_SRC_PHYSICAL_ATMOSPHERIC_H_
#define GAHM_SRC_PHYSICAL_ATMOSPHERIC_H_

#include <cassert>
#include <cmath>

#include "physical/Constants.h"

namespace Gahm::Physical::Atmospheric {

/**
 * Computes the traditional Holland B parameter
 * @param vmax maximum storm wind velocity
 * @param p_center minimum storm pressure
 * @param p_background background pressure
 * @return Holland B
 */
constexpr auto calcHollandB(const double vmax, const double p_center,
                            const double p_background) -> double {
  assert(p_center != p_background);
  return (vmax * vmax * Gahm::Physical::Constants::rhoAir() * M_E) /
         (p_background - p_center);
}

/**
 * Computes the Rossby number for the storm
 * @param vmax max storm wind velocity
 * @param rmax radius to max winds
 * @param f_coriolis coriolis force
 * @return rossby number
 */
constexpr auto rossbyNumber(double vmax, double rmax, double f_coriolis)
    -> double {
  assert(f_coriolis > 0.0);
  assert(rmax > 0.0);
  assert(vmax > 0.0);
  return vmax / (f_coriolis * rmax);
}

/**
 * Compute the queensland inflow angle
 * @param r radius at current position
 * @param rmx radius to maximum winds
 * @return inflow angle
 */
constexpr auto queenslandInflowAngle(double r, double rmx) noexcept -> double {
  constexpr double degree1 = Constants::deg2rad();
  constexpr double degree10 = 10.0 * Constants::deg2rad();
  constexpr double degree25 = 25.0 * Constants::deg2rad();
  constexpr double degree75 = 75.0 * Constants::deg2rad();
  if (r > 0.0 && r < rmx) {
    return degree10 * r / rmx;
  } else if (rmx <= r && r < 1.2 * rmx) {
    return degree10 + degree75 * (r / rmx - degree1);
  } else if (r >= 1.2 * rmx) {
    return degree25;
  } else {
    return 0.0;
  }
}

}  // namespace Gahm::Physical::Atmospheric

#endif  // GAHM_SRC_PHYSICAL_ATMOSPHERIC_H_
