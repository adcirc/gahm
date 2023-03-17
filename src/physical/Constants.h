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
#ifndef GAHM_SRC_PHYSICALCONSTANTS_H_
#define GAHM_SRC_PHYSICALCONSTANTS_H_

#include <cmath>

namespace Gahm::Physical::Constants {

/*
 * Pi constant
 * @return Pi constant
 */
static constexpr double pi() { return M_PI; }

/*
 * 2*Pi constant
 * @return 2*Pi constant
 */
static constexpr double twoPi() { return 2.0 * M_PI; }

/*
 * Pi/2 constant
 * @return Pi/2 constant
 */
static constexpr double halfPi() { return M_PI_2; }

/*
 * Pi/4 constant
 * @return Pi/4 constant
 */
static constexpr double quarterPi() { return M_PI_4; }

/*
 * Pi/180 constant
 * @return Pi/180 constant
 */
static constexpr double deg2rad() { return M_PI / 180.0; }

/*
 * 180/Pi constant
 * @return 180/Pi constant
 */
static constexpr double rad2deg() { return 180.0 / M_PI; }

/*
 * Background pressure in millibars
 * @return Background pressure in millibars
 */
static constexpr double backgroundPressure() { return 1013.00; }

/*
 * Wind speed reduction factor for 10m winds
 * @return Wind speed reduction factor for 10m winds
 */
static constexpr double windReduction() { return 0.9; }

/*
 * Rho of air in kg/m^3
 * @return Rho of air in kg/m^3
 */
static constexpr double rhoAir() { return 1.293; }

/*
 * Gravitational acceleration in m/s^2
 * @return Gravitational acceleration in m/s^2
 */
static constexpr double g() { return 9.80665; }

/*
 * Rho of water in kg/m^3
 * @return Rho of water in kg/m^3
 */
static constexpr double rhoWater() { return 1000.0; }

/*
 * One to ten conversion factor
 * @return One to ten conversion factor
 */
static constexpr double oneToten() { return 0.8928; }

}  // namespace Gahm::Physical::Constants

#endif  // GAHM_SRC_PHYSICALCONSTANTS_H_
