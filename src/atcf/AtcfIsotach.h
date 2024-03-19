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

#include <array>
#include <cassert>
#include <vector>

#include "atcf/AtcfQuadrant.h"
#include "datatypes/CircularArray.h"

#ifdef SWIG
#define NODISCARD
#else
#define NODISCARD [[nodiscard]]
#endif

namespace Gahm::Atcf {

class AtcfIsotach {
 public:
  AtcfIsotach() = default;

  AtcfIsotach(double wind_speed, const std::array<double, 4> &radii)
      : m_wind_speed(wind_speed),
        m_quadrants({AtcfQuadrant(0, radii[0]), AtcfQuadrant(1, radii[1]),
                     AtcfQuadrant(2, radii[2]), AtcfQuadrant(3, radii[3])}) {}

  NODISCARD auto windSpeed() const -> double { return m_wind_speed; }

  NODISCARD const Gahm::Datatypes::CircularArray<Gahm::Atcf::AtcfQuadrant, 4> &
  quadrants() const {
    return m_quadrants;
  }

  Gahm::Datatypes::CircularArray<Gahm::Atcf::AtcfQuadrant, 4> &quadrants() {
    return m_quadrants;
  }

  NODISCARD const Gahm::Atcf::AtcfQuadrant &quadrant(int quadrant_index) const {
    return m_quadrants[quadrant_index];
  }

  NODISCARD Gahm::Atcf::AtcfQuadrant &quadrant(int quadrant_index) {
    return m_quadrants[quadrant_index];
  }

  NODISCARD auto radii() const -> Gahm::Datatypes::CircularArray<double, 4> {
    return Gahm::Datatypes::CircularArray<double, 4>(std::array<double, 4>{
        m_quadrants[0].isotachRadius(), m_quadrants[1].isotachRadius(),
        m_quadrants[2].isotachRadius(), m_quadrants[3].isotachRadius()});
  }

 private:
  double m_wind_speed;
  Gahm::Datatypes::CircularArray<Gahm::Atcf::AtcfQuadrant, 4> m_quadrants;
};

}  // namespace Gahm::Atcf

#endif  // GAHM_SRC_ATCF_ATCFISOTACH_H_
