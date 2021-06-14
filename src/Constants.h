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
#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <cassert>
#include <cmath>
#include <limits>
#include <tuple>

#include "UnitConversion.h"

namespace Constants {

static constexpr double rotation_earth() { return 3600.0 * 7.2921 * 10e-5; }

static constexpr std::array<double, 4> m_quadrantAngles = {
    45.0 * Units::convert(Units::Degree, Units::Radian),
    (45.0 - 90.0) * Units::convert(Units::Degree, Units::Radian),
    (45.0 - 180.0) * Units::convert(Units::Degree, Units::Radian),
    (45.0 - 270.0) * Units::convert(Units::Degree, Units::Radian)};

static constexpr double pi() { return M_PI; }
static constexpr double twopi() { return pi() * 2.0; }
static constexpr double halfpi() { return pi() * 0.5; }
static constexpr double e() { return M_E; }

static constexpr double backgroundPressure() { return 1013.0; }
static constexpr double windReduction() { return 0.9; }
static constexpr double rhoAir() { return 1.293; }
static constexpr double g() { return 9.80665; }
static constexpr double omega() { return 2.0 * pi() / 86164.20; }
static constexpr double rhoWat0() { return 1000.0; }
static constexpr double one2ten() { return 0.8928; }

static constexpr double equatorialRadius() { return 6378137.0; }
static constexpr double polarRadius() { return 6356752.3; }

static double radiusEarth(
    const double latitude = std::numeric_limits<double>::max()) {
  if (latitude == std::numeric_limits<double>::max()) return 6378135.0;
  const double l = Units::convert(Units::Degree, Units::Radian) * latitude;
  return std::sqrt(
      (std::pow(equatorialRadius(), 4.0) * std::cos(l) * std::cos(l) +
       std::pow(polarRadius(), 4.0) * std::sin(l) * std::sin(l)) /
      (std::pow(equatorialRadius(), 2.0) * std::cos(l) * std::cos(l) +
       std::pow(polarRadius(), 2.0) * std::sin(l) * std::sin(l)));
}

static double radiusEarth(const double y1, const double y2) {
  return radiusEarth((y1 + y2) / 2.0);
}

static double cartesian_distance(const double x1, const double y1,
                                 const double x2, const double y2) {
  return std::sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1));
}

static double geodesic_distance(const double x1, const double y1,
                                const double x2, const double y2) {
  const double lat1 = Units::convert(Units::Degree, Units::Radian) * y1;
  const double lon1 = Units::convert(Units::Degree, Units::Radian) * x1;
  const double lat2 = Units::convert(Units::Degree, Units::Radian) * y2;
  const double lon2 = Units::convert(Units::Degree, Units::Radian) * x2;
  return 2.0 * radiusEarth(y1, y2) *
         std::asin(std::sqrt(std::pow(std::sin((lat2 - lat1) / 2.0), 2.0) +
                             std::cos(lat1) * std::cos(lat2) *
                                 std::pow(std::sin((lon2 - lon1) / 2.0), 2.0)));
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

static double azimuthEarth(const double x1, const double y1, const double x2,
                           const double y2) {
  const double lam0 = x1 * Units::convert(Units::Degree, Units::Radian);
  const double phi0 = y1 * Units::convert(Units::Degree, Units::Radian);
  const double lam1 = x2 * Units::convert(Units::Degree, Units::Radian);
  const double phi1 = y2 * Units::convert(Units::Degree, Units::Radian);
  const double dlam = lam1 - lam0;
  const double a = std::sin(dlam) * std::cos(phi1);
  const double b = std::cos(phi0) * std::sin(phi1);
  const double c = std::sin(phi0) * std::cos(phi1) * std::cos(dlam);
  const double azi = std::atan2(a, b - c);
  return azi * Units::convert(Units::Radian, Units::Degree);
}

static constexpr double coriolis(double lat) noexcept {
  return 2.0 * Constants::omega() *
         Units::convert(Units::Degree, Units::Radian) * lat;
}

static constexpr double calcHollandB(double vmax, double p0, double pinf) {
  assert(p0 != pinf);
  return (vmax * vmax * Constants::rhoAir() * Constants::e()) /
         (Units::convert(Units::Millibar, Units::Pascal) * (pinf - p0));
}

static constexpr double frictionAngle(double r, double rmx) noexcept {
  if (0.0 < r && r < rmx) {
    return 10.0 * r / rmx;
  } else if (rmx <= r && r < 1.2 * rmx) {
    return 10.0 + 75.0 * (r / rmx - 1.0);
  } else if (r >= 1.2 * rmx) {
    return 25.0;
  } else {
    return 0.0;
  }
}

constexpr double quadrantAngle(size_t index) {
  return Constants::m_quadrantAngles[index];
}

template <typename T>
static constexpr T fast_exp(const T x) noexcept {
  if (std::is_same<T, float>()) {
    constexpr auto v0 = double((1 << 20) / M_LN2);
    constexpr auto v1 = double((1 << 20) * 1023 - 0);
    union union_exp {
      double d_;
      int32_t i_[2];
      explicit union_exp(int32_t v) : i_{0, v} {}
    };
    union_exp uu(v0 * x + v1);
    return uu.d_;
  } else if (std::is_same<T, double>()) {
    constexpr auto v0 = double((int64_t(1) << 52) / M_LN2);
    constexpr auto v1 = double((int64_t(1) << 52) * 1023 - 0);
    union union_exp {
      double d_;
      int64_t i_;
      explicit union_exp(int64_t v) : i_{v} {}
    };
    union_exp uu(v0 * x + v1);
    return uu.d_;
  } else {
    return T(0);
  }
}

};  // namespace Constants

#endif  // CONSTANTS_H
