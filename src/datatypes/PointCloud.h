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

  PointCloud(const std::vector<double> &x, const std::vector<double> &y) {
    assert(x.size() == y.size());
    m_points.reserve(x.size());
    for (size_t i = 0; i < x.size(); i++) {
      m_points.emplace_back(x[i], y[i]);
    }
  }

  PointCloud(const size_t size, const double x[], const double y[]) {
    m_points.reserve(size);
    for (size_t i = 0; i < size; i++) {
      m_points.emplace_back(x[i], y[i]);
    }
  }

  NODISCARD const std::vector<Point> &points() const { return m_points; }

  NODISCARD std::vector<double> x() const {
    std::vector<double> x;
    x.reserve(m_points.size());
    for (const auto &point : m_points) {
      x.push_back(point.x());
    }
    return x;
  }

  NODISCARD std::vector<double> y() const {
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
  void addPoint(double x, double y) { m_points.emplace_back(x, y); }

  void clear() { m_points.clear(); }

  void removePoint(const Gahm::Datatypes::Point &point) {
    auto it = std::find(m_points.begin(), m_points.end(), point);
    if (it != m_points.end()) {
      m_points.erase(it);
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

  NODISCARD bool operator==(const Gahm::Datatypes::PointCloud &rhs) const {
    return m_points == rhs.m_points;
  }

  NODISCARD bool operator!=(const Gahm::Datatypes::PointCloud &rhs) const {
    return !(rhs == *this);
  }

  NODISCARD Gahm::Datatypes::Point &operator[](size_t index) {
    return m_points[index];
  }
  NODISCARD const Gahm::Datatypes::Point &operator[](size_t index) const {
    return m_points[index];
  }

#endif

  NODISCARD size_t size() const { return m_points.size(); }

  NODISCARD bool empty() const { return m_points.empty(); }

 private:
  std::vector<Gahm::Datatypes::Point> m_points;
};

}  // namespace Gahm::Datatypes
#endif  // GAHM_POINTCLOUD_H
