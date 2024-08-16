//
// Created by Zach Cobell on 8/4/24.
//

#ifndef GAHM_QUADRANT_H
#define GAHM_QUADRANT_H

#include <algorithm>
#include <array>
#include <cassert>
#include <cmath>
#include <cstddef>
#include <ostream>
#include <ranges>
#include <vector>

#include "datatypes/Point.h"
#include "datatypes/QuadCode.h"
#include "datatypes/QuadUnitVec.h"
#include "datatypes/Vec.h"
#include "storm/Isotach.h"
#include "storm/StormTranslation.h"

namespace Gahm::Storm {

/**
 * @class Quadrant
 * @brief A class representing a quadrant of a storm.
 * @details A quadrant is a section of a storm that is divided into four equal
 * parts. Each quadrant has three isotachs that represent the wind speed at
 * different radii. The isotachs are stored in an array of Isotach objects.
 */
class Quadrant {
 public:
  /**
   * @brief Default constructor for the Quadrant class.
   */
  constexpr Quadrant() : m_isotachs(), m_quadrant_code(Types::QuadCode::NE) {}

  /**
   * @brief Constructor for the Quadrant class.
   * @param code Quadrant code
   * @param latitude Latitude of the storm center in degrees
   * @param isotachs Array of Isotach objects describing the wind speed at
   * different radii
   */
  constexpr Quadrant(Types::QuadCode::QuadrantCode code, double latitude,
                     const std::array<Isotach, 3>& isotachs)
      : m_isotachs(isotachs),
        m_quadrant_code(code),
        m_unit_vector_tbl(Types::QuadUnitVec::quadrant_unit_vector(
            m_quadrant_code, latitude)) {}
  /**
   * Returns the quadrant code.
   * @return Quadrant code
   */
  [[nodiscard]] constexpr auto quadrant_code() const
      -> Types::QuadCode::QuadrantCode {
    return m_quadrant_code;
  }

  /**
   * Returns the isotachs
   * @return Array of Isotach objects
   */
  [[nodiscard]] constexpr auto isotachs() const
      -> const std::array<Isotach, 3>& {
    return m_isotachs;
  }

  /**
   * Returns the isotach at the specified index.
   * @param index Index of the isotach
   * @return Isotach object
   */
  [[nodiscard]] constexpr auto isotach(size_t index) const -> const Isotach& {
    assert(index < 3);
    return m_isotachs.at(index);
  }

  [[nodiscard]] constexpr auto isotach(size_t index) -> Isotach& {
    assert(index < 3);
    return m_isotachs.at(index);
  }

  /**
   * Sets the isotachs.
   * @param isotachs Array of Isotach objects
   */
  void set_isotachs(const std::array<Isotach, 3>& isotachs) {
    m_isotachs = isotachs;
  }

  /**
   * Sets the unit vector at the top of the boundary layer.
   * @param vec Unit vector
   */
  void set_unit_vector_tbl(const Types::Vec& vec) { m_unit_vector_tbl = vec; }

  /**
   * Returns the unit vector at the top of the boundary layer.
   * @return Unit vector
   */
  [[nodiscard]] constexpr auto unit_vector_tbl() const -> const Types::Vec& {
    return m_unit_vector_tbl;
  }

  /**
   * Returns the number of populated isotachs.
   * @return Number of populated isotachs
   */
  [[nodiscard]] constexpr auto n_populated_isotachs() const -> size_t {
    return m_valid_isotachs.size();
  }

  [[nodiscard]] constexpr auto valid_isotachs() const
      -> const std::vector<Isotach>& {
    return m_valid_isotachs;
  }

  /**
   * Computes the GAHM parameters for the quadrant.
   */
  void compute_gahm_parameters(const StormTranslation& translation,
                               const Types::Point& eye_location,
                               double central_pressure,
                               double background_pressure, double v_max);

 private:
  std::array<Isotach, 3> m_isotachs;
  Types::QuadCode::QuadrantCode m_quadrant_code;
  Types::Vec m_unit_vector_tbl;
  std::vector<Isotach> m_valid_isotachs;
};

}  // namespace Gahm::Storm

auto operator<<(std::ostream& stream,
                const Gahm::Storm::Quadrant& quadrant) -> std::ostream&;

#endif  // GAHM_QUADRANT_H
