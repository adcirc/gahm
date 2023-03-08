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
#ifndef GAHM_SRC_PHYSICAL_ATMOSPHERIC_H_
#define GAHM_SRC_PHYSICAL_ATMOSPHERIC_H_

#include <cassert>
#include <cmath>

#include "physical/Constants.h"

namespace Gahm::Physical::Atmospheric {

/**
 * Computes the traditional Holland B parameter
 * @param vmax maximum storm wind velocity
 * @param p0 minimum storm pressure
 * @param pinf background pressure
 * @return Holland B
 */
constexpr double calcHollandB(const double vmax, const double p0,
                              const double pinf) {
  assert(p0 != pinf);
  return (vmax * vmax * Gahm::Physical::Constants::rhoAir() * M_E) /
         (100.0 * (pinf - p0));
}

/**
 * Computes the Rossby number for the storm
 * @param vmax max storm wind velocity
 * @param rmax radius to max winds
 * @param fc coriolis force
 * @return rossby number
 */
constexpr double rossbyNumber(double vmax, double rmax, double fc) {
  assert(fc > 0.0);
  assert(rmax > 0.0);
  assert(vmax > 0.0);
  return vmax / (fc * rmax);
}

/**
 * Compute the queensland inflow angle
 * @param r radius at current positoin
 * @param rmx radius to maximum winds
 * @return inflow angle
 */
constexpr double queenslandInflowAngle(double r, double rmx) noexcept {
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
