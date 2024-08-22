//
// Created by Zach Cobell on 8/15/24.
//

#ifndef GAHM_RADIALPROFILE_H
#define GAHM_RADIALPROFILE_H

#include <vector>

#include "atcf/AtcfPeriod.h"
#include "datatypes/QuadCode.h"
#include "datatypes/Vec.h"
#include "storm/Quadrant.h"

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

[[nodiscard]] auto get_profile(const Atcf::AtcfPeriod& period,
                               Types::QuadCode::QuadrantCode quadrant_code,
                               double distance_start, double distance_end,
                               double distance_step) -> Profile;

[[nodiscard]] auto get_profile(const Atcf::AtcfPeriod& period,
                               const Storm::Quadrant& quadrant,
                               double distance_start, double distance_end,
                               double distance_step) -> Profile;

[[nodiscard]] auto get_profile(
    const Atcf::AtcfPeriod& period, const Storm::Quadrant& quadrant,
    const std::vector<double>& distance_pts) -> Profile;

}  // namespace Gahm::Output::RadialProfile

#endif  // GAHM_RADIALPROFILE_H
