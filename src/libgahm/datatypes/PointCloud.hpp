//
// Created by Zach Cobell on 8/22/24.
//

#ifndef GAHM_POINTCLOUD_HPP
#define GAHM_POINTCLOUD_HPP

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <vector>

#include "datatypes/Point.hpp"

namespace Gahm::Types {

class PointCloud {
 public:
  PointCloud() = default;

  PointCloud(const std::vector<double> &x_positions,
             const std::vector<double> &y_positions) {
    assert(x_positions.size() == y_positions.size());
    m_points.reserve(x_positions.size());
    std::transform(x_positions.begin(), x_positions.end(), y_positions.begin(),
                   std::back_inserter(m_points),
                   [](double x, double y) { return Point(x, y); });
  }

  [[nodiscard]] const std::vector<Gahm::Types::Point> &points() const { return m_points; }

  [[nodiscard]] auto x() const -> std::vector<double> {
    std::vector<double> x_vals;
    x_vals.reserve(m_points.size());
    std::transform(m_points.begin(), m_points.end(), std::back_inserter(x_vals),
                   [](const Point &point) { return point.x(); });
    return x_vals;
  }

  [[nodiscard]] auto y() const -> std::vector<double> {
    std::vector<double> y;
    y.reserve(m_points.size());
    std::transform(m_points.begin(), m_points.end(), std::back_inserter(y),
                   [](const Point &point) { return point.y(); });
    return y;
  }

  void addPoint(const Point &point) { m_points.push_back(point); }

  void addPoint(double x_pos, double y_pos) {
    m_points.emplace_back(x_pos, y_pos);
  }

  void clear() { m_points.clear(); }

  void removePoint(const Gahm::Types::Point &point) {
    auto iter = std::find(m_points.begin(), m_points.end(), point);
    if (iter != m_points.end()) {
      m_points.erase(iter);
    }
  }

  void reserve(size_t size) { m_points.reserve(size); }

#ifndef SWIG
  auto begin() { return m_points.begin(); }
  [[nodiscard]] auto begin() const { return m_points.begin(); }

  auto end() { return m_points.end(); }
  [[nodiscard]] auto end() const { return m_points.end(); }

  auto front() { return m_points.front(); }
  [[nodiscard]] auto front() const { return m_points.front(); }

  auto back() { return m_points.back(); }
  [[nodiscard]] auto back() const { return m_points.back(); }

  [[nodiscard]] auto operator==(const PointCloud &rhs) const -> bool {
    return m_points == rhs.m_points;
  }

  [[nodiscard]] auto operator!=(const PointCloud &rhs) const -> bool {
    return !(rhs == *this);
  }

  [[nodiscard]] auto operator[](size_t index) -> Point & {
    return m_points[index];
  }
  [[nodiscard]] auto operator[](size_t index) const -> const Point & {
    return m_points[index];
  }
#endif

  [[nodiscard]] auto size() const -> size_t { return m_points.size(); }

  [[nodiscard]] auto empty() const -> bool { return m_points.empty(); }

 private:
  std::vector<Gahm::Types::Point> m_points;
};

}  // namespace Gahm::Types

#endif  // GAHM_POINTCLOUD_HPP
