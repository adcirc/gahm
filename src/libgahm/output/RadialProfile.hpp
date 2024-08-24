//
// Created by Zach Cobell on 8/15/24.
//

#ifndef GAHM_RADIALPROFILE_HPP
#define GAHM_RADIALPROFILE_HPP

#include <vector>

#include "atcf/AtcfPeriod.hpp"
#include "datatypes/QuadCode.hpp"
#include "datatypes/Vec.hpp"
#include "storm/Quadrant.hpp"

namespace Gahm::Output::RadialProfile {

struct ProfilePoint {
  double distance{};
  double pressure{};
  Types::Vec wind_vector;
};

struct Profile {
  std::vector<ProfilePoint> data{};
};

[[nodiscard]] auto generate_distance_vector(
    double distance_start, double distance_end,
    double distance_step) -> std::vector<double>;

[[nodiscard]] auto get_profile(
    const Gahm::Atcf::AtcfPeriod& period,
    Gahm::Types::QuadCode::QuadrantCode quadrant_code, double distance_start,
    double distance_end, double distance_step) -> Profile;

[[nodiscard]] auto get_profile(const Gahm::Atcf::AtcfPeriod& period,
                               const Gahm::Storm::Quadrant& quadrant,
                               double distance_start, double distance_end,
                               double distance_step) -> Profile;

[[nodiscard]] auto get_profile(
    const Gahm::Atcf::AtcfPeriod& period, const Gahm::Storm::Quadrant& quadrant,
    const std::vector<double>& distance_pts) -> Profile;

}  // namespace Gahm::Output::RadialProfile

#endif  // GAHM_RADIALPROFILE_HPP
