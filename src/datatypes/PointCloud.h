// GNU General Public License v3.0
//
// This file is part of the GAHM model (https://github.com/adcirc/gahm).
// Copyright (c) 2023 ADCIRC Development Group.
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, version 3.
//
// This program is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program. If not, see <http://www.gnu.org/licenses/>.
//
// Author: Zach Cobell
// Contact: zcobell@thewaterinstitute.org
//
#ifndef GAHM_POINTCLOUD_H
#define GAHM_POINTCLOUD_H

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <vector>

#include "datatypes/Point.h"

#ifdef SWIG
#define NODISCARD
#else
#define NODISCARD [[nodiscard]]
#endif

namespace Gahm::Datatypes {

class PointCloud {
 public:
  PointCloud() = default;

  PointCloud(const std::vector<double> &x_positions,
             const std::vector<double> &y_positions) {
    assert(x_positions.size() == y_positions.size());
    m_points.reserve(x_positions.size());
    for (size_t i = 0; i < x_positions.size(); i++) {
      m_points.emplace_back(x_positions[i], y_positions[i]);
    }
  }

  NODISCARD const std::vector<Point> &points() const { return m_points; }

  NODISCARD auto x() const -> std::vector<double> {
    std::vector<double> x_vals;
    x_vals.reserve(m_points.size());
    for (const auto &point : m_points) {
      x_vals.push_back(point.x());
    }
    return x_vals;
  }

  NODISCARD auto y() const -> std::vector<double> {
    std::vector<double> y;
    y.reserve(m_points.size());
    for (const auto &point : m_points) {
      y.push_back(point.y());
    }
    return y;
  }

  void addPoint(const Gahm::Datatypes::Point &point) {
    m_points.push_back(point);
  }

  void addPoint(double x_pos, double y_pos) {
    m_points.emplace_back(x_pos, y_pos);
  }

  void clear() { m_points.clear(); }

  void removePoint(const Gahm::Datatypes::Point &point) {
    auto iter = std::find(m_points.begin(), m_points.end(), point);
    if (iter != m_points.end()) {
      m_points.erase(iter);
    }
  }

  void reserve(size_t size) { m_points.reserve(size); }

#ifndef SWIG
  auto begin() { return m_points.begin(); }
  NODISCARD auto begin() const { return m_points.begin(); }

  auto end() { return m_points.end(); }
  NODISCARD auto end() const { return m_points.end(); }

  auto front() { return m_points.front(); }
  NODISCARD auto front() const { return m_points.front(); }

  auto back() { return m_points.back(); }
  NODISCARD auto back() const { return m_points.back(); }

  NODISCARD auto operator==(const Gahm::Datatypes::PointCloud &rhs) const
      -> bool {
    return m_points == rhs.m_points;
  }

  NODISCARD auto operator!=(const Gahm::Datatypes::PointCloud &rhs) const
      -> bool {
    return !(rhs == *this);
  }

  NODISCARD auto operator[](size_t index) -> Gahm::Datatypes::Point & {
    return m_points[index];
  }
  NODISCARD auto operator[](size_t index) const
      -> const Gahm::Datatypes::Point & {
    return m_points[index];
  }

#endif

  NODISCARD auto size() const -> size_t { return m_points.size(); }

  NODISCARD auto empty() const -> bool { return m_points.empty(); }

 private:
  std::vector<Gahm::Datatypes::Point> m_points;
};

}  // namespace Gahm::Datatypes
#endif  // GAHM_POINTCLOUD_H
