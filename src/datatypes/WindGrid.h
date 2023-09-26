//
// Created by Zach Cobell on 3/15/23.
//

#ifndef GAHM_WINDGRID_H
#define GAHM_WINDGRID_H

#include <cstdlib>
#include <vector>

#include "datatypes/Point.h"
#include "datatypes/PointCloud.h"

namespace Gahm::Datatypes {
class WindGrid {
 public:
  WindGrid(double xll, double yll, double dx, double dy, size_t nx, size_t ny)
      : m_xll(xll), m_yll(yll), m_dx(dx), m_dy(dy), m_nx(nx), m_ny(ny) {}

  static WindGrid fromCorners(double xll, double yll, double xur, double yur,
                              double dx, double dy) {
    return {xll,
            yll,
            dx,
            dy,
            static_cast<size_t>((xur - xll) / dx),
            static_cast<size_t>((yur - yll) / dy)};
  }

  [[nodiscard]] double xll() const { return m_xll; }
  [[nodiscard]] double yll() const { return m_yll; }
  [[nodiscard]] double dx() const { return m_dx; }
  [[nodiscard]] double dy() const { return m_dy; }
  [[nodiscard]] size_t nx() const { return m_nx; }
  [[nodiscard]] size_t ny() const { return m_ny; }

  void setXll(double xll) { m_xll = xll; }
  void setYll(double yll) { m_yll = yll; }
  void setDx(double dx) { m_dx = dx; }
  void setDy(double dy) { m_dy = dy; }
  void setNx(size_t nx) { m_nx = nx; }
  void setNy(size_t ny) { m_ny = ny; }

  [[nodiscard]] double x(size_t i) const {
    return m_xll + static_cast<double>(i) * m_dx;
  }
  [[nodiscard]] double y(size_t j) const {
    return m_yll + static_cast<double>(j) * m_dy;
  }

  [[nodiscard]] std::vector<double> x_vector() const {
    std::vector<double> x(m_nx);
    for (size_t i = 0; i < m_nx; i++) {
      x[i] = m_xll + static_cast<double>(i) * m_dx;
    }
    return x;
  }

  [[nodiscard]] std::vector<double> y_vector() const {
    std::vector<double> y(m_ny);
    for (size_t j = 0; j < m_ny; j++) {
      y[j] = m_yll + static_cast<double>(j) * m_dy;
    }
    return y;
  }

  [[nodiscard]] Gahm::Datatypes::PointCloud points() const {
    auto xv = x_vector();
    auto yv = y_vector();
    Gahm::Datatypes::PointCloud points;
    points.reserve(xv.size() * yv.size());
    for (const auto &x : xv) {
      for (const auto &y : yv) {
        points.addPoint(x, y);
      }
    }
    return points;
  }

  [[nodiscard]] std::vector<std::vector<double>> x_grid() const {
    std::vector<std::vector<double>> x_g;
    x_g.reserve(m_nx);
    for (size_t i = 0; i < m_nx; i++) {
      x_g.emplace_back(m_ny);
      for (size_t j = 0; j < m_ny; j++) {
        x_g[i][j] = m_xll + static_cast<double>(i) * m_dx;
      }
    }
    return x_g;
  }

  [[nodiscard]] std::vector<std::vector<double>> y_grid() const {
    std::vector<std::vector<double>> y_g;
    y_g.reserve(m_nx);
    for (size_t i = 0; i < m_nx; i++) {
      y_g.emplace_back(m_ny);
      for (size_t j = 0; j < m_ny; j++) {
        y_g[i][j] = m_yll + static_cast<double>(j) * m_dy;
      }
    }
    return y_g;
  }

 private:
  double m_xll;
  double m_yll;
  double m_dx;
  double m_dy;
  size_t m_nx;
  size_t m_ny;
};
}  // namespace Gahm::Datatypes

#endif  // GAHM_WINDGRID_H
