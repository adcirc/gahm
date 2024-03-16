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
#ifndef GAHM_SRC_EARTH_H_
#define GAHM_SRC_EARTH_H_

#include <cmath>
#include <limits>
#include <tuple>

#include "datatypes/Point.h"
#include "physical/Constants.h"
#include "physical/Units.h"

namespace Gahm::Physical::Earth {

/*
 * Rotational rate of the earth in radians/s
 * @return Rotational rate of the earth in radians/s
 */
constexpr auto omega() -> double { return 7.292115e-5; }

/*
 * Earth's angular velocity in radians per second
 * @return Earth's angular velocity in radians per second
 */
static auto coriolis(const double lat) -> double {
  return 2.0 * omega() * std::sin(lat * Gahm::Physical::Constants::deg2rad());
}

/*
 * Earth equatorial radius in meters
 * @return Earth equatorial radius in meters
 */
constexpr auto equatorialRadius() -> double { return 6378137.0; }

/*
 * Earth polar radius in meters
 * @return Earth polar radius in meters
 */
constexpr auto polarRadius() -> double { return 6356752.3; }

/*
 * Earth radius in meters at a given latitude
 * Default latitude is the equator
 * See
 * https://en.wikipedia.org/wiki/Earth_radius#Radius_at_a_given_geodetic_latitude
 * for derivation
 * @param latitude Latitude in degrees
 * @return Earth radius in meters
 */
static auto radius(const double latitude = std::numeric_limits<double>::max())
    -> double {
  if (latitude == std::numeric_limits<double>::max()) {
    return equatorialRadius();
  }
  const double lat_radians = Gahm::Physical::Constants::deg2rad() * latitude;
  return std::sqrt((std::pow(equatorialRadius(), 4.0) * std::cos(lat_radians) *
                        std::cos(lat_radians) +
                    std::pow(polarRadius(), 4.0) * std::sin(lat_radians) *
                        std::sin(lat_radians)) /
                   (std::pow(equatorialRadius(), 2.0) * std::cos(lat_radians) *
                        std::cos(lat_radians) +
                    std::pow(polarRadius(), 2.0) * std::sin(lat_radians) *
                        std::sin(lat_radians)));
}

/*
 * Earth radius in meters between two latitudes
 * See https://en.wikipedia.org/wiki/Earth_radius#Radius_between_two_latitudes
 * for derivation
 * @param y1 Latitude 1 in degrees
 * @param y2 Latitude 2 in degrees
 * @return Earth radius in meters
 */
static auto radius(const double y1, const double y2) -> double {
  return Earth::radius((y1 + y2) / 2.0);
}

/*
 * Distance between two points on the earth's surface
 * @param x1 Longitude 1 in degrees
 * @param y1 Latitude 1 in degrees
 * @param x2 Longitude 2 in degrees
 * @param y2 Latitude 2 in degrees
 * @return Distance between two points on the earth's surface in meters
 */
static auto distance(const double x1, const double y1, const double x2,
                     const double y2) -> double {
  constexpr double deg2rad = Units::convert(Units::Degree, Units::Radian);
  const double lat1 = deg2rad * y1;
  const double lon1 = deg2rad * x1;
  const double lat2 = deg2rad * y2;
  const double lon2 = deg2rad * x2;
  auto dx = lon2 - lon1;
  auto dy = lat2 - lat1;
  auto a = std::pow(std::sin(dy / 2.0), 2.0) +
           std::cos(lat1) * std::cos(lat2) * std::pow(std::sin(dx / 2.0), 2.0);
  auto c = 2.0 * std::atan2(std::sqrt(a), std::sqrt(1.0 - a));
  return radius(y1, y2) * c;
}

/**
 * @brief Distance between two points on the earth's surface
 * @param p0 Point 0
 * @param p1 Point 1
 * @return Distance between two points on the earth's surface in meters
 */
static auto distance(const Gahm::Datatypes::Point &p0,
                     const Gahm::Datatypes::Point &p1) -> double {
  return distance(p0.x(), p0.y(), p1.x(), p1.y());
}

/*
 * Azimuth between two points on the earth's surface
 * @param x1 Longitude 1 in degrees
 * @param y1 Latitude 1 in degrees
 * @param x2 Longitude 2 in degrees
 * @param y2 Latitude 2 in degrees
 * @return Azimuth between two points on the earth's surface in meters
 */
static auto azimuth(double x1, double y1, double x2, double y2) -> double {
  constexpr double deg2rad = Units::convert(Units::Degree, Units::Radian);
  const double dx = (x2 - x1) * deg2rad;
  const double phi1 = y1 * deg2rad;
  const double phi2 = y2 * deg2rad;
  const double ay = std::sin(dx) * std::cos(phi2);
  const double ax = std::cos(phi1) * std::sin(phi2) -
                    std::sin(phi1) * std::cos(phi2) * std::cos(dx);
  auto azi = std::atan2(-ay, -ax);
  if (azi < 0.0) {
    azi += Physical::Constants::twoPi();
  }
  return azi;
}

/**
 * @brief Azimuth between two points on the earth's surface
 * @param p0 Point 0
 * @param p1 Point 1
 * @return Azimuth between two points on the earth's surface in meters
 */
static auto azimuth(const Gahm::Datatypes::Point &p0,
                    const Gahm::Datatypes::Point &p1) -> double {
  return azimuth(p0.x(), p0.y(), p1.x(), p1.y());
}

/*
 * Compute the spherical distance between two points on the earth's surface
 * as well as at the mean latitudes/longitudes between the points
 * @param x1 Longitude 1 in degrees
 * @param y1 Latitude 1 in degrees
 * @param x2 Longitude 2 in degrees
 * @param y2 Latitude 2 in degrees
 * @return Tuple of distances between the two points and the midpoint
 */
static auto sphericalDx(const double x1, const double y1, const double x2,
                        const double y2) -> std::tuple<double, double, double> {
  const double meanx = (x1 + x2) / 2.0;
  const double meany = (y1 + y2) / 2.0;
  return std::make_tuple(Gahm::Physical::Earth::distance(x1, meany, x2, meany),
                         Gahm::Physical::Earth::distance(meanx, y1, meanx, y2),
                         Gahm::Physical::Earth::distance(x1, y1, x2, y2));
}

}  // namespace Gahm::Physical::Earth

#endif  // GAHM_SRC_EARTH_H_
