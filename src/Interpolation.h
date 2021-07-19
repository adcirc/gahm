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
#ifndef GAHM_SRC_INTERPOLATION_H_
#define GAHM_SRC_INTERPOLATION_H_

namespace Interpolation {
/**
 * Computes a linear interpolation between two values with a specified
 * weighting factor
 * @param[in] weight weighting factor
 * @param[in] v1 value 1 for weighting
 * @param[in] v2 value 2 for weighting
 * @return interpolated value
 */
static constexpr double linearInterp(const double weight, const double v1,
                                     const double v2) {
  return (1.0 - weight) * v1 + weight * v2;
}

static double angleInterp(const double weight, const double v1,
                          const double v2) {
  constexpr double min_angle =
      10.0 * Units::convert(Units::Degree, Units::Radian);

  // if (v2 - v1 < min_angle) {
  //  double v = linearInterp(weight, v1, v2);
  //  if(v < 0.0 ) return v+= Constants::twopi();
  //  if(v > Constants::twopi()) return v -= Constants::twopi();
  //  assert( v > 0.0 );
  //  assert( v < Constants::twopi());
  //  return v;
  //} else {
  double v1x = std::cos(v1);
  double v1y = std::sin(v1);
  double v2x = std::cos(v2);
  double v2y = std::sin(v2);
  double v3x = Interpolation::linearInterp(weight, v1x, v2x);
  double v3y = Interpolation::linearInterp(weight, v1y, v2y);
  double v = std::atan2(v3y, v3x);
  return v;
  //}
}

static constexpr double quadrantInterp(double power1, double power2,
                                       double value1, double value2) {
  return ((value1 * power1) + (value2 * power2)) / (power1 + power2);
}

}  // namespace Interpolation

#endif  // GAHM_SRC_INTERPOLATION_H_
