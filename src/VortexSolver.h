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
#ifndef VORTEXSOLVER_H
#define VORTEXSOLVER_H

#include <utility>

#include "AtcfLine.h"

namespace Gahm {
enum VortexSolverType { Derivative, NoDerivative };

class VortexSolverInternal {
 public:
  VortexSolverInternal(double rmax, double vmax, double bg, double phi,
                       double fc, double vr);

  template <VortexSolverType T, std::enable_if_t<T == Derivative, bool> = true>
  std::pair<double, double> operator()(const double &r) const {
    return std::make_pair(f(r), fprime(r));
  }

  template <VortexSolverType T,
            std::enable_if_t<T == NoDerivative, bool> = true>
  double operator()(const double &r) const {
    return f(r);
  }

 private:
  double m_rmax;
  double m_vmax;
  double m_bg;
  double m_phi;
  double m_fc;
  double m_vr;

  double f(const double &r) const;
  double fprime(const double &r) const;
};

template <VortexSolverType T>
class VortexSolver {
 public:
  VortexSolver(double rmax, double vmax, double bg, double phi, double fc,
               double vr)
      : m_data(rmax, vmax, bg, phi, fc, vr) {}

  VortexSolver(const AtcfLine *data, const size_t currentIsotach,
               const size_t current_quadrant)
      : m_data(
            data->radiusMaxWinds(),
            data->cisotach(currentIsotach)->cvmaxBl()->at(current_quadrant),
            data->cisotach(currentIsotach)->chollandB()->at(current_quadrant),
            data->cisotach(currentIsotach)->cphi()->at(current_quadrant),
            data->coriolis(),
            data->cisotach(currentIsotach)
                ->cquadrantVr()
                ->at(current_quadrant)) {}

  auto operator()(const double &r) { return m_data.operator()<T>(r); }

 private:
  VortexSolverInternal m_data;
};
}  // namespace Gahm
#endif  // VORTEXSOLVER_H
