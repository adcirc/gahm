//
// Created by Zach Cobell on 6/30/22.
//

#include "VortexSolution.h"

#include <utility>

#include "Atmospheric.h"
#include "GahmEquations.h"
#include "Interpolation.h"

using namespace Gahm;

VortexSolution::VortexSolution(const Gahm::Vortex *vortex,
                               std::vector<double> x_points,
                               std::vector<double> y_points)
    : m_vortex(vortex),
      m_xpoints(std::move(x_points)),
      m_ypoints(std::move(y_points)),
      m_distance(x_points.size(), 0.0),
      m_azimuth(x_points.size(), 0.0) {
  assert(m_xpoints.size() == m_ypoints.size());
}

Gahm::WindData VortexSolution::get(const Date &d) {
  WindData wind_field(m_xpoints.size(),
                      {0.0, 0.0, Physical::backgroundPressure()});

  if (d < m_vortex->atcf().data().begin()->date() ||
      d > (m_vortex->atcf().data().end() - 1)->date()) {
    return wind_field;
  }

  //...Need an expression here to trigger this for performance win
  // For now, it always updates
  bool update_distance_and_azimuth = true;

  const auto raw_cycle_data = this->getCycle(d);
  const auto cycle =
      Gahm::VortexSolution::interpolate_storm_parameters(raw_cycle_data);

  if (update_distance_and_azimuth) {
    this->compute_distance_and_azimuth(cycle);
  }

  auto [u_transit, v_transit] = cycle.storm_position.transitSpeedComponents();

  for (size_t i = 0; i < m_xpoints.size(); ++i) {
    const auto prior_weights =
        cycle.cycle_weight.this_cycle->selectIsotachAndQuadrant(m_distance[i],
                                                                m_azimuth[i]);
    const auto next_weights =
        cycle.cycle_weight.next_cycle->selectIsotachAndQuadrant(m_distance[i],
                                                                m_azimuth[i]);

    const auto solver_input = this->generate_solver_input_at_location(
        cycle, i, prior_weights, next_weights);

    auto coriolis = Physical::coriolis(m_ypoints[i]);
    auto phi = GahmEquations::phi(solver_input.vmax_at_boundary_layer,
                                  solver_input.radius_to_max_winds,
                                  solver_input.holland_b, coriolis);
    double wind_speed = GahmEquations::GahmFunction(
        solver_input.radius_to_max_winds, solver_input.vmax_at_boundary_layer,
        solver_input.isotach_speed, m_distance[i], coriolis,
        solver_input.holland_b, phi);

    double speed_over_vmax =
        std::abs(wind_speed / solver_input.vmax_at_boundary_layer);

    double tsx = speed_over_vmax * u_transit;
    double tsy = speed_over_vmax * v_transit;
    wind_speed *= Physical::windReduction();

    double wind_speed_u = -wind_speed * std::cos(m_azimuth[i]);
    double wind_speed_v = wind_speed * std::sin(m_azimuth[i]);

    double friction_angle = Atmospheric::queenslandInflowAngle(
        m_distance[i], solver_input.radius_to_max_winds_true);

    auto [wind_speed_u_rotated, wind_speed_v_rotated] =
        VortexSolution::rotate_winds(wind_speed_u, wind_speed_v, friction_angle,
                                     cycle.storm_position.latitude());

    double pressure = GahmEquations::GahmPressure(
        cycle.central_pressure, cycle.background_pressure, m_distance[i],
        solver_input.radius_to_max_winds, solver_input.holland_b, phi);

    wind_field.set(i, {wind_speed_u_rotated, wind_speed_v_rotated, pressure});
  }

  return wind_field;
}

VortexSolution::CycleWeight VortexSolution::getCycle(const Date &date) const {
  if (date <= m_vortex->atcf().data().begin()->date()) {
    return {m_vortex->atcf().data().begin(), m_vortex->atcf().data().begin(),
            0.0};
  } else if (date >= m_vortex->atcf().data().back().date()) {
    return {m_vortex->atcf().data().end() - 1,
            m_vortex->atcf().data().end() - 1, 0.0};
  } else {
    auto next_cycle = m_vortex->atcf().selectForecastPeriod(date);
    auto this_cycle = next_cycle - 1;
    double weight =
        static_cast<double>(next_cycle->date().toSeconds() - date.toSeconds()) /
        static_cast<double>(next_cycle->date().toSeconds() -
                            this_cycle->date().toSeconds());
    return {this_cycle, next_cycle, 1.0 - weight};
  }
}

VortexSolution::CycleParameters VortexSolution::interpolate_storm_parameters(
    const CycleWeight &cycle) {
  CycleParameters param;
  param.cycle_weight = cycle;
  if (cycle.this_cycle == cycle.next_cycle) {
    param.background_pressure = cycle.this_cycle->backgroundPressure();
    param.max_wind_speed = cycle.this_cycle->maximumWindVelocity();
    param.storm_position = cycle.this_cycle->stormPosition();
  } else {
    param.background_pressure = Interpolation::linearInterp(
        cycle.weight, cycle.this_cycle->backgroundPressure(),
        cycle.next_cycle->backgroundPressure());
    param.max_wind_speed = Interpolation::linearInterp(
        cycle.weight, cycle.this_cycle->maximumWindVelocity(),
        cycle.next_cycle->maximumWindVelocity());

    double longitude = Interpolation::linearInterp(
        cycle.weight, cycle.this_cycle->stormPosition().longitude(),
        cycle.next_cycle->stormPosition().longitude());
    double latitude = Interpolation::linearInterp(
        cycle.weight, cycle.this_cycle->stormPosition().latitude(),
        cycle.next_cycle->stormPosition().latitude());
    double forward_speed = Interpolation::linearInterp(
        cycle.weight, cycle.this_cycle->stormPosition().forwardSpeed(),
        cycle.next_cycle->stormPosition().forwardSpeed());
    double transit_direction = Interpolation::linearInterp(
        cycle.weight, cycle.this_cycle->stormPosition().transitDirection(),
        cycle.next_cycle->stormPosition().transitDirection());
    param.storm_position =
        StormPosition(longitude, latitude, forward_speed, transit_direction);
  }
  return param;
}

void VortexSolution::compute_distance_and_azimuth(
    const VortexSolution::CycleParameters &c) {
  constexpr double deg2rad = Units::convert(Units::Degree, Units::Radian);
  const double radius_earth = Physical::radiusEarth();
  const double cosStormCenterY =
      std::cos(deg2rad * c.storm_position.latitude());
  for (auto i = 0; i < m_xpoints.size(); ++i) {
    double dx = deg2rad * radius_earth *
                (m_xpoints[i] - c.storm_position.longitude()) * cosStormCenterY;
    double dy =
        deg2rad * radius_earth * (m_ypoints[i] - c.storm_position.latitude());
    m_distance[i] = std::sqrt(dx * dx + dy * dy);
    m_azimuth[i] = Constants::twopi() + std::atan2(dx, dy);
    if (m_azimuth[i] > Constants::twopi()) m_azimuth[i] -= Constants::twopi();
  }
}

VortexSolution::SolverInput VortexSolution::generate_solver_input_at_location(
    const CycleParameters &cycle, size_t query_index,
    const PositionWeights &prior_weights,
    const PositionWeights &next_weights) const {
  const auto prior_cycle_parameters = this->generate_solver_input_for_cycle(
      cycle.cycle_weight.this_cycle, query_index, prior_weights);
  const auto next_cycle_parameters = this->generate_solver_input_for_cycle(
      cycle.cycle_weight.next_cycle, query_index, next_weights);

  auto rmax = Interpolation::linearInterp(
      cycle.cycle_weight.weight, prior_cycle_parameters.radius_to_max_winds,
      next_cycle_parameters.radius_to_max_winds);
  auto rmax_true = Interpolation::linearInterp(
      cycle.cycle_weight.weight,
      prior_cycle_parameters.radius_to_max_winds_true,
      next_cycle_parameters.radius_to_max_winds_true);
  auto vmax = Interpolation::linearInterp(
      cycle.cycle_weight.weight, prior_cycle_parameters.vmax_at_boundary_layer,
      next_cycle_parameters.vmax_at_boundary_layer);
  auto iso_radius = Interpolation::linearInterp(
      cycle.cycle_weight.weight, prior_cycle_parameters.isotach_radius,
      next_cycle_parameters.isotach_radius);
  auto iso_speed = Interpolation::linearInterp(
      cycle.cycle_weight.weight, prior_cycle_parameters.isotach_speed,
      next_cycle_parameters.isotach_speed);
  auto b = Interpolation::linearInterp(cycle.cycle_weight.weight,
                                       prior_cycle_parameters.holland_b,
                                       next_cycle_parameters.holland_b);
  return {vmax, rmax, rmax_true, b, iso_speed, iso_radius};
}

VortexSolution::SolverInput VortexSolution::generate_solver_input_for_cycle(
    const cycle_iterator &cycle, size_t query_index,
    const PositionWeights &weights) const {
  static constexpr double deg2rad =
      Units::convert(Units::Degree, Units::Radian);
  static constexpr double angle_1 = deg2rad;
  static constexpr double angle_89 = 89.0 * deg2rad;
  static constexpr double angle_90 = 90.0 * deg2rad;

  if (weights.delta_angle() < angle_1) {
    return VortexSolution::interpolate_solver_parameters(
        cycle, weights, weights.this_isotach_iterators().first,
        weights.next_isotach_iterators().first);
  } else if (weights.delta_angle() > angle_89) {
    return VortexSolution::interpolate_solver_parameters(
        cycle, weights, weights.this_isotach_iterators().second,
        weights.next_isotach_iterators().second);
  } else {
    const auto s1 = VortexSolution::interpolate_solver_parameters(
        cycle, weights, weights.this_isotach_iterators().first,
        weights.next_isotach_iterators().first);
    const auto s2 = VortexSolution::interpolate_solver_parameters(
        cycle, weights, weights.this_isotach_iterators().second,
        weights.next_isotach_iterators().second);

    const auto [d1, d2] = weights.delta_angle_weights();

    double rmax = Interpolation::quadrantInterp(d1, d2, s1.radius_to_max_winds,
                                                s2.radius_to_max_winds);
    double rmaxtrue = Interpolation::quadrantInterp(
        d1, d2, s1.radius_to_max_winds_true, s2.radius_to_max_winds_true);
    double vmax = Interpolation::quadrantInterp(
        d1, d2, s1.vmax_at_boundary_layer, s2.vmax_at_boundary_layer);
    double b =
        Interpolation::quadrantInterp(d1, d2, s1.holland_b, s2.holland_b);
    double iso_radius = Interpolation::quadrantInterp(d1, d2, s1.isotach_radius,
                                                      s2.isotach_radius);
    double iso_speed = Interpolation::quadrantInterp(d1, d2, s1.isotach_speed,
                                                     s2.isotach_speed);
    return {vmax, rmax, rmaxtrue, b, iso_speed, iso_radius};
  }
}

VortexSolution::SolverInput VortexSolution::interpolate_solver_parameters(
    const cycle_iterator &cycle, const PositionWeights &weights,
    const StormQuadrant::quadrant_it<4> &it1,
    const StormQuadrant::quadrant_it<4> &it2) {
  if (weights.isotach() == -1) {
    auto b = Atmospheric::calcHollandB(cycle->maximumWindVelocity(),
                                       cycle->centralPressure(),
                                       cycle->backgroundPressure());
    return {cycle->maximumWindVelocity(), cycle->radiusToMaxWinds(),
            cycle->radiusToMaxWinds(),    b,
            cycle->maximumWindVelocity(), cycle->radiusToMaxWinds()};
  } else if (it1 == it2) {
    return {it1->vmax_at_boundary_layer(),
            it1->radius_to_max_winds(),
            it1->radius_to_max_winds(),
            it1->gahm_holland_b(),
            it1->isotach_speed_at_boundary_layer(),
            it1->isotach_radius()};
  } else {
    auto iso_weight = weights.isotach_weight();
    auto vmax =
        Interpolation::linearInterp(iso_weight, it1->vmax_at_boundary_layer(),
                                    it2->vmax_at_boundary_layer());
    auto rmax = Interpolation::linearInterp(
        iso_weight, it1->radius_to_max_winds(), it2->radius_to_max_winds());
    auto rmax_true = Interpolation::linearInterp(
        iso_weight, it1->radius_to_max_winds(), it2->radius_to_max_winds());
    auto b = Interpolation::linearInterp(iso_weight, it1->gahm_holland_b(),
                                         it2->gahm_holland_b());
    auto iso_speed = Interpolation::linearInterp(
        iso_weight, it1->isotach_speed_at_boundary_layer(),
        it2->isotach_speed_at_boundary_layer());
    auto iso_radius = Interpolation::linearInterp(
        iso_weight, it1->isotach_radius(), it2->isotach_radius());
    return {vmax, rmax, rmax_true, b, iso_speed, iso_radius};
  }
}

std::tuple<int, double> VortexSolution::get_base_quadrant(double angle) {
  auto angle2 = angle + Constants::quarterpi();
  if (angle2 > Constants::twopi()) angle2 -= Constants::twopi();
  auto quad = std::fmod(angle2 / Constants::halfpi(), 4);
  const double rem = angle2 - quad * Constants::halfpi();
  return {quad, rem};
}

std::tuple<double, double> VortexSolution::rotate_winds(const double u,
                                                        const double v,
                                                        double friction_angle,
                                                        const double latitude) {
  if (latitude < 0.0) friction_angle *= -1.0;
  const double cos_angle = std::cos(friction_angle);
  const double sin_angle = std::sin(friction_angle);
  return {u * cos_angle - v * sin_angle, u * sin_angle + v * cos_angle};
}