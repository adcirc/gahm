// MIT License
//
// Copyright (c) 2023 ADCIRC Development Group
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
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
