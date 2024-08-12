//
// Created by Zach Cobell on 8/1/24.
//

#ifndef GAHM_ROTATIONMATRIX_H
#define GAHM_ROTATIONMATRIX_H

#include <array>
#include <cmath>
#include <ostream>

namespace Gahm::Types {

class RotationMatrix {
 public:
  RotationMatrix(double angle, double latitude) : m_matrix({0, 0, 0, 0}) {
    const auto sgn = copysign(1.0, latitude);
    const auto cos_angle = std::cos(angle);
    const auto sin_angle = std::sin(angle);
    m_matrix = {{
        {cos_angle, -sgn * sin_angle},
        {sgn * sin_angle, cos_angle},
    }};
  }

  [[nodiscard]] constexpr auto at(int i, int j) const -> double {
    return m_matrix[i][j];
  }

  [[nodiscard]] auto operator()() -> std::array<std::array<double, 2>, 2> {
    return m_matrix;
  }

  // Iterators
  [[nodiscard]] auto begin() -> std::array<std::array<double, 2>, 2>::iterator {
    return m_matrix.begin();
  }

  [[nodiscard]] auto end() -> std::array<std::array<double, 2>, 2>::iterator {
    return m_matrix.end();
  }

  [[nodiscard]] auto cbegin() const
      -> std::array<std::array<double, 2>, 2>::const_iterator {
    return m_matrix.cbegin();
  }

  [[nodiscard]] auto cend() const
      -> std::array<std::array<double, 2>, 2>::const_iterator {
    return m_matrix.cend();
  }

  [[nodiscard]] auto rbegin()
      -> std::array<std::array<double, 2>, 2>::reverse_iterator {
    return m_matrix.rbegin();
  }

  [[nodiscard]] auto rend()
      -> std::array<std::array<double, 2>, 2>::reverse_iterator {
    return m_matrix.rend();
  }

  [[nodiscard]] auto crbegin() const
      -> std::array<std::array<double, 2>, 2>::const_reverse_iterator {
    return m_matrix.crbegin();
  }

  [[nodiscard]] auto crend() const
      -> std::array<std::array<double, 2>, 2>::const_reverse_iterator {
    return m_matrix.crend();
  }

  [[nodiscard]] auto size() const -> std::size_t { return m_matrix.size(); }

  [[nodiscard]] auto empty() const -> bool { return m_matrix.empty(); }

  [[nodiscard]] auto data() -> std::array<std::array<double, 2>, 2>::pointer {
    return m_matrix.data();
  }

  [[nodiscard]] auto data() const
      -> const std::array<std::array<double, 2>, 2> & {
    return m_matrix;
  }

 private:
  std::array<std::array<double, 2>, 2> m_matrix;
};

}  // namespace Gahm::Types

auto operator<<(std::ostream &stream,
                const Gahm::Types::RotationMatrix &matrix) -> std::ostream &;

#endif  // GAHM_ROTATIONMATRIX_H
