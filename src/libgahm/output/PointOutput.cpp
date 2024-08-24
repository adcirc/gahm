//
// Created by Zach Cobell on 8/22/24.
//

#include "PointOutput.hpp"

#include <algorithm>

#include "atcf/AtcfPeriod.hpp"
#include "datatypes/Datetime.hpp"
#include "datatypes/Grid.hpp"
#include "datatypes/Point.hpp"
#include "datatypes/PointCloud.hpp"
#include "gahm/GahmSolution.hpp"

namespace {

auto get_solution_point(const Gahm::Atcf::AtcfPeriod &period,
                        const Gahm::Types::Point &point)
    -> Gahm::Output::PointOutput::SolutionPoint {
  const auto sln =
      Gahm::Solver::Solution::get(Gahm::Solver::Solution::GahmInputParamsPoint{
          period.quadrants(), point, period.eye_location(),
          period.translation(), period.central_pressure(),
          period.background_pressure(), period.coriolis_force()});
  return {.pressure = sln.pressure, .wind_vector = sln.wind_vector};
}

[[nodiscard]] auto interpolate_two_snaps(const Gahm::Atcf::AtcfPeriod &period_1,
                                         const Gahm::Atcf::AtcfPeriod &period_2,
                                         const double weight,
                                         const Gahm::Types::PointCloud &points)
    -> Gahm::Output::PointOutput::PointSolution {
  Gahm::Output::PointOutput::PointSolution solution;
  solution.data.reserve(points.points().size());
  std::transform(points.points().begin(), points.points().end(),
                 std::back_inserter(solution.data),
                 [&](const Gahm::Types::Point &point) {
                   const auto point_1 = get_solution_point(period_1, point);
                   const auto point_2 = get_solution_point(period_2, point);
                   return Gahm::Output::PointOutput::SolutionPoint{
                       .pressure = (point_1.pressure * (1.0 - weight)) +
                                   (point_2.pressure * weight),
                       .wind_vector = point_1.wind_vector * (1.0 - weight) +
                                      point_2.wind_vector * weight};
                 });
  return solution;
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

[[nodiscard]] auto get_points(
    const Atcf::AtcfPeriod &period_1, const Atcf::AtcfPeriod &period_2,
    const Types::Datetime &datetime,
    const Types::PointCloud &points) -> PointSolution {
  if (datetime <= period_1.datetime()) {
    return get_points(period_1, points);
  }

  if (datetime >= period_2.datetime()) {
    return get_points(period_2, points);
  }

  // We need to check if we can shortcut this by interpolating the snap. This
  // means that we have the exact same number of isotachs in each quadrant. If
  // we don't, then this is a brute force operation, and we end up doubling our
  // work.
  const auto interpolated_period =
      Atcf::AtcfPeriod::interpolate(period_1, period_2, datetime);

  if (interpolated_period.has_value()) {
    return get_points(interpolated_period.value(), points);
  }

  const auto weight =
      static_cast<double>(datetime.seconds_since_epoch() -
                          period_1.datetime().seconds_since_epoch()) /
      static_cast<double>(period_2.datetime().seconds_since_epoch() -
                          period_1.datetime().seconds_since_epoch());

  // If we can't interpolate the snaps, we need to brute force the solution.
  return interpolate_two_snaps(period_1, period_2, weight, points);
}

[[nodiscard]] auto get_points(const Atcf::AtcfPeriod &period_1,
                              const Atcf::AtcfPeriod &period_2,
                              const Types::Datetime &datetime,
                              const Types::Grid &grid) -> PointSolution {
  return get_points(period_1, period_2, datetime, grid.points());
}

}  // namespace Gahm::Output::PointOutput