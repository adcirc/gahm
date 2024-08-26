//
// Created by Zach Cobell on 8/22/24.
//

#ifndef GAHM_POINTOUTPUT_HPP
#define GAHM_POINTOUTPUT_HPP

#include <cstddef>

#include "atcf/AtcfPeriod.hpp"
#include "atcf/AtcfTrack.hpp"
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

  [[nodiscard]] constexpr auto size() const noexcept -> size_t {
    return data.size();
  }
};

// Transforms the PointSolution into a PointSolutionArray (AOS-->SOA)
struct PointSolutionArray {
  std::vector<double> pressure;
  std::vector<double> wind_u;
  std::vector<double> wind_v;
  std::vector<double> wind_speed;

  explicit PointSolutionArray(
      const Gahm::Output::PointOutput::PointSolution &point_solution)
      : pressure(), wind_u(), wind_v(), wind_speed() {
    pressure.reserve(point_solution.size());
    wind_u.reserve(point_solution.size());
    wind_v.reserve(point_solution.size());
    std::for_each(point_solution.data.begin(), point_solution.data.end(),
                  [this](const SolutionPoint &point) {
                    pressure.push_back(point.pressure);
                    wind_u.push_back(point.wind_vector.u());
                    wind_v.push_back(point.wind_vector.v());
                    wind_speed.push_back(point.wind_vector.magnitude());
                  });
  }

  [[nodiscard]] constexpr auto size() const noexcept -> size_t {
    return pressure.size();
  }
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

[[nodiscard]] auto get_points(const Atcf::AtcfTrack &track,
                              const Types::Datetime &datetime,
                              const Types::PointCloud &points) -> PointSolution;

[[nodiscard]] auto get_points(const Atcf::AtcfTrack &track,
                              const Types::Datetime &datetime,
                              const Types::Grid &grid) -> PointSolution;

}  // namespace Gahm::Output::PointOutput
#endif  // GAHM_POINTOUTPUT_HPP
