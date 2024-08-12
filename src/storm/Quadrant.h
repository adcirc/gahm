//
// Created by Zach Cobell on 8/4/24.
//

#ifndef GAHM_QUADRANT_H
#define GAHM_QUADRANT_H

#include <array>
#include <cstdint>

#include "datatypes/RotationMatrix.h"
#include "datatypes/Vec.h"
#include "physical/Constants.h"
#include "storm/Isotach.h"

namespace Gahm::Storm {

class Quadrant {
 public:
  enum QuadrantCode : uint8_t { NE = 0, SE = 1, SW = 2, NW = 3 };

  Quadrant() : m_isotachs(), m_quadrant_code(NE), m_unit_vector_tbl() {}

  [[nodiscard]] static constexpr auto integer_to_quadrant_code(uint8_t code)
      -> QuadrantCode {
    return static_cast<QuadrantCode>(code);
  }

  explicit Quadrant(QuadrantCode code, double latitude,
                    const std::array<Isotach, 3>& isotachs);

  [[nodiscard]] auto quadrant_code() const -> QuadrantCode {
    return m_quadrant_code;
  }

  [[nodiscard]] auto isotachs() const -> const std::array<Isotach, 3>& {
    return m_isotachs;
  }

  auto isotachs() -> std::array<Isotach, 3>& { return m_isotachs; }

  [[nodiscard]] auto isotach(size_t index) const -> const Isotach& {
    return m_isotachs[index];
  }

  void set_isotachs(const std::array<Isotach, 3>& isotachs) {
    m_isotachs = isotachs;
  }

  void set_unit_vector(const Types::Vec& vec) { m_unit_vector_tbl = vec; }

  [[nodiscard]] auto unit_vector_tbl() const -> const Types::Vec& {
    return m_unit_vector_tbl;
  }

 private:
  std::array<Isotach, 3> m_isotachs;
  QuadrantCode m_quadrant_code;
  Types::Vec m_unit_vector_tbl;
};

}  // namespace Gahm::Storm

auto operator<<(std::ostream& stream,
                const Gahm::Storm::Quadrant& quadrant) -> std::ostream&;

#endif  // GAHM_QUADRANT_H
