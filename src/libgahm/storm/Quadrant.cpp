//
// Created by Zach Cobell on 8/4/24.
//

#include "Quadrant.h"

#include <algorithm>
#include <array>
#include <numeric>
#include <ostream>
#include <ranges>

#include "datatypes/Point.h"
#include "datatypes/QuadUnitVec.h"
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

auto Quadrant::interpolate(const Gahm::Storm::Quadrant& quadrant_1,
                           const Gahm::Storm::Quadrant& quadrant_2,
                           const double weight, double latitude) -> Quadrant {
  const auto this_unit_vector_tbl = Types::QuadUnitVec::quadrant_unit_vector(
      quadrant_1.quadrant_code(), latitude);

  std::array<Isotach, 3> this_isotachs;
  std::ranges::transform(
      quadrant_1.isotachs(), quadrant_2.isotachs(), this_isotachs.begin(),
      [&](const Isotach& isotach_1, const Isotach& isotach_2) {
        return Isotach::interpolate(isotach_1, isotach_2, weight);
      });
  return {quadrant_1.quadrant_code(), this_unit_vector_tbl, this_isotachs};
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
