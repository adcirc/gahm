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
                          double vmax, double f_coriolis, double gahm_b);

  [[nodiscard]] auto operator()(const double &radiusToMaxWinds) const
      -> std::pair<double, double>;

  void setGahmB(double gahm_b);
  [[nodiscard]] auto gahm_b() const -> double;

 private:
  static auto f(double radius_to_max_winds, double vmax, double isotach_speed,
                double isotach_radius, double f_coriolis, double gahm_b)
      -> double;
  static auto f_prime(double radius_to_max_winds, double vmax,
                      double isotach_radius, double f_coriolis, double gahm_b)
      -> double;

  double m_isotachRadius;
  double m_vmax;
  double m_f_coriolis;
  double m_isotachSpeed;
  double m_gahm_b;
};
}  // namespace Gahm::Solver

#endif  // GAHM_SRC_GAHMRADIUSSOLVERPRIVATE_H_
