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
#ifndef PHYSICAL_H
#define PHYSICAL_H

#include <cmath>
#include <limits>
#include <tuple>

class Physical {
 public:
  static constexpr double rotation_earth() { return 3600.0 * 7.2921 * 10e-5; }

  static constexpr double km2nmi() { return 1.852; }
  static constexpr double nmi2km() { return 1.0 / km2nmi(); }

  static constexpr double ms2mph() { return 2.23694; }
  static constexpr double kt2mph() { return 1.15078; }
  static constexpr double mph2kt() { return 1.0 / kt2mph(); }
  static constexpr double mph2ms() { return 1.0 / ms2mph(); }
  static constexpr double ms2kt() { return 1.94394; }
  static constexpr double kt2ms() { return 1.0 / ms2kt(); }
  static constexpr double mb2pa() { return 100.0; }

  static constexpr double pi() { return M_PI; }
  static constexpr double twopi() { return pi() * 2.0; }
  static constexpr double halfpi() { return pi() * 0.5; }
  static constexpr double e() { return M_E; }

  static constexpr double backgroundPressure() { return 1013.0; }
  static constexpr double windReduction() { return 0.9; }
  static constexpr double rhoAir() { return 1.15; }
  static constexpr double g() { return 9.80665; }
  static constexpr double omega() { return 2.0 * pi() / 86164.20; }
  static constexpr double rhoWat0() { return 1000.0; }
  static constexpr double one2ten() { return 0.8928; }

  static constexpr double deg2rad() { return pi() / 180.0; }
  static constexpr double rad2deg() { return 180.0 / pi(); }

  static constexpr double equatorialRadius() { return 6378137.0; }
  static constexpr double polarRadius() { return 6356752.3; }

  static double radiusEarth(
      const double latitude = std::numeric_limits<double>::max()) {
    if (latitude == std::numeric_limits<double>::max()) return 6378206.40;
    const double l = deg2rad() * latitude;
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
    return std::sqrt(std::pow(x2 - x1, 2.0) + std::pow(y2 - y1, 2.0));
  }

  static double geodesic_distance(const double x1, const double y1,
                                  const double x2, const double y2) {
    const double lat1 = deg2rad() * y1;
    const double lon1 = deg2rad() * x1;
    const double lat2 = deg2rad() * y2;
    const double lon2 = deg2rad() * x2;
    return 2.0 * radiusEarth(y1, y2) *
           std::asin(
               std::sqrt(std::pow(std::sin((lat2 - lat1) / 2.0), 2.0) +
                         std::cos(lat1) * std::cos(lat2) *
                             std::pow(std::sin((lon2 - lon1) / 2.0), 2.0)));
  }

  static double sphericalDistance(const double dx, const double dy,
                                  const double y1, const double y2) {
    return radiusEarth(y1, y2) *
           (2.0 * std::asin(std::sqrt(std::pow(std::sin(dy / 2.0), 2.0) +
                                      std::cos(y1 * deg2rad()) *
                                          std::cos(y2 * deg2rad()) *
                                          std::pow(std::sin(dx / 2.0), 2.0))));
  }

  static std::tuple<double, double, double> sphericalDx(const double x1,
                                                        const double y1,
                                                        const double x2,
                                                        const double y2) {
    const double avglat = (y2 + y1) / 2.0;
    return {sphericalDistance(x2 - x1, 0.0, avglat, avglat),
            sphericalDistance(0.0, y2 - y1, avglat, avglat),
            geodesic_distance(x1, x2, y1, y2)};
  }

  static double distance(const double x1, const double y1, const double x2,
                         const double y2, const bool geodesic = false) {
    return geodesic ? geodesic_distance(x1, y1, x2, y2)
                    : cartesian_distance(x1, y1, x2, y2);
  }

  static double azimuthEarth(const double x1, const double y1, const double x2,
                             const double y2) {
    const double dx = deg2rad() * radiusEarth(y1, y2) * (x1 - x2) *
                      std::cos(deg2rad() * (y2));
    const double dy = deg2rad() * radiusEarth(y1, y2) * (y1 - y2);
    const double azi = 360.0 + rad2deg() * std::atan2(dy, dx);
    return azi > 360.0 ? azi - 360.0 : azi;
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
};

#endif  // PHYSICAL_H
