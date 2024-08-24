//
// Created by Zach Cobell on 8/13/24.
//

#ifndef GAHM_QUADCODE_H
#define GAHM_QUADCODE_H

#include <cassert>
#include <cstdint>

namespace Gahm::Types::QuadCode {

/**
 * @brief Enumerates the four quadrants of a storm.
 */
enum QuadrantCode : uint8_t { NE = 0, SE = 1, SW = 2, NW = 3 };

/**
 * @brief Converts an integer to a QuadrantCode.
 *
 * The integer must be in the range [0, 3].
 * The table is:
 * 0 -> NE
 * 1 -> SE
 * 2 -> SW
 * 3 -> NW
 *
 * @param code The integer to convert
 * @return The QuadrantCode.
 */
[[nodiscard]] static constexpr auto integer_to_quadrant_code(unsigned code)
    -> Types::QuadCode::QuadrantCode {
  assert(code < 4);
  return static_cast<Types::QuadCode::QuadrantCode>(code);
}

[[nodiscard]] static constexpr auto quadrant_code_to_integer(
    Types::QuadCode::QuadrantCode code) -> unsigned {
  return static_cast<unsigned>(code);
}

#ifndef SWIG
[[nodiscard]] static constexpr auto quadrant_code_to_string(
    Types::QuadCode::QuadrantCode code) -> const char* {
  switch (code) {
    case Types::QuadCode::QuadrantCode::NE:
      return "NE";
    case Types::QuadCode::QuadrantCode::SE:
      return "SE";
    case Types::QuadCode::QuadrantCode::SW:
      return "SW";
    case Types::QuadCode::QuadrantCode::NW:
      return "NW";
    default:
      return "Unknown";
  }
}
#endif

}  // namespace Gahm::Types::QuadCode

#endif  // GAHM_QUADCODE_H
