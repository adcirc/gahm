//
// Created by Zach Cobell on 8/15/24.
//

#include "RadialProfile.h"

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <tuple>
#include <vector>

#include "atcf/AtcfPeriod.h"
#include "datatypes/Point.h"
#include "datatypes/QuadCode.h"
#include "datatypes/RotationMatrix.h"
#include "datatypes/Vec.h"
#include "gahm/GahmEquations.h"
#include "physical/Atmospheric.h"
#include "physical/Constants.h"
#include "storm/Quadrant.h"
#include "storm/StormTranslation.h"
#include "util/Interpolation.h"

namespace Gahm::Output::RadialProfile {

namespace detail {
/**
 * Find the isotachs that bound the distance
 * @param quadrant
 * @param distance
 * @return
 */
auto select_isotach(const Storm::Quadrant &quadrant, const double distance) {
  // Let's do this (below), but with standard algorithms
  //    auto isotach_lower = quadrant.valid_isotachs().begin();
  //    auto isotach_upper = std::next(isotach_lower);
  //    while (isotach_upper->radius() > distance) {
  //      isotach_lower = isotach_upper;
  //      isotach_upper++;
  //    }
  // The isotachs are arranged from the fastest isotach to the slowest, so
  // we will look for something like this:
  //   64        50        35
  //    |----x--------------|
  // where x is the distance we are looking for. We want to find the isotach
  // that is just below x and the isotach that is just above x. We can do
  // this by using std::upper_bound and std::prev.
  // First, check if the distance is outside the range of isotachs
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

auto solve_gahm(const Storm::Quadrant &quadrant, const double distance,
                const Types::Point &eye_location,
                const Storm::StormTranslation &translation,
                const double central_pressure, const double background_pressure,
                const double coriolis) -> RadialProfile::ProfilePoint {
  if (distance < 1.0) {
    return RadialProfile::ProfilePoint{distance, Types::Vec(0, 0),
                                       central_pressure};
  }

  const auto [isotach_lower, isotach_upper, isotach_ratio] =
      select_isotach(quadrant, distance);

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

  const auto wind_speed_tbl = Solver::GahmEquations::GahmWindSpeed(
      r_max, v_max, distance, coriolis, gahm_b);
  const auto pressure = Solver::GahmEquations::GahmPressure(
      central_pressure, background_pressure, distance, r_max, gahm_b, gahm_phi);

  auto wind_speed_10_10 =
      wind_speed_tbl * Physical::Constants::topOfBoundaryLayerToTenMeter();
  const auto turning_angle_matrix = Types::RotationMatrix(
      -Physical::Atmospheric::turning_angle(distance, r_max), eye_location.y());
  const auto unit_vector_10_ta =
      Types::Vec::matmul_22_21(turning_angle_matrix.data(), unit_vector);

  const auto ratio = std::min(1.0, wind_speed_10_10 / v_max);
  const auto background_vector = translation.velocity() * ratio;
  auto wind_vec_10_10 =
      unit_vector_10_ta * wind_speed_10_10 + background_vector;
  wind_speed_10_10 = wind_vec_10_10.magnitude();

  const auto s_max_10_10 =
      (Types::Vec::matmul_22_21(
           Types::RotationMatrix(-10, eye_location.y()).data(), unit_vector) *
       v_max)
          .magnitude();

  auto s_ratio = 1.0;
  if (wind_speed_10_10 > s_max_10_10) {
    s_ratio = s_max_10_10 / wind_speed_10_10;
  }
  wind_vec_10_10 = wind_vec_10_10 * s_ratio;

  return RadialProfile::ProfilePoint{distance, wind_vec_10_10, pressure};
}

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
}  // namespace detail

auto get_profile(const Atcf::AtcfPeriod &period,
                 Gahm::Types::QuadCode::QuadrantCode quadrant_code,
                 double distance_start, double distance_end,
                 double distance_step) -> Profile {
  return get_profile(
      period,
      period.quadrant(Types::QuadCode::quadrant_code_to_integer(quadrant_code)),
      distance_start, distance_end, distance_step);
}

auto get_profile(const Atcf::AtcfPeriod &period,
                 const Storm::Quadrant &quadrant, double distance_start,
                 double distance_end, double distance_step) -> Profile {
  const auto distance_pts = detail::generate_distance_vector(
      distance_start, distance_end, distance_step);

  Profile profile;
  profile.data.reserve(distance_pts.size());

  std::ranges::transform(distance_pts, std::back_inserter(profile.data),
                         [&](const auto &distance) {
                           return detail::solve_gahm(
                               quadrant, distance, period.eye_location(),
                               period.translation(), period.central_pressure(),
                               period.background_pressure(),
                               period.coriolis_force());
                         });

  return profile;
}

}  // namespace Gahm::Output::RadialProfile