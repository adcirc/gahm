// MIT License
//
// Copyright (c) 2023 ADCIRC Development Group
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
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
static constexpr double piOverTwo() { return M_PI_2; }

/*
 * Pi/4 constant
 * @return Pi/4 constant
 */
static constexpr double piOverFour() { return M_PI_4; }

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
