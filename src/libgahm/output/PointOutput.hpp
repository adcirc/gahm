//
// Created by Zach Cobell on 8/22/24.
//

#ifndef GAHM_POINTOUTPUT_HPP
#define GAHM_POINTOUTPUT_HPP

#include "atcf/AtcfPeriod.hpp"
#include "datatypes/Datetime.hpp"
#include "datatypes/Grid.hpp"
#include "datatypes/Point.hpp"
#include "datatypes/PointCloud.hpp"

namespace Gahm::Output::PointOutput {

struct SolutionPoint {
  double pressure;
  Types::Vec wind_vector;
};

struct PointSolution {
  std::vector<SolutionPoint> data{};
};

[[nodiscard]] auto get_points(const Atcf::AtcfPeriod &period,
                              const Types::PointCloud &points) -> PointSolution;

[[nodiscard]] auto get_points(const Atcf::AtcfPeriod &period,
                              const Types::Grid &grid) -> PointSolution;

[[nodiscard]] auto get_points(const Atcf::AtcfPeriod &period_1,
                              const Atcf::AtcfPeriod &period_2,
                              const Types::Datetime &datetime,
                              const Types::PointCloud &points) -> PointSolution;

[[nodiscard]] auto get_points(const Atcf::AtcfPeriod &period_1,
                              const Atcf::AtcfPeriod &period_2,
                              const Types::Datetime &datetime,
                              const Types::Grid &grid) -> PointSolution;

}  // namespace Gahm::Output::PointOutput
#endif  // GAHM_POINTOUTPUT_HPP
