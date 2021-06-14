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
#include "VortexSolver.h"

#include <cmath>
#include <iostream>

#include "Constants.h"

VortexSolverInternal::VortexSolverInternal(double rmax, double vmax, double bg,
                                           double phi, double fc, double vr)
    : m_rmax(rmax), m_vmax(vmax), m_bg(bg), m_phi(phi), m_fc(fc), m_vr(vr) {}

double VortexSolverInternal::f(const double &r) const {
  //...Reused variables
  const double vmaxSq = std::pow(m_vmax, 2.0);
  const double rOverRmaxBg = std::pow(r / m_rmax, m_bg);
  const double expPhiOneMinusROverRmaxBg =
      std::exp(m_phi * (1.0 - rOverRmaxBg));

  //...Intermediate
  const double fa =
      vmaxSq +
      m_vmax * r * Units::convert(Units::Kilometer, Units::Meter) * m_fc;
  const double fb = rOverRmaxBg * expPhiOneMinusROverRmaxBg;
  const double fc = std::pow(
      (m_rmax * Units::convert(Units::Kilometer, Units::Meter) * m_fc) / 2.0,
      2.0);
  const double fd =
      (Units::convert(Units::Kilometer, Units::Meter) * m_rmax * m_fc) / 2.0;

  //...Return
  return std::sqrt(fa * fb + fc) - fd - m_vr;
}

double VortexSolverInternal::fprime(const double &r) const {
  constexpr double dx = 0.001;
  double f0 = f(r - dx * 0.5);
  double f1 = f(r + dx * 0.5);
  return (f1 - f0) / dx;
}
