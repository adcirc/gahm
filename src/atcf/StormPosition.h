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
#ifndef GAHM_SRC_ATCF_STORMPOSITION_H_
#define GAHM_SRC_ATCF_STORMPOSITION_H_

#include "datatypes/Point.h"
#include "util/Interpolation.h"

namespace Gahm::Atcf {

class StormPosition {
 public:
  StormPosition() = default;

  StormPosition(double x, double y) : m_point(x, y) {}

  [[nodiscard]] double x() const { return m_point.x(); }
  void setX(double x) { m_point.setX(x); }

  [[nodiscard]] double y() const { return m_point.y(); }
  void setY(double y) { m_point.setY(y); }

  [[nodiscard]] Datatypes::Point point() const { return m_point; }

  [[nodiscard]] const Datatypes::Point &pointRef() const { return m_point; }

  static StormPosition interpolate(const StormPosition &p1,
                                   const StormPosition &p2, double t) {
    return {Interpolation::linear(p1.x(), p2.x(), t),
            Interpolation::linear(p1.y(), p2.y(), t)};
  }

 private:
  Datatypes::Point m_point;
};

}  // namespace Gahm::Atcf

#endif  // GAHM_SRC_ATCF_STORMPOSITION_H_
