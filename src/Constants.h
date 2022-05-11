// MIT License
//
// Copyright (c) 2020 ADCIRC Development Group
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
#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <array>
#include <cassert>
#include <cmath>
#include <iostream>
#include <limits>
#include <tuple>

#include "UnitConversion.h"
namespace Gahm {
namespace Constants {

static constexpr std::array<double, 4> m_quadrantAngles = {
    45.0 * Units::convert(Units::Degree, Units::Radian),
    (45.0 - 90.0) * Units::convert(Units::Degree, Units::Radian),
    (45.0 - 180.0) * Units::convert(Units::Degree, Units::Radian),
    (45.0 - 270.0) * Units::convert(Units::Degree, Units::Radian)};

static constexpr double pi() { return M_PI; }
static constexpr double twopi() { return pi() * 2.0; }
static constexpr double halfpi() { return pi() * 0.5; }
static constexpr double quarterpi() { return pi() * 0.25; }
static constexpr double e() { return M_E; }

constexpr double quadrantAngle(size_t index) {
  return Constants::m_quadrantAngles[index];
}

};  // namespace Constants
}  // namespace Gahm

#endif  // CONSTANTS_H
