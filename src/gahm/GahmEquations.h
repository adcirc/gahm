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
#ifndef GAHM_SRC_GAHMEQUATIONS_H_
#define GAHM_SRC_GAHMEQUATIONS_H_

#include <cassert>

#include "physical/Atmospheric.h"

namespace Gahm::Solver::GahmEquations {

double GahmFunction(double radius_to_max_wind, double vmax_at_boundary_layer,
                    double isotach_windspeed_at_boundary_layer, double distance,
                    double coriolis_force, double gahm_holland_b, double phi);

double GahmFunction(double radius_to_max_wind, double vmax_at_boundary_layer,
                    double isotach_windspeed_at_boundary_layer, double distance,
                    double coriolis_force, double gahm_holland_b);

double GahmFunctionDerivative(double radius_to_max_wind,
                              double vmax_at_boundary_layer,
                              double isotach_windspeed_at_boundary_layer,
                              double coriolis_force, double gahm_holland_b,
                              double phi);

double GahmFunctionDerivative(double radius_to_max_wind,
                              double vmax_at_boundary_layer,
                              double isotach_windspeed_at_boundary_layer,
                              double coriolis_force, double gahm_holland_b);

double GahmPressure(double central_pressure, double background_pressure,
                    double distance, double radius_to_max_winds,
                    double gahm_holland_b, double phi);

/**
 * Compute the GAHM phi parameter
 * @param vmax maximum storm wind velocity
 * @param rmax radius to max winds
 * @param b GAHM holland b parameter
 * @param fc coriolis force
 * @return phi
 */
constexpr double phi(double vmax, double rmax, double bg, double fc) {
  assert(fc > 0.0);
  assert(vmax > 0.0);
  assert(rmax > 0.0);
  auto rossby = Gahm::Physical::Atmospheric::rossbyNumber(vmax, rmax, fc);
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
static double bg(double vmax, double rmax, double p0, double pinf, double fc,
                 double phi) {
  auto b = Gahm::Physical::Atmospheric::calcHollandB(vmax, p0, pinf);
  auto ro = Gahm::Physical::Atmospheric::rossbyNumber(vmax, rmax, fc);
  auto bg = (b * ((1 + 1 / ro) * std::exp(phi - 1)) / phi);
  return bg;
}
}  // namespace Gahm::Solver::GahmEquations
#endif  // GAHM_SRC_GAHMEQUATIONS_H_
