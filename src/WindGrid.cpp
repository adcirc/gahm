// MIT License
//
// Copyright (c) 2020 ADCIRC Development Group
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//
// Author: Zach Cobell
// Contact: zcobell@thewaterinstitute.org
//
#include "WindGrid.h"

#include <array>
#include <cassert>
#include <cmath>
#include <fstream>
#include <iostream>
#include <memory>

#include "Geometry.h"
#include "boost/geometry.hpp"

namespace bg = boost::geometry;
typedef bg::model::point<double, 2, bg::cs::cartesian> point_t;
typedef bg::model::polygon<point_t> polygon_t;

WindGrid::WindGrid(double llx, double lly, double urx, double ury, double dx,
                   double dy)
    : m_di(dx),
      m_dj(dy),
      m_rotation(0.0),
      m_dxx(dx),
      m_dxy(0.0),
      m_dyy(dy),
      m_dyx(0.0),
      m_ni(static_cast<size_t>(std::floor((urx - llx) / m_dxx) + 1)),
      m_nj(static_cast<size_t>(std::floor((ury - lly) / m_dyy) + 1)),
      m_width(urx - llx),
      m_height(ury - lly),
      m_center((llx + m_width / 2.0), (lly + m_height / 2.0)),
      m_corners(generateCorners(m_center.x(), m_center.y(), m_width, m_height)),
      m_geometry(std::make_unique<Geometry>(m_corners)) {
  assert(urx > llx);
  assert(lly < ury);
  assert(m_dxx > 0.0);
  assert(m_dyy > 0.0);
  assert(m_ni > 0);
  assert(m_nj > 0);
  assert(m_width > 0);
  assert(m_height > 0);
  this->generateGrid();
}

WindGrid::WindGrid(double xinit, double yinit, size_t ni, size_t nj, double di,
                   double dj, double rotation)
    : m_di(di),
      m_dj(dj),
      m_rotation(rotation * M_PI / 180.0),
      m_dxx(di * std::cos(m_rotation)),
      m_dxy(di * std::sin(m_rotation)),
      m_dyx(dj * std::sin(m_rotation)),
      m_dyy(dj * std::cos(m_rotation)),
      m_ni(ni),
      m_nj(nj),
      m_width(static_cast<double>(m_ni - 1) * m_dxx),
      m_height(static_cast<double>(m_nj - 1) * m_dyy),
      m_center((xinit + m_width / 2.0), (yinit + m_height / 2.0)),
      m_corners(generateCorners(m_center.x(), m_center.y(), m_width, m_height)),
      m_geometry(std::make_unique<Geometry>(m_corners)) {
  assert(m_di > 0);
  assert(m_dj > 0);
  assert(m_rotation >= -M_PI || m_rotation <= M_PI);
  assert(m_ni > 0);
  assert(m_nj > 0);
  this->generateGrid();
}

WindGrid::~WindGrid() = default;

WindGrid::WindGrid(const WindGrid &w)
    : m_di(w.di()),
      m_dj(w.dj()),
      m_rotation(w.rotation() * M_PI / 180.0),
      m_dxx(m_di * std::cos(m_rotation)),
      m_dxy(m_di * std::sin(m_rotation)),
      m_dyx(m_dj * std::sin(m_rotation)),
      m_dyy(m_dj * std::cos(m_rotation)),
      m_ni(w.ni()),
      m_nj(w.nj()),
      m_width(static_cast<double>(m_ni - 1) * m_dxx),
      m_height(static_cast<double>(m_nj - 1) * m_dyy),
      m_center((w.bottom_left().x() + m_width / 2.0),
               (w.bottom_left().y() + m_height / 2.0)),
      m_corners(generateCorners(m_center.x(), m_center.y(), m_width, m_height)),
      m_geometry(std::make_unique<Geometry>(m_corners)) {}

void WindGrid::generateGrid() {
  m_grid.resize(ni());
  for (size_t i = 0; i < ni(); ++i) {
    m_grid[i].resize(nj());
    for (size_t j = 0; j < nj(); ++j) {
      m_grid[i][j] = {bottom_left().x() + i * m_dxx - j * m_dyx,
                      bottom_left().y() + j * m_dyy + i * m_dyx};
    }
  }
}

Point WindGrid::center(const size_t i, const size_t j) const {
  assert(i < ni() - 1 && j < nj() - 1);
  if (i > ni() - 1 || j > nj() + 1) return {0, 0};
  return {(m_grid[i][j].x() + m_grid[i + i][j + 1].x()) / 2.0,
          (m_grid[i][j].y() + m_grid[i + 1][j + 1].y()) / 2.0};
}

Point WindGrid::corner(const size_t i, const size_t j) const {
  assert(i < ni() && j < nj());
  return i < ni() && j < nj() ? Point(m_grid[i][j].x(), m_grid[i][j].y())
                              : Point(0.0, 0.0);
}

size_t WindGrid::ni() const { return m_ni; }
size_t WindGrid::nj() const { return m_nj; }
double WindGrid::rotation() const { return m_rotation * 180.0 / M_PI; }
double WindGrid::di() const { return m_di; }
double WindGrid::dj() const { return m_dj; }
double WindGrid::dxx() const { return m_dxx; }
double WindGrid::dxy() const { return m_dxy; }
double WindGrid::dyx() const { return m_dyx; }
double WindGrid::dyy() const { return m_dyy; }
double WindGrid::dx() const { return m_dxx; }
double WindGrid::dy() const { return m_dyy; }
Point WindGrid::bottom_left() const { return m_corners[0]; }
Point WindGrid::bottom_right() const { return m_corners[1]; }
Point WindGrid::top_left() const { return m_corners[3]; }
Point WindGrid::top_right() const { return m_corners[2]; }

void WindGrid::write(const std::string &filename) const {
  std::ofstream fout;
  fout.open(filename);
  size_t count = 0;
  size_t count_i = 0;
  for (auto &i : m_grid) {
    size_t count_j = 0;
    for (auto &j : i) {
      fout << j.x() << " " << j.y() << " " << count << " " << count_i << " "
           << count_j << "\n";
      count++;
      count_j++;
    }
    count_i++;
  }
  fout.close();
}

Point WindGrid::corner(const size_t index) const {
  size_t j = index % nj();
  size_t i = index / nj();
  return this->corner(i, j);
}

Point WindGrid::center(const size_t index) const {
  size_t j = index % nj();
  size_t i = index / nj();
  return this->center(i, j);
}

bool WindGrid::point_inside(const Point &p) const {
  return this->m_geometry->is_inside(p);
}

std::array<Point, 4> WindGrid::generateCorners(const double cx, const double cy,
                                               const double w, const double h,
                                               const double rotation) {
  Point top_right(
      cx + ((w / 2.0) * std::cos(rotation)) - ((h / 2.0) * std::sin(rotation)),
      cy + ((w / 2.0) * std::sin(rotation)) + ((h / 2.0) * std::cos(rotation)));

  Point top_left(
      cx - ((w / 2.0) * std::cos(rotation)) - ((h / 2.0) * std::sin(rotation)),
      cy - ((w / 2.0) * std::sin(rotation)) + ((h / 2.0) * std::cos(rotation)));

  Point bottom_left(
      cx - ((w / 2.0) * std::cos(rotation)) + ((h / 2.0) * std::sin(rotation)),
      cy - ((w / 2.0) * std::sin(rotation)) - ((h / 2.0) * std::cos(rotation)));

  Point bottom_right(
      cx + ((w / 2.0) * std::cos(rotation)) + ((h / 2.0) * std::sin(rotation)),
      cy + ((w / 2.0) * std::sin(rotation)) - ((h / 2.0) * std::cos(rotation)));

  return {bottom_left, bottom_right, top_right, top_left};
}

std::tuple<std::vector<double>, std::vector<double>> WindGrid::griddata()
    const {
  std::vector<double> x;
  std::vector<double> y;
  x.reserve(m_grid.size() * m_grid[0].size());
  y.reserve(x.capacity());

  for (auto j = 0; j < nj(); ++j) {
    for (auto i = 0; i < ni(); ++i) {
      auto p = corner(i, j);
      x.push_back(p.x());
      y.push_back(p.y());
    }
  }

  return std::make_tuple(x, y);
}

std::vector<double> WindGrid::xpoints() const {
  std::vector<double> x;
  x.reserve(m_grid.size() * m_grid[0].size());

  for (auto j = 0; j < nj(); ++j) {
    for (auto i = 0; i < ni(); ++i) {
      auto p = corner(i, j);
      x.push_back(p.x());
    }
  }
  return x;
}

std::vector<double> WindGrid::ypoints() const {
  std::vector<double> y;
  y.reserve(m_grid.size() * m_grid[0].size());

  for (auto j = 0; j < nj(); ++j) {
    for (auto i = 0; i < ni(); ++i) {
      auto p = corner(i, j);
      y.push_back(p.y());
    }
  }
  return y;
}
