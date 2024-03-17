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

#include <algorithm>
#include <cassert>
#include <cmath>
#include <iterator>
#include <tuple>
#include <utility>
#include <vector>

#include "atcf/AtcfSnap.h"
#include "atcf/StormPosition.h"
#include "datatypes/Date.h"
#include "datatypes/PointPosition.h"
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
  auto sim_time = this->selectTime(date);
  auto time_it = std::get<0>(sim_time);
  auto time_weight = std::get<1>(sim_time);

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

  //...Generate a solution object
  Datatypes::VortexSolution solution;
  solution.reserve(m_points.size());

  const Datatypes::Uvp default_solution = {0.0, 0.0, central_pressure / 100.0};

  double fc = Gahm::Physical::Earth::coriolis(current_storm_position.y());

  //...We won't solve for points that are within 1 km of the storm center
  constexpr double min_distance = 1.0;

  //...Get the point position information at both time levels. Note that the
  // point position is the position of the point relative to the storm at the
  // current time, not the position of the point relative to the storm at the
  // time of the specified snap
  for (auto &point : m_points) {
    double distance = Physical::Earth::distance(
        point.x(), point.y(), current_storm_position.point().x(),
        current_storm_position.point().y());

    //...Check for the case where the point is at the center of the storm
    if (distance <= min_distance) {
      solution.push_back(default_solution);
      continue;
    }

    double azimuth = Physical::Earth::azimuth(
        point.x(), point.y(), current_storm_position.point().x(),
        current_storm_position.point().y());

    auto point_position_0 =
        Vortex::getPointPosition(*time_it, distance, azimuth);
    auto point_position_1 =
        Vortex::getPointPosition(*time_it_next, distance, azimuth);

    //...Interpolate parameter packs in space at two time points
    auto pack_t0 = Vortex::getParameterPack(point_position_0, *time_it);
    auto pack_t1 = Vortex::getParameterPack(point_position_1, *time_it_next);

    //...Interpolate the parameter packs in time
    auto pack = Vortex::interpolateParameterPack(pack_t0, pack_t1, time_weight);

    //...Phi
    auto phi = Gahm::Solver::GahmEquations::phi(pack.vmax_at_boundary_layer,
                                                pack.radius_to_max_wind,
                                                pack.holland_b, fc);

    //...Solve for the wind speed
    auto wind_speed = Gahm::Solver::GahmEquations::GahmWindSpeed(
        pack.radius_to_max_wind, pack.vmax_at_boundary_layer, distance, fc,
        pack.holland_b);

    //...Solve for the pressure value
    auto pressure = Gahm::Solver::GahmEquations::GahmPressure(
                        central_pressure, background_pressure, distance,
                        pack.radius_to_max_wind, pack.holland_b, phi) /
                    100.0;

    //...Begin finalizing the wind speed by decomposing the wind vector, adding
    // in the translation velocity, and rotating the winds
    auto [tsx, tsy] = Vortex::computeTranslationVelocityComponents(
        wind_speed, pack.vmax_at_boundary_layer, current_storm_translation);

    //...Move the wind speed back to 10m in height
    wind_speed *= Physical::Constants::topOfBoundaryLayerToTenMeter();

    //...Decompose the wind speed to its vector parts
    auto [u, v] = Vortex::decomposeWindVector(wind_speed, azimuth,
                                              current_storm_position.y());

    //...Rotate the winds
    auto [uf, vf] = Vortex::rotate_winds(
        u, v, Vortex::friction_angle(distance, pack.radius_to_max_wind_true),
        current_storm_position.y());

    //...Add the translation speed back into the wind vector
    //    uf += tsx;
    //    vf += tsy;

    uf *= Physical::Constants::oneMinuteToTenMinuteWind();
    vf *= Physical::Constants::oneMinuteToTenMinuteWind();

    //...Add the solution to the solution vector
    solution.emplace_back(
        uf, vf, pressure, azimuth, distance, tsx, tsy,
        point_position_0.quadrant_weight(), point_position_0.isotach_weight(),
        point_position_0.quadrant(), point_position_0.isotach());
  }

  return solution;
}

/**
 * Decompose a wind vector into u and v components
 * @param wind_speed The wind speed
 * @param azimuth The azimuth of the point relative to the storm center
 * @param latitude The latitude of the point
 * @return Tuple containing the u and v components
 */
std::tuple<double, double> Vortex::decomposeWindVector(double wind_speed,
                                                       double azimuth,
                                                       double latitude) {
  if (latitude < 0.0) {
    auto u_vec = wind_speed * std::cos(azimuth);
    auto v_vec = -wind_speed * std::sin(azimuth);
    return {u_vec, v_vec};
  } else {
    auto u_vec = -wind_speed * std::cos(azimuth);
    auto v_vec = wind_speed * std::sin(azimuth);
    return {u_vec, v_vec};
  }
}

/**
 * Compute the translation velocity components
 * @param wind_speed The wind speed at the boundary layer
 * @param vmax_at_boundary_layer The maximum wind speed at the boundary layer
 * @param translation The storm translation object
 * @param azimuth The azimuth of the point relative to the storm center
 * @return Tuple containing the translation velocity components
 */
std::tuple<double, double> Vortex::computeTranslationVelocityComponents(
    double wind_speed, double vmax_at_boundary_layer,
    const Atcf::StormTranslation &translation) {
  double scaling_factor = std::min(1.0, wind_speed / vmax_at_boundary_layer);

  double s_x =
      translation.transitSpeed() * std::sin(translation.translationDirection());
  double s_y =
      translation.transitSpeed() * std::cos(translation.translationDirection());

  s_x *= scaling_factor;
  s_y *= scaling_factor;

  return {s_x, s_y};
}

/**
 * Select the time for the vortex solution
 * @param date Date to solve the vortex for
 * @return Tuple containing the iterator to the time snap and the time weight
 */
auto Vortex::selectTime(const Datatypes::Date &date) const
    -> std::tuple<std::vector<Atcf::AtcfSnap>::const_iterator, double> {
  if (date <= m_atcfFile->data().front().date()) {
    return {m_atcfFile->data().begin(), 0.0};
  } else if (date >= m_atcfFile->data().back().date()) {
    return {std::prev(m_atcfFile->data().end()), 1.0};
  } else {
    auto time_it = std::lower_bound(
        m_atcfFile->data().begin(), m_atcfFile->data().end(), date,
        [](const Atcf::AtcfSnap &lhs, const Datatypes::Date &rhs) {
          return lhs.date() < rhs;
        });

    auto prev_time_it = std::prev(time_it);
    const double time_weight =
        static_cast<double>(date.toSeconds() -
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
Gahm::Datatypes::PointPosition Vortex::getPointPosition(
    const Atcf::AtcfSnap &snap, const double distance, const double azimuth) {
  auto [base_quadrant, delta_angle] = Vortex::getBaseQuadrant(azimuth);
  auto [isotach, isotach_weight] =
      Vortex::getBaseIsotach(distance, base_quadrant, snap);
  auto [isotach_adjacent, isotach_adjacent_weight] =
      Vortex::getBaseIsotach(distance, base_quadrant - 1, snap);
  return {isotach,     base_quadrant,    isotach_weight,
          delta_angle, isotach_adjacent, isotach_adjacent_weight};
}

/**
 * Get the base quadrant for a given angle
 * @param angle Angle to get the base quadrant for in radians
 * @return Base quadrant and remainder (delta angle) for the specified input
 * angle
 */
std::tuple<int, double> Vortex::getBaseQuadrant(double angle) {
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
std::tuple<int, double> Vortex::getBaseIsotach(double distance, int quadrant,
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
    auto isotach_it = std::lower_bound(
        radii.begin(), radii.end(), distance,
        [](const double &this_radius, const double &this_distance) {
          return this_radius < this_distance;
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
auto Vortex::getParameterPack(
    const Gahm::Datatypes::PointPosition &point_position,
    const Atcf::AtcfSnap &snap) -> Vortex::t_parameter_pack {
  return Vortex::interpolateParameterPackQuadrant(point_position, snap);
}

/**
 * Helper function to convert an isotach into a parameter pack for a given
 * quadrant
 * @param isotach Isotach to convert
 * @param quadrant Quadrant to convert the isotach for
 * @return Parameter pack object
 */
auto Vortex::isotachToParameterPack(const Atcf::AtcfIsotach &isotach,
                                    int quadrant) -> Vortex::t_parameter_pack {
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
auto Vortex::interpolateParameterPackIsotach(
    const Gahm::Datatypes::PointPosition &point_position,
    const Atcf::AtcfSnap &snap, int quadrant_index)
    -> Vortex::t_parameter_pack {
  auto [isotach, quadrant, weight] = [&]() {
    if (quadrant_index == 0) {
      return std::make_tuple(static_cast<size_t>(point_position.isotach()),
                             point_position.quadrant() + quadrant_index,
                             point_position.isotach_weight());
    } else {
      return std::make_tuple(
          static_cast<size_t>(point_position.isotach_adjacent()),
          point_position.quadrant() + quadrant_index,
          point_position.isotach_adjacent_weight());
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
    const Gahm::Datatypes::PointPosition &point_position,
    const Atcf::AtcfSnap &snap) {
  const auto p0 =
      Vortex::interpolateParameterPackIsotach(point_position, snap, -1);
  const auto p1 =
      Vortex::interpolateParameterPackIsotach(point_position, snap, 0);
  return Vortex::interpolateParameterPackRadial(
      p0, p1, point_position.quadrant_weight());
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