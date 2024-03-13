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
#ifndef GAHM_POINT_H
#define GAHM_POINT_H

#ifdef SWIG
#define NODISCARD
#else
#define NODISCARD [[nodiscard]]
#endif

namespace Gahm::Datatypes {
class Point {
 public:
  Point() : m_x(0.0), m_y(0.0) {}

  Point(double x_position, double y_position)
      : m_x(x_position), m_y(y_position) {}

  NODISCARD auto x() const -> double { return m_x; }
  NODISCARD auto y() const -> double { return m_y; }

  void setX(double x_position) { m_x = x_position; }
  void setY(double y_position) { m_y = y_position; }

  auto operator==(const Point &rhs) const -> bool {
    return m_x == rhs.m_x && m_y == rhs.m_y;
  }

  auto operator!=(const Point &rhs) const -> bool { return !(rhs == *this); }

 private:
  double m_x;
  double m_y;
};
}  // namespace Gahm::Datatypes
#endif  // GAHM_POINT_H
