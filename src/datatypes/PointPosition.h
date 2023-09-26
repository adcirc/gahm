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
#ifndef GAHM_POINTPOSITION_H
#define GAHM_POINTPOSITION_H

#include <cstddef>

#ifndef SWIG
#define NODISCARD [[nodiscard]]
#else
#define NODISCARD
#endif

namespace Gahm::Datatypes {

class PointPosition {
 public:
  PointPosition(int isotach, int quadrant, double isotach_weight,
                double quadrant_weight, int isotach_adjacent,
                double isotach_adjacent_weight)
      : m_isotach(isotach),
        m_quadrant(quadrant),
        m_isotach_weight(isotach_weight),
        m_quadrant_weight(quadrant_weight),
        m_isotach_adjacent(isotach_adjacent),
        m_isotach_adjacent_weight(isotach_adjacent_weight) {}

  NODISCARD int isotach() const { return m_isotach; }

  NODISCARD int quadrant() const { return m_quadrant; }

  NODISCARD double isotach_weight() const { return m_isotach_weight; }

  NODISCARD double quadrant_weight() const { return m_quadrant_weight; }

  NODISCARD int isotach_adjacent() const { return m_isotach_adjacent; }

  NODISCARD double isotach_adjacent_weight() const {
    return m_isotach_adjacent_weight;
  }

 private:
  int m_isotach;
  int m_quadrant;
  double m_isotach_weight;
  double m_quadrant_weight;
  int m_isotach_adjacent;
  double m_isotach_adjacent_weight;
};
}  // namespace Gahm::Datatypes

#endif  // GAHM_POINTPOSITION_H
