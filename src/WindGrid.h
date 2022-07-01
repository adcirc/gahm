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
#ifndef METGET_WINDGRID_H
#define METGET_WINDGRID_H

#include <array>
#include <memory>
#include <string>
#include <tuple>
#include <vector>

#include "Point.h"

namespace Gahm {
class Geometry;

class WindGrid {
 public:
  using grid = std::vector<std::vector<Point>>;

  WindGrid(double llx, double lly, double urx, double ury, double dx,
           double dy);
  WindGrid(double xinit, double yinit, size_t ni, size_t nj, double dx,
           double dy, double rotation = 0.0);

  ~WindGrid();

  WindGrid(const WindGrid &w);

  double dx() const;
  double dy() const;
  size_t ni() const;
  size_t nj() const;
  double rotation() const;
  double di() const;
  double dj() const;
  double dxx() const;
  double dxy() const;
  double dyx() const;
  double dyy() const;

  Point top_left() const;
  Point top_right() const;
  Point bottom_left() const;
  Point bottom_right() const;

  Point corner(size_t i, size_t j) const;
  Point center(size_t i, size_t j) const;

  Point corner(size_t index) const;
  Point center(size_t index) const;

  bool point_inside(const Point &p) const;

  void write(const std::string &filename) const;

  const grid &grid_positions() const { return m_grid; };

  std::tuple<std::vector<double>, std::vector<double>> griddata() const;

  std::vector<double> xpoints() const;
  std::vector<double> ypoints() const;

 private:
  const double m_di;
  const double m_dj;
  const double m_rotation;
  const double m_dxx;
  const double m_dxy;
  const double m_dyx;
  const double m_dyy;
  const size_t m_ni;
  const size_t m_nj;
  const double m_width;
  const double m_height;
  const Point m_center;
  const std::array<Point, 4> m_corners;

  grid m_grid;

  std::unique_ptr<Geometry> m_geometry;

  void generateGrid();
  static std::array<Point, 4> generateCorners(double cx, double cy, double w,
                                              double h, double rotation = 0.0);
};
}  // namespace Gahm
#endif  // METGET_WINDGRID_H
