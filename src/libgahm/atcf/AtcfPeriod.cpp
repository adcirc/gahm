//
// Created by Zach Cobell on 7/30/24.
//

#include "AtcfPeriod.h"

#include <algorithm>
#include <cstddef>
#include <iostream>
#include <numeric>

#include "datatypes/QuadCode.h"
#include "storm/Quadrant.h"

namespace Gahm::Atcf {

/**
 * Compute the GAHM parameters for each quadrant
 */
void AtcfPeriod::compute_gahm_parameters() {
  std::ranges::for_each(m_quadrants, [this](Storm::Quadrant &quad) {
    quad.compute_gahm_parameters(m_translation, m_eye_location,
                                 m_central_pressure, m_background_pressure,
                                 m_v_max);
  });
}

}  // namespace Gahm::Atcf

auto operator<<(std::ostream &stream,
                const Gahm::Atcf::AtcfPeriod &period) -> std::ostream & {
  stream << "Date: " << period.datetime()
         << ", Lat: " << period.eye_location().x()
         << ", Lon: " << period.eye_location().y()
         << ", Max Wind: " << period.v_max()
         << ", Min Pressure: " << period.central_pressure()
         << ", Max Radius: " << period.r_max() << "\n"
         << "  Quadrants: \n";

  for (const auto &quad : period.quadrants()) {
    stream
        << "    Quadrant "
        << Gahm::Types::QuadCode::quadrant_code_to_string(quad.quadrant_code())
        << " ("
        << Gahm::Types::QuadCode::quadrant_code_to_integer(quad.quadrant_code())
        << "): \n";
    stream << quad;
  }
  stream << period.translation();
  return stream;
}