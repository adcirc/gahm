//
// Created by Zach Cobell on 8/22/24.
//

#include "PointOutput.h"

#include <algorithm>

#include "atcf/AtcfPeriod.h"
#include "datatypes/Grid.h"
#include "datatypes/Point.h"
#include "datatypes/PointCloud.h"
#include "gahm/GahmSolution.h"

namespace {

auto get_solution_point(const Gahm::Atcf::AtcfPeriod &period,
                        const Gahm::Types::Point &point)
    -> Gahm::Output::PointOutput::SolutionPoint {
  const auto pt =
      Gahm::Solver::Solution::get(Gahm::Solver::Solution::GahmInputParamsPoint{
          period.quadrants(), point, period.eye_location(),
          period.translation(), period.central_pressure(),
          period.background_pressure(), period.coriolis_force()});
  return {.pressure = pt.pressure, .wind_vector = pt.wind_vector};
}

}  // namespace

namespace Gahm::Output::PointOutput {

[[nodiscard]] auto get_points(const Atcf::AtcfPeriod &period,
                              const Types::PointCloud &points)
    -> PointSolution {
  PointSolution solution;
  solution.data.reserve(points.points().size());

  std::transform(points.points().begin(), points.points().end(),
                 std::back_inserter(solution.data),
                 [&](const Types::Point &point) {
                   return get_solution_point(period, point);
                 });

  return solution;
}

[[nodiscard]] auto get_points(const Atcf::AtcfPeriod &period,
                              const Types::Grid &grid) -> PointSolution {
  return get_points(period, grid.points());
}

}  // namespace Gahm::Output::PointOutput