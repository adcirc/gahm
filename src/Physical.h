// MIT License
//
// Copyright (c) 2020 ADCIRC Development Group
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
#ifndef GAHM_SRC_PHYSICAL_H_
#define GAHM_SRC_PHYSICAL_H_

#include "Constants.h"
#include "FastMath.h"

namespace Gahm {
namespace Physical {

static constexpr double rotation_earth() { return 3600.0 * 7.2921 * 10e-5; }

static constexpr double backgroundPressure() { return 1013.0; }
static constexpr double windReduction() { return 0.9; }
static constexpr double rhoAir() { return 1.293; }
static constexpr double g() { return 9.80665; }
static constexpr double omega() {
  return 2.0 * Gahm::Constants::pi() / 86164.20;
}
static constexpr double rhoWat0() { return 1000.0; }
static constexpr double one2ten() { return 0.8928; }

static constexpr double equatorialRadius() { return 6378137.0; }
static constexpr double polarRadius() { return 6356752.3; }

static double radiusEarth(
    const double latitude = std::numeric_limits<double>::max()) {
  if (latitude == std::numeric_limits<double>::max()) return 6378135.0;
  const double l =
      Gahm::Units::convert(Gahm::Units::Degree, Gahm::Units::Radian) * latitude;
  return gahm_sqrt(
      (std::pow(equatorialRadius(), 4.0) * gahm_cos(l) * gahm_cos(l) +
       std::pow(polarRadius(), 4.0) * gahm_sin(l) * gahm_sin(l)) /
      (std::pow(equatorialRadius(), 2.0) * gahm_cos(l) * gahm_cos(l) +
       std::pow(polarRadius(), 2.0) * gahm_sin(l) * gahm_sin(l)));
}

static double radiusEarth(const double y1, const double y2) {
  return Gahm::Physical::radiusEarth((y1 + y2) / 2.0);
}

static double cartesian_distance(const double x1, const double y1,
                                 const double x2, const double y2) {
  return gahm_sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1));
}

static double geodesic_distance(const double x1, const double y1,
                                const double x2, const double y2) {
  const double lat1 = Units::convert(Units::Degree, Units::Radian) * y1;
  const double lon1 = Units::convert(Units::Degree, Units::Radian) * x1;
  const double lat2 = Units::convert(Units::Degree, Units::Radian) * y2;
  const double lon2 = Units::convert(Units::Degree, Units::Radian) * x2;
  return 2.0 * radiusEarth(y1, y2) *
         std::asin(gahm_sqrt(std::pow(gahm_sin((lat2 - lat1) / 2.0), 2.0) +
                             gahm_cos(lat1) * gahm_cos(lat2) *
                                 std::pow(gahm_sin((lon2 - lon1) / 2.0), 2.0)));
}

static std::tuple<double, double, double> sphericalDx(const double x1,
                                                      const double y1,
                                                      const double x2,
                                                      const double y2) {
  double meanx = (x1 + x2) / 2.0;
  double meany = (y1 + y2) / 2.0;
  return std::make_tuple(geodesic_distance(x1, meany, x2, meany),
                         geodesic_distance(meanx, y1, meanx, y2),
                         geodesic_distance(x1, y1, x2, y2));
}

static double distance(const double x1, const double y1, const double x2,
                       const double y2, const bool geodesic = false) {
  return geodesic ? geodesic_distance(x1, y1, x2, y2)
                  : cartesian_distance(x1, y1, x2, y2);
}

static double azimuthEarth(double x1, double y1, double x2, double y2) {
  constexpr double deg2rad = Units::convert(Units::Degree, Units::Radian);
  x1 *= deg2rad;
  y1 *= deg2rad;
  x2 *= deg2rad;
  y2 *= deg2rad;
  double dx = x1 - x2;
  double cy2 = gahm_cos(y2);
  double x = gahm_cos(y1) * gahm_sin(y2) - gahm_sin(y1) * cy2 * gahm_cos(dx);
  double y = gahm_sin(dx) * cy2;
  return std::atan2(y, x);
}

static constexpr double coriolis(double lat) noexcept {
  return 2.0 * Physical::omega() *
         Units::convert(Units::Degree, Units::Radian) * lat;
}

static constexpr double calcHollandB(double vmax, double p0, double pinf) {
  assert(p0 != pinf);
  return (vmax * vmax * Physical::rhoAir() * Constants::e()) /
         (Units::convert(Units::Millibar, Units::Pascal) * (pinf - p0));
}

static constexpr double frictionAngle(double r, double rmx) noexcept {
  constexpr double deg2rad = Units::convert(Units::Degree, Units::Radian);
  constexpr double degree1 = deg2rad;
  constexpr double degree10 = 10.0 * deg2rad;
  constexpr double degree25 = 25.0 * deg2rad;
  constexpr double degree75 = 75.0 * deg2rad;
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

}  // namespace Physical
}  // namespace Gahm
#endif  // GAHM_SRC_PHYSICAL_H_
