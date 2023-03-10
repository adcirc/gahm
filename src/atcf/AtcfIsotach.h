// GNU General Public License v3.0
//
// This file is part of the GAHM model (https://github.com/adcirc/gahm).
// Copyright (c) 2023 ADCIRC Development Group.
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, version 3.
//
// This program is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program. If not, see <http://www.gnu.org/licenses/>.
//
// Author: Zach Cobell
// Contact: zcobell@thewaterinstitute.org
//
#ifndef GAHM_SRC_ATCF_ATCFISOTACH_H_
#define GAHM_SRC_ATCF_ATCFISOTACH_H_

#include <cassert>

#include "atcf/AtcfQuadrant.h"
#include "datatypes/CircularArray.h"

namespace Gahm::Atcf {

class AtcfIsotach {
 public:
  AtcfIsotach(double wind_speed, const std::array<double, 4> &radii)
      : m_wind_speed(wind_speed),
        m_quadrants({AtcfQuadrant(0, radii[0]), AtcfQuadrant(1, radii[1]),
                     AtcfQuadrant(2, radii[2]), AtcfQuadrant(3, radii[3])}) {}

  [[nodiscard]] double getWindSpeed() const { return m_wind_speed; }

  [[nodiscard]] const Gahm::Datatypes::CircularArray<AtcfQuadrant, 4>
      &getQuadrants() const {
    return m_quadrants;
  }

  Gahm::Datatypes::CircularArray<AtcfQuadrant, 4> &getQuadrants() {
    return m_quadrants;
  }

  [[nodiscard]] const Gahm::Atcf::AtcfQuadrant &getQuadrant(
      int quadrant_index) const {
    assert(quadrant_index >= 0 && quadrant_index < 4);
    return m_quadrants[quadrant_index];
  }

  [[nodiscard]] Gahm::Atcf::AtcfQuadrant &getQuadrant(int quadrant_index) {
    assert(quadrant_index >= 0 && quadrant_index < 4);
    return m_quadrants[quadrant_index];
  }

 private:
  double m_wind_speed;
  Gahm::Datatypes::CircularArray<AtcfQuadrant, 4> m_quadrants;
};

}  // namespace Gahm::Atcf

#endif  // GAHM_SRC_ATCF_ATCFISOTACH_H_
