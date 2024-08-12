//
// Created by Zach Cobell on 7/30/24.
//

#ifndef GAHM_POINT_H
#define GAHM_POINT_H

#include <ostream>

namespace Gahm::Types {
class Point {
 public:
  constexpr Point() : m_x(0), m_y(0) {}
  constexpr Point(double x, double y) : m_x(x), m_y(y) {}

  [[nodiscard]] constexpr auto x() const -> double { return m_x; }
  [[nodiscard]] constexpr auto y() const -> double { return m_y; }

  void setX(double x_location) { m_x = x_location; }
  void setY(double y_location) { m_y = y_location; }

 private:
  double m_x;
  double m_y;
};
}  // namespace Gahm::Types

auto operator<<(std::ostream &stream,
                const Gahm::Types::Point &point) -> std::ostream &;

#endif  // GAHM_POINT_H
