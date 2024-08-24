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

  /**
   * @brief Equality operator
   * @param other Point to compare
   * @return true if the points are equal, false otherwise
   */
  [[nodiscard]] auto operator==(const Point &other) const -> bool {
    return m_x == other.m_x && m_y == other.m_y;
  }

  /**
   * @brief Inequality operator
   * @param other Point to compare
   * @return true if the points are not equal, false otherwise
   */
  [[nodiscard]] auto operator!=(const Point &other) const -> bool {
    return !(*this == other);
  }

 private:
  double m_x;
  double m_y;
};
}  // namespace Gahm::Types

#ifndef SWIG
auto operator<<(std::ostream &stream,
                const Gahm::Types::Point &point) -> std::ostream &;
#endif
#endif  // GAHM_POINT_H
