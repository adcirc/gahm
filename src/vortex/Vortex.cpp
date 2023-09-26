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

#include "Vortex.h"

#include <cassert>
#include <cmath>
#include <utility>

#include "atcf/StormPosition.h"
#include "gahm/GahmEquations.h"
#include "physical/Earth.h"
#include "util/Interpolation.h"

namespace Gahm {

/**
 * Constructor for the Vortex class that takes an AtcfFile and a point cloud
 *
 * @param atcfFile Pointer to the AtcfFile object
 * @param points Point cloud to be used for the vortex solution
 */
Vortex::Vortex(const Atcf::AtcfFile *atcfFile, Datatypes::PointCloud points)
    : m_atcfFile(atcfFile), m_points(std::move(points)) {}

/**
 * Solve the vortex for a given date
 * @param date Date to solve the vortex for
 * @return Vortex solution
 */
Datatypes::VortexSolution Vortex::solve(const Datatypes::Date &date) {
  //...Get the time iterator, next time iterator, and time weight. If the date
  // is after the last time snap, then use the last time snap
  auto [time_it, time_weight] = this->selectTime(date);
  if (time_it == m_atcfFile->data().end()) {
    time_it = std::prev(time_it);
    time_weight = 1.0;
  }
  auto time_it_next = std::next(time_it);
  if (time_it_next == m_atcfFile->data().end()) {
    time_it_next = time_it;
  }

  //...Interpolate the storm to the current position
  const auto current_storm_position = Gahm::Atcf::StormPosition::interpolate(
      time_it->position(), std::next(time_it)->position(), time_weight);
  const auto current_storm_translation =
      Gahm::Atcf::StormTranslation::interpolate(
          time_it->translation(), std::next(time_it)->translation(),
          time_weight);
  const auto background_pressure = Gahm::Interpolation::linear(
      time_it->backgroundPressure(), std::next(time_it)->backgroundPressure(),
      time_weight);
  const auto central_pressure = Gahm::Interpolation::linear(
      time_it->centralPressure(), std::next(time_it)->centralPressure(),
      time_weight);

  struct t_point_data {
    t_point_data(double distance, double azimuth, double fc,
                 t_point_position p0, t_point_position p1)
        : distance(distance), azimuth(azimuth), fc(fc), p0(p0), p1(p1) {}
    double distance;
    double azimuth;
    double fc;
    t_point_position p0;
    t_point_position p1;
  };
  std::vector<t_point_data> point_data;
  point_data.reserve(m_points.size());

  //...Get the point position information at both time levels. Note that the
  // point position is the position of the point relative to the storm at the
  // current time, not the position of the point relative to the storm at the
  // time of the specified snap
  for (auto &point : m_points) {
    double distance = Physical::Earth::distance(
        point.x(), point.y(), current_storm_position.point().x(),
        current_storm_position.point().y());
    double azimuth = Physical::Earth::azimuth(
        point.x(), point.y(), current_storm_position.point().x(),
        current_storm_position.point().y());
    auto point_position_0 =
        Vortex::getPointPosition(*time_it, distance, azimuth);
    auto point_position_1 =
        Vortex::getPointPosition(*time_it_next, distance, azimuth);
    double fc = Gahm::Physical::Earth::coriolis(point.y());

    point_data.emplace_back(distance, azimuth, fc, point_position_0,
                            point_position_1);
  }

  //...Generate a solution object
  Datatypes::VortexSolution solution;
  solution.reserve(m_points.size());

  for (const auto &point : point_data) {
    //...Interpolate parameter packs in space at two time points
    auto p0 = Vortex::getParameterPack(point.p0, *time_it);
    auto p1 = Vortex::getParameterPack(point.p1, *time_it_next);

    //...Interpolate the parameter packs in time
    auto pack = Vortex::interpolateParameterPack(p0, p1, time_weight);

    //...Phi
    auto phi = Gahm::Solver::GahmEquations::phi(pack.vmax_at_boundary_layer,
                                                pack.radius_to_max_wind,
                                                pack.holland_b, point.fc);

    //...Solve for the wind speed
    auto wind_speed = Gahm::Solver::GahmEquations::GahmWindSpeed(
        pack.radius_to_max_wind, pack.vmax_at_boundary_layer, point.distance,
        point.fc, pack.holland_b);

    //...Solve for the pressure value
    auto pressure = Gahm::Solver::GahmEquations::GahmPressure(
                        central_pressure, background_pressure, point.distance,
                        pack.radius_to_max_wind, pack.holland_b, phi) /
                    100.0;

    //...Decompose the wind into it's u/v components
    auto speed_over_vmax = wind_speed / pack.vmax_at_boundary_layer;
    auto tsx = speed_over_vmax * current_storm_translation.transitSpeedU();
    auto tsy = speed_over_vmax * current_storm_translation.transitSpeedV();
    auto u = wind_speed * std::cos(point.azimuth);
    auto v = -wind_speed * std::sin(point.azimuth);
    auto [uf, vf] = Vortex::rotate_winds(
        u, v,
        Vortex::friction_angle(point.distance, pack.radius_to_max_wind_true),
        current_storm_position.y());
    u = uf + tsx;
    v = vf + tsy;

    u *= Physical::Constants::oneToten();
    v *= Physical::Constants::oneToten();

    solution.push_back({u, v, pressure});
  }

  return solution;
}

/**
 * Select the time for the vortex solution
 * @param date Date to solve the vortex for
 * @return Tuple containing the iterator to the time snap and the time weight
 */
std::tuple<std::vector<Atcf::AtcfSnap>::const_iterator, double>
Vortex::selectTime(const Datatypes::Date &date) const {
  if (date <= m_atcfFile->data().front().date()) {
    return {m_atcfFile->data().begin(), 0.0};
  } else if (date >= m_atcfFile->data().back().date()) {
    return {std::prev(m_atcfFile->data().end()), 1.0};
  } else {
    auto time_it = std::lower_bound(
        m_atcfFile->data().begin(), m_atcfFile->data().end(), date,
        [](const Atcf::AtcfSnap &snap, const Datatypes::Date &date) {
          return snap.date() < date;
        });

    auto prev_time_it = std::prev(time_it);
    double time_weight = static_cast<double>(date.toSeconds() -
                                             prev_time_it->date().toSeconds()) /
                         static_cast<double>(time_it->date().toSeconds() -
                                             prev_time_it->date().toSeconds());

    return {prev_time_it, time_weight};
  }
}

/**
 * Find the position of a p0 in the vortex
 * @param p0 Point to find the position of
 * @param time_it Iterator to the time snap
 * @param time_weight Time weight
 * @return Point position
 */
Vortex::t_point_position Vortex::getPointPosition(const Atcf::AtcfSnap &snap,
                                                  const double distance,
                                                  const double azimuth) {
  auto [base_quadrant, quadrant_weight] = Vortex::getBaseQuadrant(azimuth);
  auto [isotach, isotach_weight] =
      Vortex::getBaseIsotach(distance, base_quadrant, snap);
  auto [isotach_adjacent, isotach_adjacent_weight] =
      Vortex::getBaseIsotach(distance, base_quadrant - 1, snap);
  return {isotach,         base_quadrant,    isotach_weight,
          quadrant_weight, isotach_adjacent, isotach_adjacent_weight};
}

/**
 * Get the base quadrant for a given angle
 * @param angle Angle to get the base quadrant for in radians
 * @return Base quadrant and remainder (delta angle) for the specified input
 * angle
 */
std::pair<int, double> Vortex::getBaseQuadrant(double angle) {
  constexpr double deg2rad = Physical::Constants::deg2rad();
  constexpr double angle_45 = 45.0 * deg2rad;
  constexpr double angle_135 = 135.0 * deg2rad;
  constexpr double angle_225 = 225.0 * deg2rad;
  constexpr double angle_315 = 315.0 * deg2rad;

  double delta_angle = 0.0;
  int base_quadrant = 0;
  if (angle < angle_45) {
    delta_angle = angle_45 + angle;
    base_quadrant = 0;
  } else if (angle <= angle_135) {
    delta_angle = angle - angle_45;
    base_quadrant = 1;
  } else if (angle <= angle_225) {
    delta_angle = angle - angle_135;
    base_quadrant = 2;
  } else if (angle <= angle_315) {
    delta_angle = angle - angle_225;
    base_quadrant = 3;
  } else {
    delta_angle = angle - angle_315;
    base_quadrant = 0;
  }
  return {base_quadrant, delta_angle};
}

/**
 * Get the base isotach for a given distance and quadrant
 * @param distance Distance from the point to the storm center
 * @param quadrant Quadrant that the point exists in
 * @param snap AtcfSnap to get the isotach from
 * @return Base isotach and weight for the distance
 */
std::pair<int, double> Vortex::getBaseIsotach(double distance, int quadrant,
                                              const Atcf::AtcfSnap &snap) {
  const auto radii = snap.radii()[quadrant];
  const auto radii_min = radii.front();
  const auto radii_max = radii.back();
  const auto max_isotach_index = snap.numberOfIsotachs() - 1;

  if (distance >= radii_max) {
    return {max_isotach_index, 1.0};
  } else if (distance <= radii_min) {
    return {0, 0.0};
  } else {
    auto isotach_it =
        std::lower_bound(radii.begin(), radii.end(), distance,
                         [](const double &radius, const double &distance) {
                           return radius < distance;
                         });
    auto prev_isotach_it = std::prev(isotach_it);
    auto isotach_index = std::distance(radii.begin(), prev_isotach_it);
    double isotach_weight =
        (distance - *prev_isotach_it) / (*isotach_it - *prev_isotach_it);
    return {isotach_index, isotach_weight};
  }
}

/**
 * Get the parameter pack for a given point position and time snap
 * @param point_position Point position to get the parameter pack for
 * @param snap Time snap 0 to get the parameter pack for
 * @return Parameter pack object
 */
Vortex::t_parameter_pack Vortex::getParameterPack(
    const t_point_position &point_position, const Atcf::AtcfSnap &snap) {
  return Vortex::interpolateParameterPackQuadrant(point_position, snap);
}

/**
 * Helper function to convert an isotach into a parameter pack for a given
 * quadrant
 * @param isotach Isotach to convert
 * @param quadrant Quadrant to convert the isotach for
 * @return Parameter pack object
 */
Vortex::t_parameter_pack Vortex::isotachToParameterPack(
    const Atcf::AtcfIsotach &isotach, int quadrant) {
  auto q = isotach.getQuadrant(quadrant);
  return {q.getRadiusToMaxWindSpeed(), q.getRadiusToMaxWindSpeed(),
          q.getVmaxAtBoundaryLayer(), q.getIsotachSpeedAtBoundaryLayer(),
          q.getGahmHollandB()};
}

/**
 * Get the parameter pack for a given point position and time snap
 * @param point_position Point position to get the parameter pack for
 * @param snap Time snap 0 to get the parameter pack for
 * @param time_index Time index to get the parameter pack for
 * @param quadrant_index Quadrant index to get the parameter pack for. Zero
 * denotes base quadrant, 1 denotes the next quadrant
 * @return Parameter pack object
 */
Vortex::t_parameter_pack Vortex::interpolateParameterPackIsotach(
    const t_point_position &point_position, const Atcf::AtcfSnap &snap,
    int quadrant_index) {
  auto [isotach, quadrant, weight] = [&]() {
    if (quadrant_index == 0) {
      return std::make_tuple(point_position.isotach,
                             point_position.quadrant + quadrant_index,
                             point_position.isotach_weight);
    } else {
      return std::make_tuple(point_position.isotach_adjacent,
                             point_position.quadrant + quadrant_index,
                             point_position.isotach_adjacent_weight);
    }
  }();

  if (isotach == snap.numberOfIsotachs() - 1) {
    return Vortex::isotachToParameterPack(snap.getIsotachs()[isotach],
                                          quadrant);
  };
  auto i0 = snap.getIsotachs()[isotach];
  auto i1 = snap.getIsotachs()[isotach + 1];
  auto p0 = Vortex::isotachToParameterPack(i0, quadrant);
  auto p1 = Vortex::isotachToParameterPack(i1, quadrant);
  return Vortex::interpolateParameterPack(p0, p1, weight);
}

/**
 * Interpolate the parameter pack for a given point position and time snap in
 * quadrant space
 * @param point_position Information about the point position relative to the
 * storm center
 * @param snap Time snap to interpolate the parameter pack for
 * @param time_index Time index to interpolate the parameter pack for
 * @return Parameter pack object
 */
Vortex::t_parameter_pack Vortex::interpolateParameterPackQuadrant(
    const Vortex::t_point_position &point_position,
    const Atcf::AtcfSnap &snap) {
  auto p0 = Vortex::interpolateParameterPackIsotach(point_position, snap, -1);
  auto p1 = Vortex::interpolateParameterPackIsotach(point_position, snap, 0);
  return Vortex::interpolateParameterPackRadial(p0, p1,
                                                point_position.quadrant_weight);
}

/**
 * Interpolate a new parameter pack from two existing parameter packs and a
 * weighting factor
 * @param p0 Parameter pack 0
 * @param p1 Parameter pack 1
 * @param weight Weighting factor
 * @return Interpolated parameter pack
 */
Vortex::t_parameter_pack Vortex::interpolateParameterPack(
    const Vortex::t_parameter_pack &p0, const Vortex::t_parameter_pack &p1,
    double weight) {
  auto rmax = Interpolation::linear(p0.radius_to_max_wind,
                                    p1.radius_to_max_wind, weight);
  auto rmax_true = Interpolation::linear(p0.radius_to_max_wind_true,
                                         p1.radius_to_max_wind_true, weight);
  auto vmax = Interpolation::linear(p0.vmax_at_boundary_layer,
                                    p1.vmax_at_boundary_layer, weight);
  auto isotach_speed =
      Interpolation::linear(p0.isotach_speed_at_boundary_layer,
                            p1.isotach_speed_at_boundary_layer, weight);
  auto b = Interpolation::linear(p0.holland_b, p1.holland_b, weight);
  return {rmax, rmax_true, vmax, isotach_speed, b};
}

/**
 * Interpolate a new parameter pack from two existing parameter packs and a
 * weighting factor radially from the storm center
 * @param p0 Parameter pack 0
 * @param p1 Parameter pack 1
 * @param weight Weighting factor
 * @return Interpolated parameter pack
 */
Vortex::t_parameter_pack Vortex::interpolateParameterPackRadial(
    const Vortex::t_parameter_pack &p0, const Vortex::t_parameter_pack &p1,
    double weight) {
  auto rmax = Interpolation::angle_idw(p0.radius_to_max_wind,
                                       p1.radius_to_max_wind, weight);
  auto rmax_true = Interpolation::angle_idw(p0.radius_to_max_wind_true,
                                            p1.radius_to_max_wind_true, weight);
  auto vmax = Interpolation::angle_idw(p0.vmax_at_boundary_layer,
                                       p1.vmax_at_boundary_layer, weight);
  auto isotach_speed =
      Interpolation::angle_idw(p0.isotach_speed_at_boundary_layer,
                               p1.isotach_speed_at_boundary_layer, weight);
  auto b = Interpolation::angle_idw(p0.holland_b, p1.holland_b, weight);
  return {rmax, rmax_true, vmax, isotach_speed, b};
}

/**
 * Computes the friction angle for a given radius and radius to max winds
 * @param radius The radius to compute the friction angle for
 * @param radius_to_max_winds The radius to max winds
 * @return The friction angle
 */
double Vortex::friction_angle(double radius, double radius_to_max_winds) {
  constexpr double degtorad = Physical::Constants::deg2rad();
  constexpr double angle_10 = 10.0 * degtorad;
  constexpr double angle_25 = 25.0 * degtorad;
  constexpr double angle_75 = 75.0 * degtorad;

  if (0.0 <= radius && radius < radius_to_max_winds) {
    return angle_10;
  } else if (radius_to_max_winds <= radius &&
             radius < 1.2 * radius_to_max_winds) {
    return angle_10 + angle_75 * (radius / radius_to_max_winds - 1.0);
  } else if (radius >= 1.2 * radius_to_max_winds) {
    return angle_25;
  } else {
    return 0.0;
  }
}

/**
 * Rotate the winds by a given angle
 * @param u u-component of the wind
 * @param v v-component of the wind
 * @param angle Angle to rotate the winds by in radians
 * @param latitude Latitude of the point to rotate the winds for
 * @return Rotated winds as a tuple
 */
std::tuple<double, double> Vortex::rotate_winds(double u, double v,
                                                double angle, double latitude) {
  auto sign = (latitude > 0.0) ? 1.0 : -1.0;
  auto a = sign * angle;
  auto cosa = std::cos(a);
  auto sina = std::sin(a);
  auto u_rot = u * cosa - v * sina;
  auto v_rot = u * sina + v * cosa;
  return std::make_tuple(u_rot, v_rot);
}

}  // namespace Gahm