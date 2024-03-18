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
#ifndef GAHM_INTERPOLATION_H
#define GAHM_INTERPOLATION_H

#include <cmath>

namespace Gahm::Interpolation {

/**
 * @brief Linear interpolation
 * @param v0 First value
 * @param v1 Second value
 * @param weight Weight
 * @return Linearly interpolated value
 */
constexpr auto linear(double v0, double v1, double weight) noexcept -> double {
  return (v0 * (1.0 - weight)) + (v1 * weight);
}

constexpr auto angle(double v0, double v1, double weight,
                     bool wrap = false) noexcept -> double {
  if (wrap && v1 < v0) v1 += Gahm::Physical::Constants::twoPi();
  auto angle = linear(v0, v1, weight);
  if (angle < 0.0) {
    angle += Gahm::Physical::Constants::twoPi();
  } else if (angle > Gahm::Physical::Constants::twoPi()) {
    angle -= Gahm::Physical::Constants::twoPi();
  }
  return angle;
}

}  // namespace Gahm::Interpolation

#endif  // GAHM_INTERPOLATION_H
