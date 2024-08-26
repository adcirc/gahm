//
// Created by Zach Cobell on 8/22/24.
//

#ifndef GAHM_GRID_HPP
#define GAHM_GRID_HPP

#include <cstddef>

#include "datatypes/Point.hpp"
#include "datatypes/PointCloud.hpp"

namespace Gahm::Types {

class Grid {
 public:
  constexpr Grid(const Gahm::Types::Point &lower_left, double dx, double dy, size_t nx,
                 size_t ny)
      : m_ll(lower_left), m_dx(dx), m_dy(dy), m_nx(nx), m_ny(ny) {}

  static constexpr auto fromCorners(double x1, double y1, double x2, double y2,
                                    double dx, double dy) -> Gahm::Types::Grid {
    const auto grid_xll = std::min(x1, x2);
    const auto grid_yll = std::min(y1, y2);
    const auto grid_xur = std::max(x1, x2);
    const auto grid_yur = std::max(y1, y2);
    return {{grid_xll, grid_yll},
            dx,
            dy,
            static_cast<size_t>((grid_xur - grid_xll) / dx),
            static_cast<size_t>((grid_yur - grid_yll) / dy)};
  }

  static constexpr auto fromCorners(const Gahm::Types::Point &pt1, const Gahm::Types::Point &pt2,
                                    double dx, double dy) -> Gahm::Types::Grid {
    return fromCorners(pt1.x(), pt1.y(), pt2.x(), pt2.y(), dx, dy);
  }

  [[nodiscard]] constexpr auto xll() const -> double { return m_ll.x(); }
  [[nodiscard]] constexpr auto yll() const -> double { return m_ll.y(); }
  [[nodiscard]] constexpr auto dx() const -> double { return m_dx; }
  [[nodiscard]] constexpr auto dy() const -> double { return m_dy; }
  [[nodiscard]] constexpr auto nx() const -> size_t { return m_nx; }
  [[nodiscard]] constexpr auto ny() const -> size_t { return m_ny; }
  [[nodiscard]] constexpr auto size() const -> size_t { return m_nx * m_ny; }

  [[nodiscard]] auto x(size_t index) const -> double {
    return m_ll.x() + (static_cast<double>(index) * m_dx);
  }
  [[nodiscard]] auto y(size_t index) const -> double {
    return m_ll.y() + (static_cast<double>(index) * m_dy);
  }

  [[nodiscard]] auto x_vector() const -> std::vector<double> {
    std::vector<double> x_vec;
    x_vec.reserve(m_nx);
    for (size_t i = 0; i < m_nx; i++) {
      x_vec.push_back(m_ll.x() + (static_cast<double>(i) * m_dx));
    }
    return x_vec;
  }

  [[nodiscard]] auto y_vector() const -> std::vector<double> {
    std::vector<double> y_vec;
    y_vec.reserve(m_ny);
    for (size_t j = 0; j < m_ny; j++) {
      y_vec.push_back(m_ll.y() + (static_cast<double>(j) * m_dy));
    }
    return y_vec;
  }

  [[nodiscard]] auto points() const -> Gahm::Types::PointCloud {
    const auto x_vec = this->x_vector();
    const auto y_vec = this->y_vector();
    PointCloud points;
    points.reserve(x_vec.size() * y_vec.size());
    for (const auto &x_val : x_vec) {
      for (const auto &y_val : y_vec) {
        points.addPoint(x_val, y_val);
      }
    }
    return points;
  }

  [[nodiscard]] auto x_grid() const -> std::vector<std::vector<double>> {
    std::vector<std::vector<double>> x_g;
    x_g.reserve(m_nx);
    for (size_t i = 0; i < m_nx; i++) {
      x_g.emplace_back();
      x_g[i].reserve(m_ny);
      for (size_t j = 0; j < m_ny; j++) {
        x_g[i].push_back(m_ll.x() + (static_cast<double>(i) * m_dx));
      }
    }
    return x_g;
  }

  [[nodiscard]] auto y_grid() const -> std::vector<std::vector<double>> {
    std::vector<std::vector<double>> y_g;
    y_g.reserve(m_nx);
    for (size_t i = 0; i < m_nx; i++) {
      y_g.emplace_back();
      y_g[i].reserve(m_ny);
      for (size_t j = 0; j < m_ny; j++) {
        y_g[i].push_back(m_ll.y() + (static_cast<double>(j) * m_dy));
      }
    }
    return y_g;
  }

 private:
  Gahm::Types::Point m_ll;
  double m_dx;
  double m_dy;
  size_t m_nx;
  size_t m_ny;
};

}  // namespace Gahm::Types

#endif  // GAHM_GRID_HPP
