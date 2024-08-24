//
// Created by Zach Cobell on 8/15/24.
//

#include "RadialProfile.hpp"

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <numeric>
#include <vector>

#include "atcf/AtcfPeriod.hpp"
#include "datatypes/QuadCode.hpp"
#include "gahm/GahmSolution.hpp"
#include "storm/Quadrant.hpp"

namespace Gahm::Output::RadialProfile {

/**
 * @brief Generate a vector of distances from the storm center
 * @param distance_start Starting distance from the storm center
 * @param distance_end Ending distance from the storm center
 * @param distance_step Step size between each point
 * @return Vector of distances
 */
auto generate_distance_vector(double distance_start, double distance_end,
                              double distance_step) -> std::vector<double> {
  std::vector<double> distance_pts;
  const auto n_pts = static_cast<size_t>(
      std::ceil((distance_end - distance_start) / distance_step));
  distance_pts.reserve(n_pts + 1);
  std::ranges::generate_n(
      std::back_inserter(distance_pts), static_cast<long>(n_pts) + 1, [&]() {
        return distance_start +
               distance_step * static_cast<double>(distance_pts.size());
      });
  return distance_pts;
}

/**
 * @brief Get the wind speed and pressure along a radial profile
 * @overload get_profile
 * @param period Atcf time snap
 * @param quadrant_code Quadrant where the profile is located
 * @param distance_start The distance from the storm center to start the profile
 * @param distance_end The distance from the storm center to end the profile
 * @param distance_step The step size between each point in the profile
 * @return Profile object containing the wind speed and pressure
 */
auto get_profile(const Atcf::AtcfPeriod &period,
                 Gahm::Types::QuadCode::QuadrantCode quadrant_code,
                 double distance_start, double distance_end,
                 double distance_step) -> Profile {
  return get_profile(
      period,
      period.quadrant(Types::QuadCode::quadrant_code_to_integer(quadrant_code)),
      distance_start, distance_end, distance_step);
}

/**
 * @overload get_profile
 * @param period Atcf time snap
 * @param quadrant Quadrant where the profile is located
 * @param distance_start The distance from the storm center to start the profile
 * @param distance_end The distance from the storm center to end the profile
 * @param distance_step The step size between each point in the profile
 * @return Profile object containing the wind speed and pressure
 */
auto get_profile(const Atcf::AtcfPeriod &period,
                 const Storm::Quadrant &quadrant, double distance_start,
                 double distance_end, double distance_step) -> Profile {
  return get_profile(
      period, quadrant,
      generate_distance_vector(distance_start, distance_end, distance_step));
}

/**
 * @overload get_profile
 * @param period Atcf time snap
 * @param quadrant Quadrant where the profile is located
 * @param distance_pts Vector of distances from the storm center
 * @return Profile object containing the wind speed and pressure
 */
auto get_profile(const Atcf::AtcfPeriod &period,
                 const Storm::Quadrant &quadrant,
                 const std::vector<double> &distance_pts) -> Profile {
  Profile profile;
  profile.data.reserve(distance_pts.size());

  std::ranges::transform(
      distance_pts, std::back_inserter(profile.data),
      [&](const auto &distance) {
        const auto sln =
            Solver::Solution::get(Solver::Solution::GahmInputParamsQuadrant{
                quadrant, distance, period.eye_location(), period.translation(),
                period.central_pressure(), period.background_pressure(),
                period.coriolis_force()});
        return ProfilePoint{distance, sln.pressure, sln.wind_vector};
      });

  return profile;
}

}  // namespace Gahm::Output::RadialProfile