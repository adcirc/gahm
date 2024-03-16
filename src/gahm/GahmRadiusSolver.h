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
#ifndef GAHM_SRC_GAHMRADIUSSOLVER_H_
#define GAHM_SRC_GAHMRADIUSSOLVER_H_

#include <cstddef>

#include "gahm/GahmRadiusSolverPrivate.h"

namespace Gahm::Solver {
class GahmRadiusSolver {
 public:
  GahmRadiusSolver(double isotach_radius, double isotach_speed, double vmax,
                   double f_coriolis, double gahm_b);

  [[nodiscard]] auto solve(double lower, double upper, double guess) const
      -> double;

  void setGahmB(double gahm_b);

  [[nodiscard]] auto gahm_b() const -> double;

 private:
  GahmRadiusSolverPrivate m_solver;
  size_t m_max_it;
};
}  // namespace Gahm::Solver
#endif  // GAHM_SRC_GAHMRADIUSSOLVER_H_
