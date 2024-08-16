//
// Created by Zach Cobell on 8/16/24.
//

#include "GahmSolution.h"

#include <algorithm>
#include <tuple>
#include <vector>

#include "datatypes/RotationMatrix.h"
#include "datatypes/Vec.h"
#include "gahm/GahmEquations.h"
#include "gahm/GahmSolution.h"
#include "physical/Atmospheric.h"
#include "physical/Constants.h"
#include "storm/Quadrant.h"
#include "util/Interpolation.h"

namespace Gahm::Solver::Solution {

namespace detail {
/**
 * @brief Find the isotachs that bound the distance
 *
 * Let's do this (below), but with standard algorithms
 *   auto isotach_lower = quadrant.valid_isotachs().begin();
 *   auto isotach_upper = std::next(isotach_lower);
 *   while (isotach_upper->radius() > distance) {
 *     isotach_lower = isotach_upper;
 *     isotach_upper++;
 *   }
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
  if (distance >= quadrant.valid_isotachs().begin()->radius()) {
    return std::make_tuple(quadrant.valid_isotachs().begin(),
                           quadrant.valid_isotachs().begin(), 0.0);
  } else if (distance <= quadrant.valid_isotachs().back().radius()) {
    return std::make_tuple(std::prev(quadrant.valid_isotachs().end()),
                           std::prev(quadrant.valid_isotachs().end()), 0.0);
  } else {
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
}

struct IsotachParams {
  double radius_to_max_winds;
  double vortex_max_10_tbl;
  double gahm_b;
  double gahm_phi;
  Types::Vec unit_vector_tbl;
};

constexpr auto get_isotach_params(
    const std::vector<Storm::Isotach>::const_iterator &isotach_lower,
    const std::vector<Storm::Isotach>::const_iterator &isotach_upper,
    double isotach_ratio) -> IsotachParams {
  const auto r_max = Gahm::Util::Interpolation::linear(
      isotach_lower->radius_to_max_winds(),
      isotach_upper->radius_to_max_winds(), isotach_ratio);
  const auto v_max = Gahm::Util::Interpolation::linear(
      isotach_lower->vortex_max_10_tbl(), isotach_upper->vortex_max_10_tbl(),
      isotach_ratio);
  const auto gahm_b = Gahm::Util::Interpolation::linear(
      isotach_lower->gahm_b(), isotach_upper->gahm_b(), isotach_ratio);
  const auto gahm_phi = Gahm::Util::Interpolation::linear(
      isotach_lower->gahm_phi(), isotach_upper->gahm_phi(), isotach_ratio);
  const auto unit_vector = Gahm::Util::Interpolation::linear(
      isotach_lower->unit_vector_tbl(), isotach_upper->unit_vector_tbl(),
      isotach_ratio);

  return IsotachParams{r_max, v_max, gahm_b, gahm_phi, unit_vector};
}

/**
 * @brief Get the solution parameters for the GAHM equations
 * @param input Input parameters for the solver
 * @param storm_params Isotach parameters passed to the solver
 * @return Tuple containing the wind speed and pressure
 */
std::tuple<double, double> get_solution_parameters(
    const GahmInputParams &input, const detail::IsotachParams &storm_params) {
  return {Solver::GahmEquations::GahmWindSpeed(
              storm_params.radius_to_max_winds, storm_params.vortex_max_10_tbl,
              input.distance, input.coriolis, storm_params.gahm_b),
          Solver::GahmEquations::GahmPressure(
              input.central_pressure, input.background_pressure, input.distance,
              storm_params.radius_to_max_winds, storm_params.gahm_b,
              storm_params.gahm_phi)};
}

/**
 * @brief Transform the wind vector from the top of the boundary layer to 10m
 * and adjust for the background wind speed
 * @param input Input parameters for the solver
 * @param storm_params Isotach parameters passed to the solver
 * @param wind_speed_tbl Wind speed at the top of the boundary layer
 * @return Wind vector at 10m
 */
auto transform_wind_vector(const GahmInputParams &input,
                           const detail::IsotachParams &storm_params,
                           double wind_speed_tbl) -> Types::Vec {
  // Convert the wind speed from the top of the boundary layer to 10m
  auto wind_speed_10_10 =
      wind_speed_tbl * Physical::Constants::topOfBoundaryLayerToTenMeter();

  // Generate the rotation matrix based on the calculated turning angle
  const auto turning_angle_matrix = Types::RotationMatrix(
      -Physical::Atmospheric::turning_angle(input.distance,
                                            storm_params.radius_to_max_winds),
      input.eye_location.y());

  // Rotate the unit vector to account for the turning angle
  const auto unit_vector_10_ta = Types::Vec::matmul_22_21(
      turning_angle_matrix.data(), storm_params.unit_vector_tbl);

  // Adjust the wind vector for the background wind speed
  const auto ratio =
      std::min(1.0, wind_speed_10_10 / storm_params.vortex_max_10_tbl);
  const auto background_vector = input.translation.velocity() * ratio;
  auto wind_vec_10_10 =
      unit_vector_10_ta * wind_speed_10_10 + background_vector;
  wind_speed_10_10 = wind_vec_10_10.magnitude();

  // Adjust the wind vector for the storm speed
  const auto s_max_10_10 =
      (Types::Vec::matmul_22_21(
           Types::RotationMatrix(-10, input.eye_location.y()).data(),
           storm_params.unit_vector_tbl) *
       storm_params.vortex_max_10_tbl)
          .magnitude();

  const auto s_ratio = [&] {
    if (wind_speed_10_10 > s_max_10_10) {
      return s_max_10_10 / wind_speed_10_10;
    }
    return 1.0;
  }();

  return wind_vec_10_10 * s_ratio;
}

}  // namespace detail

/**
 * Solve the gahm equations for a given distance and quadrant
 *
 * @param input Input parameters for the solver
 * @return Solution point
 */
auto get(const Solver::Solution::GahmInputParams &input)
    -> Solver::Solution::GahmSolutionPoint {
  // If the distance is less than 1m, then we are at the eye of the storm and
  // the wind speed is 0, and the pressure is the central pressure. This avoids
  // division by zero in the equations
  if (input.distance < 1.0) {
    return Solver::Solution::GahmSolutionPoint{Types::Vec(0, 0),
                                               input.central_pressure};
  }

  // Find the isotachs that bound the distance we are looking for
  const auto [isotach_lower, isotach_upper, isotach_ratio] =
      detail::select_isotach(input.quadrant, input.distance);

  // Get the isotach parameters for the solver
  const auto storm_params =
      detail::get_isotach_params(isotach_lower, isotach_upper, isotach_ratio);

  // Get the wind speed and pressure for the solution
  const auto [wind_speed_tbl, pressure] =
      detail::get_solution_parameters(input, storm_params);

  // Transform the wind vector to 10m and adjust for the background wind speed
  const auto wind_vec_10_10 =
      detail::transform_wind_vector(input, storm_params, wind_speed_tbl);

  // Return the solution point
  return Solver::Solution::GahmSolutionPoint{wind_vec_10_10, pressure};
}

}  // namespace Gahm::Solver::Solution