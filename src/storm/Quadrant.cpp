//
// Created by Zach Cobell on 8/4/24.
//

#include "Quadrant.h"

#include <array>
#include <cmath>
#include <ostream>

#include "datatypes/RotationMatrix.h"
#include "datatypes/Vec.h"
#include "physical/Constants.h"

using namespace Gahm::Storm;
using Vec = Gahm::Types::Vec;

auto generate_unit_vector(double v1, double v2, double latitude) -> Vec {
  return {v1 / Gahm::Physical::Constants::sqrt2() * copysign(1.0, latitude),
          v2 / Gahm::Physical::Constants::sqrt2() * copysign(1.0, latitude)};
}

auto generate_unit_vectors(double lat) -> std::array<Vec, 4> {
  return {generate_unit_vector(-1, 1, lat), generate_unit_vector(1, 1, lat),
          generate_unit_vector(1, -1, lat), generate_unit_vector(-1, -1, lat)};
}

Quadrant::Quadrant(QuadrantCode code, double latitude,
                   const std::array<Isotach, 3>& isotachs)
    : m_isotachs(isotachs),
      m_quadrant_code(code),
      m_unit_vector_tbl(generate_unit_vectors(latitude)[m_quadrant_code]) {}

auto operator<<(std::ostream& stream,
                const Gahm::Storm::Quadrant& quadrant) -> std::ostream& {
  for (auto& isotach : quadrant.isotachs()) {
    stream << "Quadrant: " << isotach << "\n";
  }
  return stream;
}