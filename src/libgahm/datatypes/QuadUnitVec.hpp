//
// Created by Zach Cobell on 8/13/24.
//

#ifndef GAHM_QUADUNITVEC_HPP
#define GAHM_QUADUNITVEC_HPP

#include <cmath>

#include "datatypes/QuadCode.hpp"
#include "datatypes/Vec.hpp"
#include "physical/Constants.hpp"

namespace Gahm::Types::QuadUnitVec {

namespace detail {
/**
 * @brief Generates a unit vector from the given values and latitude where the
 * latitude is used to determine the sign of the unit vector
 *
 * @param value1 Unit vector value 1
 * @param value2 Unit vector value 2
 * @param latitude Latitude of the storm
 * @return Unit vector
 */
constexpr auto generate_unit_vector(double value1, double value2,
                                    double latitude) noexcept
    -> Gahm::Types::Vec {
  return {
      value1 / Gahm::Physical::Constants::sqrt2() * copysign(1.0, latitude),
      value2 / Gahm::Physical::Constants::sqrt2() * copysign(1.0, latitude)};
}

}  // namespace detail

/**
 * @brief Returns the unit vector for the given quadrant code and latitude
 * @param code Quadrant code for the unit vector
 * @param latitude Latitude of the storm
 * @return Unit vector for the given quadrant code and latitude
 */
[[nodiscard]] constexpr auto quadrant_unit_vector(
    Types::QuadCode::QuadrantCode code,
    double latitude) noexcept -> Gahm::Types::Vec {
  switch (code) {
    case Types::QuadCode::QuadrantCode::NE:
      return detail::generate_unit_vector(-1, 1, latitude);
    case Types::QuadCode::QuadrantCode::SE:
      return detail::generate_unit_vector(1, 1, latitude);
    case Types::QuadCode::QuadrantCode::SW:
      return detail::generate_unit_vector(1, -1, latitude);
    case Types::QuadCode::QuadrantCode::NW:
      return detail::generate_unit_vector(-1, -1, latitude);
    default:
      return {0, 0};  // Unreachable
  }
}

}  // namespace Gahm::Types::QuadUnitVec

#endif  // GAHM_QUADUNITVEC_HPP
