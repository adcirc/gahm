//
// Created by Zach Cobell on 8/22/24.
//

#ifndef GAHM_POINTOUTPUT_H
#define GAHM_POINTOUTPUT_H

#include "atcf/AtcfPeriod.h"
#include "datatypes/Datetime.h"
#include "datatypes/Grid.h"
#include "datatypes/Point.h"
#include "datatypes/PointCloud.h"

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
#endif  // GAHM_POINTOUTPUT_H
