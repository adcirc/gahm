//
// Created by Zach Cobell on 3/15/23.
//

#ifndef GAHM_POINTCLOUD_H
#define GAHM_POINTCLOUD_H

#include <algorithm>
#include <cassert>
#include <vector>

#include "datatypes/Point.h"

#ifndef SWIG
#define NODISCARD [[nodiscard]]
#else
#define NODISCARD
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

  NODISCARD const std::vector<Point> &points() const { return m_points; }

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
#endif

  NODISCARD size_t size() const { return m_points.size(); }

  NODISCARD bool empty() const { return m_points.empty(); }

  NODISCARD Gahm::Datatypes::Point &operator[](size_t index) {
    return m_points[index];
  }
  NODISCARD const Gahm::Datatypes::Point &operator[](size_t index) const {
    return m_points[index];
  }

  NODISCARD bool operator==(const Gahm::Datatypes::PointCloud &rhs) const {
    return m_points == rhs.m_points;
  }

  NODISCARD bool operator!=(const Gahm::Datatypes::PointCloud &rhs) const {
    return !(rhs == *this);
  }

 private:
  std::vector<Gahm::Datatypes::Point> m_points;
};

}  // namespace Gahm::Datatypes
#endif  // GAHM_POINTCLOUD_H
