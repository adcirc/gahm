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
#ifndef GAHM_SRC_GAHMSOLVER_H_
#define GAHM_SRC_GAHMSOLVER_H_

#include <cstddef>

#include "gahm/GahmRadiusSolver.h"

namespace Gahm::Solver {

class GahmSolver {
 public:
  GahmSolver(double isotach_radius, double isotach_speed, double vmax,
             double p_center, double p_background, double latitude);

  void solve();

  [[nodiscard]] auto isotachRadius() const -> double;

  [[nodiscard]] auto isotachSpeed() const -> double;

  [[nodiscard]] auto latitude() const -> double;

  [[nodiscard]] auto p_center() const -> double;

  [[nodiscard]] auto p_background() const -> double;

  [[nodiscard]] auto f_coriolis() const -> double;

  [[nodiscard]] auto vmax() const -> double;

  [[nodiscard]] auto rmax() const -> double;

  [[nodiscard]] auto gahm_b() const -> double;

  [[nodiscard]] auto phi() const -> double;

  [[nodiscard]] auto it() const -> size_t;

 private:
  static auto estimateRmax(double dp, double lat, double isorad) -> double;

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
