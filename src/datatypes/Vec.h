//
// Created by Zach Cobell on 8/2/24.
//

#ifndef GAHM_VEC_H
#define GAHM_VEC_H

#include <array>
#include <cmath>
#include <ostream>
#include <iostream>

namespace Gahm::Types {

class Vec {
 public:
  constexpr Vec() : m_u(0), m_v(0) {}
  constexpr Vec(double u, double v) : m_u(u), m_v(v) {}
  constexpr explicit Vec(std::array<double, 2> arr)
      : m_u(arr[0]), m_v(arr[1]) {}

  [[nodiscard]] constexpr auto u() const -> double { return m_u; }
  [[nodiscard]] constexpr auto v() const -> double { return m_v; }

  constexpr void set_u(double x_location) { m_u = x_location; }
  constexpr void set_v(double y_location) { m_v = y_location; }

  constexpr void set(std::array<double, 2> arr) {
    m_u = arr[0];
    m_v = arr[1];
  }

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

  [[nodiscard]] constexpr auto operator+(const Vec &rhs) const -> Vec {
    return {m_u + rhs.m_u, m_v + rhs.m_v};
  }

  [[nodiscard]] constexpr auto operator-(const Vec &rhs) const -> Vec {
    return {m_u - rhs.m_u, m_v - rhs.m_v};
  }

  [[nodiscard]] constexpr auto operator+(const double &val) const -> Vec {
    return {m_u + val, m_v + val};
  }

  [[nodiscard]] constexpr auto operator-(const double &val) const -> Vec {
    return {m_u - val, m_v - val};
  }

  [[nodiscard]] constexpr auto operator*(const double &val) const -> Vec {
    return {m_u * val, m_v * val};
  }

  [[nodiscard]] constexpr auto operator/(const double &val) const -> Vec {
    return {m_u / val, m_v / val};
  }

  [[nodiscard]] auto magnitude() const -> double {
    return std::hypot(m_u, m_v);
  }

  [[nodiscard]] auto direction() const -> double {
    return std::atan2(m_v, m_u);
  }

  [[nodiscard]] auto normalize() const -> Vec {
    const auto mag = this->magnitude();
    return {m_u / mag, m_v / mag};
  }

  [[nodiscard]] constexpr auto dot(const Vec &rhs) const -> double {
    return m_u * rhs.m_u + m_v * rhs.m_v;
  }

  [[nodiscard]] constexpr auto cross(const Vec &rhs) const -> double {
    return m_u * rhs.m_v - m_v * rhs.m_u;
  }

  [[nodiscard]] constexpr auto array() const -> std::array<double, 2> {
    return {m_u, m_v};
  }

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

auto operator<<(std::ostream &stream,
                const Gahm::Types::Vec &vec) -> std::ostream &;

#endif  // GAHM_VEC_H
