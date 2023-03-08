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
#ifndef GAHM_SRC_GAHMRADIUSSOLVERPRIVATE_H_
#define GAHM_SRC_GAHMRADIUSSOLVERPRIVATE_H_

#include <cassert>
#include <cmath>
#include <utility>

#include "physical/Atmospheric.h"
#include "physical/Constants.h"

namespace Gahm::Solver {

class GahmRadiusSolverPrivate {
 public:
  GahmRadiusSolverPrivate(double isotachRadius, double isotachSpeed,
                          double vmax, double fc, double bg);

  [[nodiscard]] std::pair<double, double> operator()(
      const double &radiusToMaxWinds) const;

  void setBg(double bg);
  [[nodiscard]] double bg() const;

 private:
  static double f(double radius_to_max_winds, double vmax, double isotach_speed,
                  double isotach_radius, double fc, double bg);
  static double f_prime(double radius_to_max_winds, double vmax,
                        double isotach_radius, double fc, double bg);

  double m_isotachRadius;
  double m_isotachSpeed;
  double m_vmax;
  double m_fc;
  double m_bg;
};
}  // namespace Gahm::Solver

#endif  // GAHM_SRC_GAHMRADIUSSOLVERPRIVATE_H_
