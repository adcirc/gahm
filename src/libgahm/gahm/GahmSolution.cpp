//
// Created by Zach Cobell on 8/16/24.
//

#include "GahmSolution.h"

#include <algorithm>
#include <array>
#include <cmath>
#include <iterator>
#include <tuple>
#include <vector>

#include "datatypes/Point.h"
#include "datatypes/RotationMatrix.h"
#include "datatypes/Vec.h"
#include "gahm/GahmEquations.h"
#include "physical/Atmospheric.h"
#include "physical/Constants.h"
#include "physical/Earth.h"
#include "storm/Quadrant.h"
#include "storm/StormTranslation.h"
#include "util/Interpolation.h"

namespace Gahm::Solver::Solution {

namespace {
/**
 * @brief Find the isotachs that bound the distance
 *
 * The isotachs are arranged from the fastest isotach to the slowest, so
 * we will look for something like this:
 *  64        50        35
 *  |----x----|---------|
 * where x is the distance we are looking for. We want to find the isotach
 * that is just below x and the isotach that is just above x. We can do
 * this by using std::upper_bound and std::prev
 *
 * @param quadrant Quadrant object
 * @param distance Distance from the storm center
 * @return Tuple containing the lower isotach, upper isotach, and the weighting
 */
auto select_isotach(const Storm::Quadrant &quadrant, const double distance) {
  if (distance >= quadrant.valid_isotachs().begin()->radius() ||
      quadrant.valid_isotachs().size() == 1) {
    return std::make_tuple(quadrant.valid_isotachs().begin(),
                           quadrant.valid_isotachs().begin(), 0.0);
  }

  if (distance <= quadrant.valid_isotachs().back().radius()) {
    return std::make_tuple(std::prev(quadrant.valid_isotachs().end()),
                           std::prev(quadrant.valid_isotachs().end()), 0.0);
  }

  auto isotach_faster =
      std::lower_bound(quadrant.valid_isotachs().rbegin(),
                       quadrant.valid_isotachs().rend(), distance,
                       [](const auto &isotach, const auto &dis) {
                         return isotach.radius() < dis;
                       })
          .base();
  auto isotach_slower = std::prev(isotach_faster);
  const auto isotach_ratio =
      (distance - isotach_slower->radius()) /
      (isotach_faster->radius() - isotach_slower->radius());

  return std::make_tuple(isotach_slower, isotach_faster, isotach_ratio);
}

struct IsotachParams {
  double radius_to_max_winds{};
  double vortex_max_10_tbl{};
  double vortex_max_10_10{};
  double gahm_b{};
  double gahm_phi{};
  Types::Vec unit_vector_tbl;
};

auto get_isotach_params(
    const std::vector<Storm::Isotach>::const_iterator &isotach_lower,
    const std::vector<Storm::Isotach>::const_iterator &isotach_upper,
    double isotach_ratio) -> IsotachParams {
  const auto r_max = Gahm::Util::Interpolation::linear(
      isotach_lower->gahm_parameters().radius_to_max_winds(),
      isotach_upper->gahm_parameters().radius_to_max_winds(), isotach_ratio);
  const auto v_max = Gahm::Util::Interpolation::linear(
      isotach_lower->gahm_parameters().vortex_max_10_tbl(),
      isotach_upper->gahm_parameters().vortex_max_10_tbl(), isotach_ratio);
  const auto v_max_10_10 = Gahm::Util::Interpolation::linear(
      isotach_lower->gahm_parameters().vortex_max_10_10(),
      isotach_upper->gahm_parameters().vortex_max_10_10(), isotach_ratio);
  const auto gahm_b = Gahm::Util::Interpolation::linear(
      isotach_lower->gahm_parameters().gahm_b(),
      isotach_upper->gahm_parameters().gahm_b(), isotach_ratio);
  const auto gahm_phi = Gahm::Util::Interpolation::linear(
      isotach_lower->gahm_parameters().gahm_phi(),
      isotach_upper->gahm_parameters().gahm_phi(), isotach_ratio);
  const auto unit_vector = Gahm::Util::Interpolation::linear(
      isotach_lower->gahm_parameters().unit_vector_tbl(),
      isotach_upper->gahm_parameters().unit_vector_tbl(), isotach_ratio);

  return IsotachParams{.radius_to_max_winds = r_max,
                       .vortex_max_10_tbl = v_max,
                       .vortex_max_10_10 = v_max_10_10,
                       .gahm_b = gahm_b,
                       .gahm_phi = gahm_phi,
                       .unit_vector_tbl = unit_vector};
}

constexpr auto select_quadrant(const std::array<Storm::Quadrant, 4> &quadrants,
                               double angle)
    -> std::tuple<const Storm::Quadrant &, const Storm::Quadrant &, double> {
  constexpr double deg2rad = Physical::Constants::deg2rad();
  constexpr double angle_45 = 45.0 * deg2rad;
  constexpr double angle_135 = 135.0 * deg2rad;
  constexpr double angle_225 = 225.0 * deg2rad;
  constexpr double angle_315 = 315.0 * deg2rad;

  if (angle < angle_45) {
    return {quadrants[0], quadrants[1], angle_45 + angle};
  } else if (angle <= angle_135) {
    return {quadrants[1], quadrants[2], angle - angle_45};
  } else if (angle <= angle_225) {
    return {quadrants[2], quadrants[3], angle - angle_135};
  } else if (angle <= angle_315) {
    return {quadrants[3], quadrants[0], angle - angle_225};
  } else {
    return {quadrants[0], quadrants[1], angle - angle_315};
  }
}

auto interpolate_storm_params(const IsotachParams &p1, const IsotachParams &p2,
                              double angle) -> IsotachParams {
  constexpr double angle_90 = 90.0 * Physical::Constants::deg2rad();
  const double nd0 = 1.0 / std::pow(angle, 2.0);
  const double nd1 = 1.0 / std::pow(angle_90 - angle, 2.0);
  const double den = 1.0 / (nd0 + nd1);

  return IsotachParams{
      .radius_to_max_winds =
          (p1.radius_to_max_winds * nd0 + p2.radius_to_max_winds * nd1) * den,
      .vortex_max_10_tbl =
          (p1.vortex_max_10_tbl * nd0 + p2.vortex_max_10_tbl * nd1) * den,
      .vortex_max_10_10 =
          (p1.vortex_max_10_10 * nd0 + p2.vortex_max_10_10 * nd1) * den,
      .gahm_b = (p1.gahm_b * nd0 + p2.gahm_b * nd1) * den,
      .gahm_phi = (p1.gahm_phi * nd0 + p2.gahm_phi * nd1) * den,
      .unit_vector_tbl =
          (p1.unit_vector_tbl * nd0 + p2.unit_vector_tbl * nd1) * den};
}

/**
 * @brief Get the solution parameters for the GAHM equations
 * @param input Input parameters for the solver
 * @param storm_params Isotach parameters passed to the solver
 * @param distance Distance from the storm center
 * @return Tuple containing the wind speed and pressure
 */
auto get_solution_parameters(const GahmInputParamsBase &input,
                             const IsotachParams &storm_params,
                             double distance) -> std::tuple<double, double> {
  return {Solver::GahmEquations::GahmWindSpeed(
              storm_params.radius_to_max_winds, storm_params.vortex_max_10_tbl,
              distance, input.coriolis, storm_params.gahm_b),
          Solver::GahmEquations::GahmPressure(
              input.central_pressure, input.background_pressure, distance,
              storm_params.radius_to_max_winds, storm_params.gahm_b,
              storm_params.gahm_phi)};
}

auto interpolate_quadrants(const std::array<Storm::Quadrant, 4> &quadrants,
                           const Types::Point &eye_location,
                           const Types::Point &point,
                           double distance) -> IsotachParams {
  const auto azimuth = Physical::Earth::azimuth(eye_location, point);

  const auto [quadrant_first, quadrant_second, quadrant_angle] =
      select_quadrant(quadrants, azimuth);
  const auto [isotach_first_lower, isotach_first_upper, isotach_first_ratio] =
      select_isotach(quadrant_first, distance);
  const auto [isotach_second_lower, isotach_second_upper,
              isotach_second_ratio] = select_isotach(quadrant_second, distance);
  const auto storm_params_first = get_isotach_params(
      isotach_first_lower, isotach_first_upper, isotach_first_ratio);
  const auto storm_params_second = get_isotach_params(
      isotach_second_lower, isotach_second_upper, isotach_second_ratio);

  return interpolate_storm_params(storm_params_first, storm_params_second,
                                  quadrant_angle);
}

/**
 * @brief Add the turning angle and background velocity to the wind vector
 * @param input Input parameters for the solver
 * @param storm_params Isotach parameters passed to the solver
 * @param distance Distance from the storm center
 * @param wind_speed_10_10 Wind speed at 10m
 * @return Wind vector at 10m with the turning angle and background wind speed
 * added
 */
auto add_turning_angle_to_wind_vector(
    const GahmInputParamsBase &input, const IsotachParams &storm_params,
    const double distance, const double wind_speed_10_10) -> Types::Vec {
  // Generate the rotation matrix based on the calculated turning angle
  const auto turning_angle_matrix =
      Types::RotationMatrix(-Physical::Atmospheric::turning_angle(
                                distance, storm_params.radius_to_max_winds),
                            input.eye_location.y());

  const auto v_vor_quad_uv = Types::Vec::matmul_22_21(
      turning_angle_matrix.data(), storm_params.unit_vector_tbl);
  const auto v_vor_rad_prof_10_10 = v_vor_quad_uv * wind_speed_10_10;

  return v_vor_rad_prof_10_10;
}

/**
 * @brief Add the background velocity vector to the wind vector
 * @param vel_10_10 Wind vector at 10m with the turning angle added
 * @param v_max_10_10 Maximum wind speed at 10m
 * @param translation Storm translation object
 * @return Wind vector at 10m with the turning angle and background wind speed
 */
auto add_background_velocity_to_wind_vector(
    const Types::Vec &vel_10_10, const double v_max_10_10,
    const Storm::StormTranslation &translation) -> Types::Vec {
  const auto s_env_ratio = vel_10_10.magnitude() / v_max_10_10;
  const auto v_env_10_10 = translation.velocity() * s_env_ratio;

  return vel_10_10 + v_env_10_10;
}

/**
 * @brief Limit the quadrant profile wind speed
 *
 * The turning angle can cause the profile velocity to exceed the specified
 * Vmax when adjusted for the radial position around the storm and the
 * environmental velocity. Cap the profile velocity in this situation.
 *
 * @param input Input parameters for the solver
 * @param storm_params Isotach parameters passed to the solver
 * @param vel_10_10 velocity at 10m with the turning angle and background wind
 * speed added
 * @return Wind vector at 10m with the turning angle and background wind speed
 * added
 */
auto limit_quadrant_profile_wind_speed(
    const GahmInputParamsBase &input, const IsotachParams &storm_params,
    const Types::Vec &vel_10_10) -> Types::Vec {
  const auto v_vor_max_10_10 =
      (Types::Vec::matmul_22_21(
           Types::RotationMatrix(-10 * Physical::Constants::deg2rad(),
                                 input.eye_location.y())
               .data(),
           storm_params.unit_vector_tbl) *
       storm_params.vortex_max_10_10);
  const auto v_max_10_10_rp = v_vor_max_10_10 + input.translation.velocity();
  const auto s_max_10_10_rp = v_max_10_10_rp.magnitude();
  const auto s_vel_10_10 = vel_10_10.magnitude();

  // Scaling factor for the wind speed
  const auto s_ratio =
      s_vel_10_10 > s_max_10_10_rp ? s_max_10_10_rp / s_vel_10_10 : 1.0;

  return vel_10_10 * s_ratio;
}

/**
 * @brief Transform the wind vector from the top of the boundary layer to 10m
 * and adjust for the background wind speed
 * @param input Input parameters for the solver
 * @param storm_params Isotach parameters passed to the solver
 * @param distance Distance from the storm center
 * @param wind_speed_tbl Wind speed at the top of the boundary layer
 * @return Wind vector at 10m
 */
auto transform_wind_vector(const GahmInputParamsBase &input,
                           const IsotachParams &storm_params,
                           const double distance,
                           double wind_speed_tbl) -> Types::Vec {
  // Convert the wind speed from the top of the boundary layer to 10m
  auto wind_speed_10_10 =
      wind_speed_tbl * Physical::Constants::topOfBoundaryLayerToTenMeter();

  // Add the turning angle to the wind vector
  const auto vel_10_10 = add_turning_angle_to_wind_vector(
      input, storm_params, distance, wind_speed_10_10);

  // Add the background velocity to the wind vector
  const auto vel_10_10_env = add_background_velocity_to_wind_vector(
      vel_10_10, storm_params.vortex_max_10_10, input.translation);

  // Limit the wind speed if necessary
  return limit_quadrant_profile_wind_speed(input, storm_params, vel_10_10_env);
}

}  // namespace

/**
 * Solve the gahm equations for a given distance and quadrant
 *
 * @param input Input parameters for the solver
 * @return Solution point
 */
auto get(const Solver::Solution::GahmInputParamsQuadrant &input)
    -> Solver::Solution::GahmSolutionPoint {
  // If the distance is less than 1m, then we are at the eye of the storm and
  // the wind speed is 0, and the pressure is the central pressure. This avoids
  // division by zero in the equations
  if (input.distance < 1.0) {
    return Solver::Solution::GahmSolutionPoint{Types::Vec(0, 0),
                                               input.central_pressure / 100.0};
  }

  // Find the isotachs that bound the distance we are looking for
  const auto [isotach_lower, isotach_upper, isotach_ratio] =
      select_isotach(input.quadrant, input.distance);

  // Get the isotach parameters for the solver
  const auto storm_params =
      get_isotach_params(isotach_lower, isotach_upper, isotach_ratio);

  // Get the wind speed and pressure for the solution
  const auto [wind_speed_tbl, pressure] =
      get_solution_parameters(input, storm_params, input.distance);

  // Transform the wind vector to 10m and adjust for the background wind speed
  const auto wind_vec_10_10 = transform_wind_vector(
      input, storm_params, input.distance, wind_speed_tbl);

  // Return the solution point
  return Solver::Solution::GahmSolutionPoint{wind_vec_10_10, pressure / 100.0};
}

auto get(const Solver::Solution::GahmInputParamsPoint &input)
    -> GahmSolutionPoint {
  const auto distance =
      Physical::Earth::distance(input.point, input.eye_location);

  // If the distance is less than 1m, then we are at the eye of the storm and
  // will avoid division by zero
  if (distance < 1.0) {
    return GahmSolutionPoint{Types::Vec(0.0, 0.0),
                             input.central_pressure / 100.0};
  }

  // Interpolate the quadrant data
  const auto storm_params = interpolate_quadrants(
      input.quadrants, input.eye_location, input.point, distance);

  // Get the wind speed and pressure for the solution
  const auto [wind_speed_tbl, pressure] =
      get_solution_parameters(input, storm_params, distance);

  // Transform the wind vector to 10m and adjust for the background wind speed
  const auto wind_vec_10_10 =
      transform_wind_vector(input, storm_params, distance, wind_speed_tbl);

  // Return the solution point
  return Solver::Solution::GahmSolutionPoint{wind_vec_10_10, pressure / 100.0};
}

}  // namespace Gahm::Solver::Solution