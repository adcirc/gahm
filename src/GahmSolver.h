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
#ifndef GAHM_SRC_GAHMSOLVER_H_
#define GAHM_SRC_GAHMSOLVER_H_

#include "GahmRadiusSolver.h"
#include "Physical.h"

namespace Gahm {

class GahmSolver {
 public:
  GahmSolver(double isotach_radius, double isotach_speed, double vmax,
             double p_center, double p_background, double latitude);

  void solve();

  double isotachRadius() const;

  double isotachSpeed() const;

  double latitude() const;

  double pc() const;

  double pbk() const;

  double fc() const;

  double vmax() const;

  double rmax() const;

  double bg() const;

  size_t it() const;

 private:
  static double estimateRmax(double dp, double lat, double isorad);

  const double m_isotachRadius;
  const double m_vmax;
  const double m_latitude;
  const double m_fc;
  const double m_isotachSpeed;
  const double m_pc;
  const double m_pbk;
  const double m_rmax_guess;
  const size_t m_max_it;
  const double m_bg_tol;
  double m_bg;
  double m_rmax;
  double m_phi;
  size_t m_it;
  Gahm::GahmRadiusSolver m_solver;
};
}  // namespace Gahm

#endif  // GAHM_SRC_GAHMSOLVER_H_
