//
// Created by Zach Cobell on 8/4/24.
//

#include "Quadrant.h"

#include <algorithm>
#include <ostream>

#include "datatypes/Point.h"
#include "storm/Isotach.h"
#include "storm/StormTranslation.h"

namespace Gahm::Storm {

/**
 * @brief Constructor for the Quadrant class.
 *
 * @param translation Storm translation object
 * @param eye_location Eye location of the storm
 * @param central_pressure Central pressure of the storm
 * @param background_pressure Background pressure of the storm
 * @param v_max Maximum wind speed of the storm
 */
void Quadrant::compute_gahm_parameters(
    const Gahm::Storm::StormTranslation& translation,
    const Types::Point& eye_location, double central_pressure,
    double background_pressure, double v_max) {
  std::ranges::for_each(m_isotachs, [&](Isotach& isotach) {
    if (isotach.is_populated()) {
      isotach.compute_gahm_parameters(translation, eye_location,
                                      m_unit_vector_tbl, central_pressure,
                                      background_pressure, v_max);
    }
  });
}

}  // namespace Gahm::Storm

auto operator<<(std::ostream& stream,
                const Gahm::Storm::Quadrant& quadrant) -> std::ostream& {
  for (const auto& isotach : quadrant.isotachs()) {
    if (isotach.is_populated()) {
      stream << "     Isotach: " << isotach << "\n";
    }
  }
  return stream;
}
