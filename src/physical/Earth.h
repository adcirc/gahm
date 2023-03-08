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
#ifndef GAHM_SRC_EARTH_H_
#define GAHM_SRC_EARTH_H_

#include <cmath>

#include "physical/Constants.h"
#include "physical/Units.h"

namespace Gahm::Physical::Earth {

/*
 * Rotational rate of the earth in radians/s
 * @return Rotational rate of the earth in radians/s
 */
static constexpr double omega() { return 7.292115e-5; }

/*
 * Earth's angular velocity in radians per second
 * @return Earth's angular velocity in radians per second
 */
static double coriolis(const double lat) {
  return 2.0 * omega() * std::sin(lat * Constants::deg2rad());
}

/*
 * Earth equatorial radius in meters
 * @return Earth equatorial radius in meters
 */
static constexpr double equatorialRadius() { return 6378137.0; }

/*
 * Earth polar radius in meters
 * @return Earth polar radius in meters
 */
static constexpr double polarRadius() { return 6356752.3; }

/*
 * Earth radius in meters at a given latitude
 * Default latitude is the equator
 * See
 * https://en.wikipedia.org/wiki/Earth_radius#Radius_at_a_given_geodetic_latitude
 * for derivation
 * @param latitude Latitude in degrees
 * @return Earth radius in meters
 */
static double radius(
    const double latitude = std::numeric_limits<double>::max()) {
  if (latitude == std::numeric_limits<double>::max()) return 6378135.0;
  const double l = Constants::deg2rad() * latitude;
  return std::sqrt(
      (std::pow(equatorialRadius(), 4.0) * std::cos(l) * std::cos(l) +
       std::pow(polarRadius(), 4.0) * std::sin(l) * std::sin(l)) /
      (std::pow(equatorialRadius(), 2.0) * std::cos(l) * std::cos(l) +
       std::pow(polarRadius(), 2.0) * std::sin(l) * std::sin(l)));
}

/*
 * Earth radius in meters between two latitudes
 * See https://en.wikipedia.org/wiki/Earth_radius#Radius_between_two_latitudes
 * for derivation
 * @param y1 Latitude 1 in degrees
 * @param y2 Latitude 2 in degrees
 * @return Earth radius in meters
 */
static double radius(const double y1, const double y2) {
  return Earth::radius((y1 + y2) / 2.0);
}

}  // namespace Gahm::Physical::Earth

#endif  // GAHM_SRC_EARTH_H_
