//
// Created by Zach Cobell on 8/1/24.
//

#ifndef GAHM_ROTATIONMATRIX_H
#define GAHM_ROTATIONMATRIX_H

#include <array>
#include <cassert>
#include <cmath>
#include <cstddef>
#include <ostream>

namespace Gahm::Types {

/**
 * @class RotationMatrix
 * @brief Represents a 2x2 rotation matrix.
 */
class RotationMatrix {
 public:
  using RotMat = std::array<std::array<double, 2>, 2>;

  /**
   * Constructor
   * @param angle Angle of rotation in radians
   * @param latitude Latitude of the storm center
   */
  RotationMatrix(double angle, double latitude)
      : m_matrix(RotMat{{{0, 0}, {0, 0}}}) {
    const auto sgn = copysign(1.0, latitude);
    const auto cos_angle = std::cos(angle);
    const auto sin_angle = std::sin(angle);
    m_matrix = {{
        {cos_angle, -sgn * sin_angle},
        {sgn * sin_angle, cos_angle},
    }};
  }

  /**
   * @brief Returns the value at the given row and column
   * @param i i-th row
   * @param j j-th column
   * @return Value at the given row and column
   */
  [[nodiscard]] constexpr auto at(unsigned i, unsigned j) const -> double {
    assert(i < 2 && j < 2);
    return m_matrix.at(i).at(j);
  }

  /**
   * @brief Returns the rotation matrix as a 2D array
   * @return The rotation matrix as a 2D array
   */
  [[nodiscard]] auto operator()() const -> const RotMat & { return m_matrix; }

  [[nodiscard]] auto begin() -> RotMat::iterator { return m_matrix.begin(); }

  [[nodiscard]] auto end() -> RotMat::iterator { return m_matrix.end(); }

  [[nodiscard]] auto cbegin() const -> RotMat::const_iterator {
    return m_matrix.cbegin();
  }

  [[nodiscard]] auto cend() const -> RotMat::const_iterator {
    return m_matrix.cend();
  }

  [[nodiscard]] auto rbegin() -> RotMat::reverse_iterator {
    return m_matrix.rbegin();
  }

  [[nodiscard]] auto rend() -> RotMat::reverse_iterator {
    return m_matrix.rend();
  }

  [[nodiscard]] auto crbegin() const -> RotMat::const_reverse_iterator {
    return m_matrix.crbegin();
  }

  [[nodiscard]] auto crend() const -> RotMat::const_reverse_iterator {
    return m_matrix.crend();
  }

  [[nodiscard]] auto size() const -> size_t { return m_matrix.size(); }

  [[nodiscard]] auto empty() const -> bool { return m_matrix.empty(); }

  [[nodiscard]] auto data() const -> const RotMat & { return m_matrix; }

 private:
  RotMat m_matrix;
};

}  // namespace Gahm::Types

auto operator<<(std::ostream &stream,
                const Gahm::Types::RotationMatrix &matrix) -> std::ostream &;

#endif  // GAHM_ROTATIONMATRIX_H
