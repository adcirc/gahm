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
#ifndef GAHM_WINDGRID_H
#define GAHM_WINDGRID_H

#include <cstdlib>
#include <vector>

#include "datatypes/Point.h"
#include "datatypes/PointCloud.h"

#ifdef SWIG
#define NODISCARD
#else
#define NODISCARD [[nodiscard]]
#endif

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

  NODISCARD double xll() const { return m_xll; }
  NODISCARD double yll() const { return m_yll; }
  NODISCARD double dx() const { return m_dx; }
  NODISCARD double dy() const { return m_dy; }
  NODISCARD size_t nx() const { return m_nx; }
  NODISCARD size_t ny() const { return m_ny; }

  void setXll(double xll) { m_xll = xll; }
  void setYll(double yll) { m_yll = yll; }
  void setDx(double dx) { m_dx = dx; }
  void setDy(double dy) { m_dy = dy; }
  void setNx(size_t nx) { m_nx = nx; }
  void setNy(size_t ny) { m_ny = ny; }

  NODISCARD double x(size_t i) const {
    return m_xll + static_cast<double>(i) * m_dx;
  }
  NODISCARD double y(size_t j) const {
    return m_yll + static_cast<double>(j) * m_dy;
  }

  NODISCARD std::vector<double> x_vector() const {
    std::vector<double> x(m_nx);
    for (size_t i = 0; i < m_nx; i++) {
      x[i] = m_xll + static_cast<double>(i) * m_dx;
    }
    return x;
  }

  NODISCARD std::vector<double> y_vector() const {
    std::vector<double> y(m_ny);
    for (size_t j = 0; j < m_ny; j++) {
      y[j] = m_yll + static_cast<double>(j) * m_dy;
    }
    return y;
  }

  NODISCARD Gahm::Datatypes::PointCloud points() const {
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

  NODISCARD std::vector<std::vector<double>> x_grid() const {
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

  NODISCARD std::vector<std::vector<double>> y_grid() const {
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
