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
  double m_vmax;
  double m_fc;
  double m_isotachSpeed;
  double m_bg;
};
}  // namespace Gahm::Solver

#endif  // GAHM_SRC_GAHMRADIUSSOLVERPRIVATE_H_
