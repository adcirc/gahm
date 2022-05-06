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
#ifndef GAHM_SRC_GAHMRADIUSSOLVERPRIVATE_H_
#define GAHM_SRC_GAHMRADIUSSOLVERPRIVATE_H_

#include <cassert>
#include <cmath>
#include <utility>

#include "Atmospheric.h"
#include "Physical.h"

namespace Gahm {

class GahmRadiusSolverPrivate {
 public:
  GahmRadiusSolverPrivate(double isotachRadius, double isotachSpeed,
                          double vmax, double fc, double bg);

  std::pair<double, double> operator()(const double &radiusToMaxWinds) const;

  void setBg(double bg);
  double bg() const;

  /**
   * Compute the GAHM phi parameter
   * @param vmax maximum storm wind velocity
   * @param rmax radius to max winds
   * @param b GAHM holland b parameter
   * @param fc coriolis force
   * @return phi
   */
  static constexpr double computePhi(double vmax, double rmax, double bg,
                                     double fc) {
    assert(fc > 0.0);
    assert(vmax > 0.0);
    assert(rmax > 0.0);
    const auto rossby = Atmospheric::rossbyNumber(vmax, rmax, fc);
    return 1.0 + (1.0 / (rossby * bg * (1.0 + 1.0 / rossby)));
  }

  /**
   * Computes the GAHM modified Holland B
   * @param vmax maximum storm wind velocity
   * @param rmax radius to maximum winds
   * @param dp pressure deficit
   * @param fc coriolis force
   * @param bg current value for GAHM Holland B
   * @param phi GAHM Phi parameter
   * @return GAHM Holland B
   */
  static double computeBg(double vmax, double rmax, double p0, double pinf,
                          double fc, double phi) {
    const auto b = Atmospheric::calcHollandB(vmax, p0, pinf);
    const auto ro = Atmospheric::rossbyNumber(vmax, rmax, fc);
    auto bg = (b * ((1 + 1 / ro) * std::exp(phi - 1)) / phi);
    return bg;
  }

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
}  // namespace Gahm

#endif  // GAHM_SRC_GAHMRADIUSSOLVERPRIVATE_H_
