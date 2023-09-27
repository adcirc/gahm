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

  Point(double x, double y) : m_x(x), m_y(y) {}

  NODISCARD double x() const { return m_x; }
  NODISCARD double y() const { return m_y; }

  void setX(double x) { m_x = x; }
  void setY(double y) { m_y = y; }

  bool operator==(const Point &rhs) const {
    return m_x == rhs.m_x && m_y == rhs.m_y;
  }

  bool operator!=(const Point &rhs) const { return !(rhs == *this); }

 private:
  double m_x;
  double m_y;
};
}  // namespace Gahm::Datatypes
#endif  // GAHM_POINT_H
