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
#ifndef GAHM_SRC_GAHMRADIUSSOLVER_H_
#define GAHM_SRC_GAHMRADIUSSOLVER_H_

#include <cstddef>

#include "gahm/GahmRadiusSolverPrivate.h"

namespace Gahm::Solver {
class GahmRadiusSolver {
 public:
  GahmRadiusSolver(double isotach_radius, double isotach_speed, double vmax,
                   double fc, double bg);

  [[nodiscard]] double solve(double lower, double upper, double guess) const;

  void setBg(double bg);

  [[nodiscard]] double bg() const;

 private:
  GahmRadiusSolverPrivate m_solver;
  const size_t m_max_it;
  const bool m_solver_type;
};
}  // namespace Gahm::Solver
#endif  // GAHM_SRC_GAHMRADIUSSOLVER_H_
