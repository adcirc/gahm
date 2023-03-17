//
// Created by Zach Cobell on 3/14/23.
//

#include "Vortex.h"

#include <cassert>
#include <cmath>
#include <utility>

#include "datatypes/Date.h"
#include "gahm/GahmEquations.h"
#include "physical/Constants.h"
#include "physical/Earth.h"
#include "physical/Units.h"
#include "util/Interpolation.h"

namespace Gahm::Vortex {

/**
 * Constructor for a Vortex object
 * @param atcf AtcfFile object
 * @param x
 * @param y
 */
Vortex::Vortex(const Gahm::Atcf::AtcfFile *atcf, const std::vector<double> &x,
               const std::vector<double> &y)
    : m_atcf(atcf),
      m_points(x, y),
      m_distance_and_azimuth(atcf->size(), std::vector<t_distance_and_azimuth>(
                                               m_points.size(), {0.0, 0.0})) {
  assert(atcf->size() > 1);
  this->computeAllDistanceAndAzimuth();
}

/**
 * Constructor for a Vortex object
 * @param atcf AtcfFile object
 * @param points PointCloud object
 */
Vortex::Vortex(const Gahm::Atcf::AtcfFile *atcf, Datatypes::PointCloud points)
    : m_atcf(atcf),
      m_points(std::move(points)),
      m_distance_and_azimuth(atcf->size(), std::vector<t_distance_and_azimuth>(
                                               m_points.size(), {0.0, 0.0})) {
  assert(atcf->size() > 1);
  this->computeAllDistanceAndAzimuth();
}

/**
 * Compute the distance and azimuth between the storm and the grid points
 * These arrays will be interpolated from at runtime to avoid this initial
 * overhead
 */
void Vortex::computeAllDistanceAndAzimuth() {
  size_t index = 0;
  for (const auto &snap : m_atcf->data()) {
    size_t index_point = 0;
    for (const auto &p : m_points) {
      m_distance_and_azimuth[index][index_point] =
          Vortex::distanceAndAzimuth(p, snap.position().point());
      index_point++;
    }
    index++;
  }
}

/**
 * Compute the distance and azimuth between the storm and the given point
 * @param p Point to compute distance and azimuth to
 * @param storm Storm position
 * @return Distance and azimuth to the point as a t_distance_and_azimuth object
 */
Vortex::t_distance_and_azimuth Vortex::distanceAndAzimuth(
    const Datatypes::Point &p, const Datatypes::Point &storm) {
  constexpr double deg2rad = Physical::Constants::deg2rad();
  const double radius_earth_degrees = Physical::Earth::radius() * deg2rad;
  double dx = radius_earth_degrees * (p.x() - storm.x()) *
              std::cos(deg2rad * storm.y());
  double dy = radius_earth_degrees * (p.y() - storm.y());
  //  double azi = std::atan2(dx, dy);
  //  if (azi < 0.0) azi += 2.0 * Physical::Constants::pi();

  auto dx_rad = (storm.x() - p.x()) * deg2rad;
  auto phi1 = p.y() * deg2rad;
  auto phi2 = storm.y() * deg2rad;
  auto ay = std::sin(dx_rad) * std::cos(phi2);
  auto ax = std::cos(phi1) * std::sin(phi2) -
            std::sin(phi1) * std::cos(phi2) * std::cos(dx_rad);
  auto azi = std::atan2(ay, ax);
  if (azi < 0.0) azi += Physical::Constants::twoPi();
  return {std::sqrt(dx * dx + dy * dy), azi};
}

/**
 * Select the time index for the given time
 * @param time Time to select
 * @return Iterator to the AtcfSnap object
 */
std::tuple<std::vector<Atcf::AtcfSnap>::const_iterator, double>
Vortex::selectTimeIndex(const Gahm::Datatypes::Date &time) const {
  // If the time is outside the range of the AtcfFile, return the end iterator
  if (time <= m_atcf->data().front().date()) {
    return {m_atcf->data().begin(), 1.0};
  } else if (time >= m_atcf->data().back().date()) {
    return {std::prev(m_atcf->data().end()), 1.0};
  } else {
    auto it = std::lower_bound(
        m_atcf->data().begin(), m_atcf->data().end(), time,
        [](const Atcf::AtcfSnap &snap, const Datatypes::Date &time) {
          return snap.date() < time;
        });
    if (it->date() == time) {
      return {it, 1.0};
    } else {
      auto it2 = std::prev(it);
      auto dt =
          static_cast<double>(time.toSeconds() - it2->date().toSeconds()) /
          static_cast<double>(it->date().toSeconds() - it2->date().toSeconds());
      return {it2, dt};
    }
  }
}

/**
 * Interpolate the distance and azimuth between the storm and the grid points
 * for all grid points
 * @param timeIndex Time index
 * @param weight Weight of the time index
 * @return Vector of distance and azimuth objects
 */
std::vector<Vortex::t_distance_and_azimuth>
Vortex::interpolateDistanceAndAzimuth(
    const std::vector<Atcf::AtcfSnap>::const_iterator &timeIndex,
    double weight) const {
  auto pos = std::distance(m_atcf->data().begin(), timeIndex);
  if (weight == 1.0) {
    return m_distance_and_azimuth[pos];
  } else {
    std::vector<t_distance_and_azimuth> result;
    result.reserve(m_points.size());
    for (size_t index = 0; index < m_points.size(); index++) {
      result.push_back(
          this->computeDistanceAndAzimuth(timeIndex, weight, index, pos));
    }
    return result;
  }
}

/**
 * Compute the distance and azimuth between the storm and the grid point
 * @param timeIndex Time index
 * @param weight Weight of the time index
 * @param index Index of the grid point
 * @param position Position of the time index in the AtcfFile
 * @return Distance and azimuth object
 */
Vortex::t_distance_and_azimuth Vortex::computeDistanceAndAzimuth(
    const std::vector<Atcf::AtcfSnap>::const_iterator &timeIndex, double weight,
    size_t index, long position) const {
  [[unlikely]] if (position == -1) {
    position = std::distance(m_atcf->data().begin(), timeIndex);
  }
  auto it2 = std::next(timeIndex);
  auto pos2 = std::distance(m_atcf->data().begin(), it2);
  auto distance = Interpolation::linear(
      m_distance_and_azimuth[position][index].distance,
      m_distance_and_azimuth[pos2][index].distance, weight);
  auto azimuth =
      Interpolation::angle(m_distance_and_azimuth[position][index].azimuth,
                           m_distance_and_azimuth[pos2][index].azimuth, weight);
  return {distance, azimuth};
}

Datatypes::VortexSolution Vortex::solve(const Datatypes::Date &time) const {
  auto [thisSnap, weight] = this->selectTimeIndex(time);
  auto nextSnap = std::next(thisSnap);

  auto da = this->interpolateDistanceAndAzimuth(thisSnap, weight);
  Datatypes::VortexSolution solution;
  solution.reserve(m_points.size());

  for (size_t index = 0; index < m_points.size(); index++) {
    solution.push_back(
        this->solveEquations(thisSnap, nextSnap, weight, index, da[index]));
  }

  return solution;
}

std::tuple<int, double> Vortex::findBaseQuadrant(double azimuth) {
  if (azimuth < 0.0) {
    azimuth += Physical::Constants::twoPi();
  } else if (azimuth > Physical::Constants::twoPi()) {
    azimuth -= Physical::Constants::twoPi();
  }
  auto quad =
      std::floor(std::fmod(azimuth / Physical::Constants::halfPi(), 4.0));
  auto rem = azimuth - quad * Physical::Constants::halfPi();

  // This is the case for twoPi (360 deg)
  if (rem >= Physical::Constants::twoPi()) {
    rem -= Physical::Constants::twoPi();
  }

  auto quadrant_weight = rem / Physical::Constants::halfPi();
  return {static_cast<int>(quad), quadrant_weight};
}

std::tuple<int, double, double, double> Vortex::findBaseIsotach(
    const Atcf::AtcfSnap &snap, int quadrant, double distance) {
  const auto radii = snap.radii()[quadrant];
  if (radii.empty()) {
    assert(1 == 0);
    return {0, 1.0, snap.radiusToMaxWinds(), snap.radiusToMaxWinds()};
  } else if (distance >= radii.back()) {
    return {snap.numberOfIsotachs() - 1, 1.0, radii.back(), radii.back()};
  } else if (distance <= radii.front()) {
    return {0, 1.0, radii.front(), radii.front()};
  } else {
    auto it = std::lower_bound(radii.begin(), radii.end(), distance);
    auto it2 = std::prev(it);
    auto dt = (distance - *it2) / (*it - *it2);
    assert(dt >= 0.0 && dt <= 1.0);
    auto pos = std::distance(radii.begin(), it2);
    assert(pos >= 0 && pos < snap.numberOfIsotachs());
    return {pos, dt, *it2, *it};
  }
}

Vortex::t_quadrant_and_isotach Vortex::findQuadrantAndIsotach(
    const Atcf::AtcfSnap &snap, const t_distance_and_azimuth &da) {
  auto quadrant = Vortex::findBaseQuadrant(da.azimuth);
  auto isotach =
      Vortex::findBaseIsotach(snap, std::get<0>(quadrant), da.distance);
  return {
      std::get<0>(quadrant), std::get<1>(quadrant), std::get<0>(isotach),
      std::get<1>(isotach),  std::get<2>(isotach),  std::get<3>(isotach),
  };
}

Vortex::t_storm_parameters Vortex::interpolateAlongIsotach(
    const Atcf::AtcfSnap &snap, const t_quadrant_and_isotach &factors,
    int quadrant_position) {
  if (factors.isotach + 1 == snap.numberOfIsotachs()) {
    auto q = snap.getIsotachs()[factors.isotach].getQuadrant(quadrant_position);
    return {q.getIsotachSpeedAtBoundaryLayer(), q.getRadiusToMaxWindSpeed(),
            q.getGahmHollandB(), q.getRadiusToMaxWindSpeed()};
  } else {
    auto iso0 = snap.getIsotachs()[factors.isotach];
    auto iso1 = snap.getIsotachs()[factors.isotach + 1];
    auto q0 = iso0.getQuadrant(factors.quadrant + quadrant_position);
    auto q1 = iso1.getQuadrant(factors.quadrant + quadrant_position);

    double rmw = Interpolation::linear(q0.getRadiusToMaxWindSpeed(),
                                       q1.getRadiusToMaxWindSpeed(),
                                       factors.isotach_weight);
    double rmw_true = Interpolation::linear(q0.getRadiusToMaxWindSpeed(),
                                            q1.getRadiusToMaxWindSpeed(),
                                            factors.isotach_weight);
    double vmax_boundary_layer = Interpolation::linear(
        q0.getIsotachSpeedAtBoundaryLayer(),
        q1.getIsotachSpeedAtBoundaryLayer(), factors.isotach_weight);
    double b = Interpolation::linear(q0.getGahmHollandB(), q1.getGahmHollandB(),
                                     factors.isotach_weight);

    return {vmax_boundary_layer, rmw, b, rmw_true};
  }
}

Vortex::t_storm_parameters Vortex::interpolateAlongRadius(
    t_storm_parameters &p0, t_storm_parameters &p1,
    t_quadrant_and_isotach &factors) {
  // constexpr double angle_90 = Physical::Constants::halfPi();
  if (factors.quadrant_weight == 0.0) {
    return p0;
  } else if (factors.quadrant_weight == 1.0) {
    return p1;
  } else {
    // double d0 = 1.0 / std::pow(factors.quadrant_weight, 2.0);
    // double d1 = 1.0 / std::pow(angle_90 - factors.quadrant_weight, 2.0);
    //    double rmw = Interpolation::quadrant(p0.radius_to_max_wind,
    //                                         p1.radius_to_max_wind, d0, d1);
    //    double rmw_true = Interpolation::quadrant(
    //        p0.radius_to_max_winds_true, p1.radius_to_max_winds_true, d0, d1);
    //    double vmax_boundary_layer = Interpolation::quadrant(
    //        p0.vmax_boundary_layer, p1.vmax_boundary_layer, d0, d1);
    //    double b = Interpolation::quadrant(p0.holland_b, p1.holland_b, d0,
    //    d1);

    double rmw = Interpolation::linear(
        p0.radius_to_max_wind, p1.radius_to_max_wind, factors.quadrant_weight);
    double rmw_true = Interpolation::linear(p0.radius_to_max_winds_true,
                                            p1.radius_to_max_winds_true,
                                            factors.quadrant_weight);
    double vmax_boundary_layer =
        Interpolation::linear(p0.vmax_boundary_layer, p1.vmax_boundary_layer,
                              factors.quadrant_weight);
    double b = Interpolation::linear(p0.holland_b, p1.holland_b,
                                     factors.quadrant_weight);

    assert(rmw > 0.0);
    assert(rmw_true > 0.0);
    assert(b > 0.0);
    assert(vmax_boundary_layer > 0.0);

    return {vmax_boundary_layer, rmw, b, rmw_true};
  }
}

Vortex::t_storm_parameters Vortex::getStormParameters(
    const Atcf::AtcfSnap &snap, const t_distance_and_azimuth &da) {
  auto factors = Vortex::findQuadrantAndIsotach(snap, da);
  auto p0 = Vortex::interpolateAlongIsotach(snap, factors, 0);
  //auto p1 = Vortex::interpolateAlongIsotach(snap, factors, 1);
  //return Vortex::interpolateAlongRadius(p0, p0, factors);
  return p0;
}

Vortex::t_storm_parameters Vortex::interpolateParametersInTime(
    const t_storm_parameters &p0, const t_storm_parameters &p1,
    double time_weight) {
  double rmw = Interpolation::linear(p0.radius_to_max_wind,
                                     p1.radius_to_max_wind, time_weight);
  double rmw_true = Interpolation::linear(
      p0.radius_to_max_winds_true, p1.radius_to_max_winds_true, time_weight);
  double vmax_boundary_layer = Interpolation::linear(
      p0.vmax_boundary_layer, p1.vmax_boundary_layer, time_weight);
  double b = Interpolation::linear(p0.holland_b, p1.holland_b, time_weight);
  return {vmax_boundary_layer, rmw, b, rmw_true};
}

Datatypes::VortexSolution::t_uvp Vortex::solveEquations(
    const std::vector<Atcf::AtcfSnap>::const_iterator &thisSnap,
    const std::vector<Atcf::AtcfSnap>::const_iterator &nextSnap,
    double time_weight, size_t pointIndex,
    const t_distance_and_azimuth &distance_azimuth2) const {

  time_weight = 1.0 - time_weight;

  constexpr double threshold_distance =
      1.0 * Physical::Units::convert(Physical::Units::NauticalMile,
                                     Physical::Units::Meter);

  auto storm_position = Atcf::StormPosition::interpolate(
      thisSnap->position(), nextSnap->position(), time_weight);
  auto distance_azimuth =
      Vortex::distanceAndAzimuth(m_points[pointIndex], storm_position.point());

  if (distance_azimuth.distance < threshold_distance) {
    return {0.0, 0.0, 0.0};
    return {0.0, 0.0,
            Interpolation::linear(thisSnap->centralPressure(),
                                  nextSnap->centralPressure(), time_weight)};
  }

  auto pos0 = std::distance(m_atcf->data().begin(), thisSnap);
  auto pos1 = std::distance(m_atcf->data().begin(), nextSnap);

  auto factors = Vortex::findQuadrantAndIsotach(*thisSnap, distance_azimuth);

  auto params_t0 = Vortex::getStormParameters(
      *thisSnap, m_distance_and_azimuth[pos0][pointIndex]);
  auto params_t1 = Vortex::getStormParameters(
      *nextSnap, m_distance_and_azimuth[pos1][pointIndex]);

  auto stormTranslation = Atcf::StormTranslation::interpolate(
      thisSnap->translation(), nextSnap->translation(), time_weight);
  auto pressure = Interpolation::linear(
      thisSnap->centralPressure(), nextSnap->centralPressure(), time_weight);

  auto params =
      Vortex::interpolateParametersInTime(params_t0, params_t1, time_weight);

  auto fc = Physical::Earth::coriolis(m_points[pointIndex].y());
  auto wind_speed = Gahm::Solver::GahmEquations::GahmFunction(
      params.radius_to_max_wind, params.vmax_boundary_layer, 0.0,
      distance_azimuth.distance, fc, params.holland_b);
  auto speedOverVmax = wind_speed / params.vmax_boundary_layer;
  auto tsx = speedOverVmax * stormTranslation.transitSpeedU();
  auto tsy = speedOverVmax * stormTranslation.transitSpeedV();
  auto u = -wind_speed * std::cos(distance_azimuth.azimuth);
  auto v = wind_speed * std::sin(distance_azimuth.azimuth);

  auto phi = Solver::GahmEquations::phi(params.vmax_boundary_layer,
                                        params.radius_to_max_wind,
                                        params.holland_b, fc);
  auto p = Solver::GahmEquations::GahmPressure(
      pressure, Physical::Constants::backgroundPressure() * 100.0,
      distance_azimuth.distance, params.radius_to_max_wind,
      params.vmax_boundary_layer, phi);

  return {u, v, params.vmax_boundary_layer};
  return {u, v, p};
}

}  // namespace Gahm::Vortex

std::ostream &operator<<(std::ostream &os,
                         const Gahm::Vortex::Vortex::t_storm_parameters &p) {
  os << "   Vmax: " << p.vmax_boundary_layer << std::endl;
  os << "   Rmax: " << p.radius_to_max_wind << std::endl;
  os << "   Holland B: " << p.holland_b << std::endl;
  os << "   Rmax True: " << p.radius_to_max_winds_true << std::endl;
  return os;
}
