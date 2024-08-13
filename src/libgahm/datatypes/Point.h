//
// Created by Zach Cobell on 7/30/24.
//

#ifndef GAHM_POINT_H
#define GAHM_POINT_H

#include <ostream>

namespace Gahm::Types {

/**
 * @class Point
 * @brief A class representing a point in 2D space.
 */
class Point {
 public:
  /**
   * Default constructor
   */
  constexpr Point() : m_x(0), m_y(0) {}

  /**
   * Constructor
   * @param x x-coordinate
   * @param y y-coordinate
   */
  constexpr Point(double x, double y) : m_x(x), m_y(y) {}

  /**
   * @brief Returns the x-coordinate of the point.
   * @return x-coordinate
   */
  [[nodiscard]] constexpr auto x() const -> double { return m_x; }

  /**
   * @brief Returns the y-coordinate of the point.
   * @return y-coordinate
   */
  [[nodiscard]] constexpr auto y() const -> double { return m_y; }

  /**
   * @brief Sets the x-coordinate of the point.
   * @param x_location x-coordinate
   */
  void setX(double x_location) { m_x = x_location; }

  /**
   * @brief Sets the y-coordinate of the point.
   * @param y_location y-coordinate
   */
  void setY(double y_location) { m_y = y_location; }

 private:
  double m_x;
  double m_y;
};
}  // namespace Gahm::Types

auto operator<<(std::ostream &stream,
                const Gahm::Types::Point &point) -> std::ostream &;

#endif  // GAHM_POINT_H
