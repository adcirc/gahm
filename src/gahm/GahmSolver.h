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
#ifndef GAHM_SRC_GAHMSOLVER_H_
#define GAHM_SRC_GAHMSOLVER_H_

#include "gahm/GahmRadiusSolver.h"
#include "physical/Constants.h"

namespace Gahm::Solver {

class GahmSolver {
 public:
  GahmSolver(double isotach_radius, double isotach_speed, double vmax,
             double p_center, double p_background, double latitude);

  void solve();

  [[nodiscard]] double isotachRadius() const;

  [[nodiscard]] double isotachSpeed() const;

  [[nodiscard]] double latitude() const;

  [[nodiscard]] double pc() const;

  [[nodiscard]] double pbk() const;

  [[nodiscard]] double fc() const;

  [[nodiscard]] double vmax() const;

  [[nodiscard]] double rmax() const;

  [[nodiscard]] double bg() const;

  [[nodiscard]] double phi() const;

  [[nodiscard]] size_t it() const;

 private:
  static double estimateRmax(double dp, double lat, double isorad);

  double m_isotachRadius;
  double m_isotachSpeed;
  double m_vmax;
  double m_pc;
  double m_pbk;
  double m_latitude;
  double m_fc;
  double m_rmax_guess;
  double m_rmax;
  double m_bg;
  double m_bg_tol;
  double m_phi;
  size_t m_max_it;
  size_t m_it;
  GahmRadiusSolver m_solver;
};
}  // namespace Gahm::Solver

#endif  // GAHM_SRC_GAHMSOLVER_H_
