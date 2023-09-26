//
// Created by Zach Cobell on 3/15/23.
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
