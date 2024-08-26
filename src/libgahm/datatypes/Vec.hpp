//
// Created by Zach Cobell on 8/2/24.
//

#ifndef GAHM_VEC_H
#define GAHM_VEC_H

#include <array>
#include <cmath>
#include <iostream>
#include <ostream>

#include "physical/Constants.hpp"

namespace Gahm::Types {

/**
 * @class Vec
 * @brief A class representing a 2D vector with basic vector operations.
 */
class Vec {
 public:
  /**
   * @brief Default constructor for Vec.
   */
  constexpr Vec() : m_u(0), m_v(0) {}

  /**
   * @brief Constructor for Vec
   * @param u u-component of the vector
   * @param v v-component of the vector
   */
  constexpr Vec(double u, double v) : m_u(u), m_v(v) {}

  /**
   * @brief Constructor for Vec
   * @param arr Array containing the u and v components of the vector
   */
  constexpr explicit Vec(const std::array<double, 2> &arr)
      : m_u(arr[0]), m_v(arr[1]) {}

  /**
   * @brief Get the u-component of the vector
   * @return u-component of the vector
   */
  [[nodiscard]] constexpr auto u() const -> double { return m_u; }

  /**
   * @brief Get the v-component of the vector
   * @return v-component of the vector
   */
  [[nodiscard]] constexpr auto v() const -> double { return m_v; }

  /**
   * @brief Set the u-component of the vector
   * @param u_location u-component of the vector
   */
  constexpr void set_u(double u_location) { m_u = u_location; }

  /**
   * @brief Set the v-component of the vector
   * @param v_location v-component of the vector
   */
  constexpr void set_v(double v_location) { m_v = v_location; }

  /**
   * @brief Set the u and v components of the vector
   * @param arr Array containing the u and v components of the vector
   */
  constexpr void set(const std::array<double, 2> &arr) {
    m_u = arr[0];
    m_v = arr[1];
  }

  /**
   * @brief Set the u and v components of the vector
   * @param u u-component of the vector
   * @param v v-component of the vector
   */
  constexpr void set(double u, double v) {
    m_u = u;
    m_v = v;
  }

  [[nodiscard]] constexpr auto operator==(const Vec &rhs) const -> bool {
    return m_u == rhs.m_u && m_v == rhs.m_v;
  }

  [[nodiscard]] constexpr auto operator!=(const Vec &rhs) const -> bool {
    return !(*this == rhs);
  }

  /**
   * @brief Add two vectors together element-wise
   * @param rhs Vector to add to this vector
   * @return New vector with the sum of the two vectors
   */
  [[nodiscard]] constexpr auto operator+(const Vec &rhs) const -> Vec {
    return {m_u + rhs.m_u, m_v + rhs.m_v};
  }

  /**
   * @brief Subtract two vectors element-wise
   * @param rhs Vector to subtract from this vector
   * @return New vector with the difference of the two vectors
   */
  [[nodiscard]] constexpr auto operator-(const Vec &rhs) const -> Vec {
    return {m_u - rhs.m_u, m_v - rhs.m_v};
  }

  /**
   * @brief Add a scalar to each element of the vector
   * @param val Scalar to add to each element of the vector
   * @return New vector with the scalar added to each element
   */
  [[nodiscard]] constexpr auto operator+(const double &val) const -> Vec {
    return {m_u + val, m_v + val};
  }

  /**
   * @brief Subtract a scalar from each element of the vector
   * @param val Scalar to subtract from each element of the vector
   * @return New vector with the scalar subtracted from each element
   */
  [[nodiscard]] constexpr auto operator-(const double &val) const -> Vec {
    return {m_u - val, m_v - val};
  }

  /**
   * @brief Multiply each element of the vector by a scalar
   * @param val Scalar to multiply each element of the vector by
   * @return New vector with each element multiplied by the scalar
   */
  [[nodiscard]] constexpr auto operator*(const double &val) const -> Vec {
    return {m_u * val, m_v * val};
  }

  /**
   * @brief Divide each element of the vector by a scalar
   * @param val Scalar to divide each element of the vector by
   * @return New vector with each element divided by the scalar
   */
  [[nodiscard]] constexpr auto operator/(const double &val) const -> Vec {
    return {m_u / val, m_v / val};
  }

  /**
   * @brief Get the magnitude of the vector
   * @return Magnitude of the vector
   */
  [[nodiscard]] auto magnitude() const -> double {
    return std::hypot(m_u, m_v);
  }

  /**
   * @brief Get the direction of the vector normalized to [0, 2pi)
   * @return Direction of the vector
   */
  [[nodiscard]] auto direction() const -> double {
    auto dir = std::atan2(m_v, m_u);
    if (dir < 0) {
      dir += 2 * Physical::Constants::twoPi();
    }
    return dir;
  }

  /**
   * @brief Normalize the vector to a unit vector
   * @return Unit vector
   */
  [[nodiscard]] auto normalize() const -> Vec {
    const auto mag = this->magnitude();
    return {m_u / mag, m_v / mag};
  }

  /**
   * @brief Calculate the dot product of two vectors
   * @param rhs Vector to take the dot product with
   * @return Dot product of the two vectors
   */
  [[nodiscard]] constexpr auto dot(const Vec &rhs) const -> double {
    return (m_u * rhs.m_u) + (m_v * rhs.m_v);
  }

  /**
   * @brief Calculate the cross product of two vectors
   * @param rhs Vector to take the cross product with
   * @return Cross product of the two vectors
   */
  [[nodiscard]] constexpr auto cross(const Vec &rhs) const -> double {
    return (m_u * rhs.m_v) - (m_v * rhs.m_u);
  }

  /**
   * @brief Get the u and v components of the vector as an array
   * @return Array containing the u and v components of the vector
   */
  [[nodiscard]] constexpr auto array() const -> std::array<double, 2> {
    return {m_u, m_v};
  }

  /**
   * @brief Multiply a 2x2 matrix by a 2x1 vector
   * @param mat 2x2 matrix
   * @param vec 2x1 vector
   * @return 2x1 vector resulting from the matrix multiplication
   */
  [[nodiscard]] static constexpr auto matmul_22_21(
      const std::array<std::array<double, 2>, 2> &mat, const Vec &vec) -> Vec {
    return {mat[0][0] * vec.u() + mat[1][0] * vec.v(),
            mat[0][1] * vec.u() + mat[1][1] * vec.v()};
  }

 private:
  double m_u;
  double m_v;
};

}  // namespace Gahm::Types

#ifndef SWIG
auto operator<<(std::ostream &stream,
                const Gahm::Types::Vec &vec) -> std::ostream &;
#endif
#endif  // GAHM_VEC_H
