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
#include <cmath>

#include "physical/Atmospheric.h"

namespace Gahm::Solver::GahmEquations {

auto GahmFunction(double radius_to_max_wind, double vmax_at_boundary_layer,
                  double isotach_windspeed_at_boundary_layer, double distance,
                  double coriolis_force, double gahm_holland_b, double phi)
    -> double;

auto GahmFunction(double radius_to_max_wind, double vmax_at_boundary_layer,
                  double isotach_windspeed_at_boundary_layer, double distance,
                  double coriolis_force, double gahm_holland_b) -> double;

auto GahmFunctionDerivative(double radius_to_max_wind,
                            double vmax_at_boundary_layer,
                            double isotach_windspeed_at_boundary_layer,
                            double coriolis_force, double gahm_holland_b,
                            double phi) -> double;

auto GahmFunctionDerivative(double radius_to_max_wind,
                            double vmax_at_boundary_layer,
                            double isotach_windspeed_at_boundary_layer,
                            double coriolis_force, double gahm_holland_b)
    -> double;

auto GahmPressure(double central_pressure, double background_pressure,
                  double distance, double radius_to_max_winds,
                  double gahm_holland_b, double phi) -> double;

auto GahmWindSpeed(double radius_to_max_wind, double vmax_at_boundary_layer,
                   double distance, double coriolis, double gahm_holland_b)
    -> double;

/**
 * Compute the GAHM phi parameter
 * @param vmax maximum storm wind velocity
 * @param rmax radius to max winds
 * @param b GAHM holland b parameter
 * @param f_coriolis coriolis force
 * @return phi
 */
constexpr auto phi(double vmax, double rmax, double gahm_b, double f_coriolis)
    -> double {
  assert(f_coriolis > 0.0);
  assert(vmax > 0.0);
  assert(rmax > 0.0);
  const auto rossby =
      Gahm::Physical::Atmospheric::rossbyNumber(vmax, rmax, f_coriolis);
  return 1.0 + (1.0 / (rossby * gahm_b * (1.0 + 1.0 / rossby)));
}

/**
 * Computes the GAHM modified Holland B
 * @param vmax maximum storm wind velocity
 * @param rmax radius to maximum winds
 * @param dp pressure deficit
 * @param f_coriolis coriolis force
 * @param bg current value for GAHM Holland B
 * @param phi GAHM Phi parameter
 * @return GAHM Holland B
 */
static auto gahm_b(double vmax, double rmax, double p_center,
                   double p_background, double f_coriolis, double phi)
    -> double {
  const auto holland_b =
      Gahm::Physical::Atmospheric::calcHollandB(vmax, p_center, p_background);
  const auto rossby =
      Gahm::Physical::Atmospheric::rossbyNumber(vmax, rmax, f_coriolis);
  const auto gahm_b =
      (holland_b * ((1 + 1 / rossby) * std::exp(phi - 1)) / phi);
  return gahm_b;
}
}  // namespace Gahm::Solver::GahmEquations
#endif  // GAHM_SRC_GAHMEQUATIONS_H_
