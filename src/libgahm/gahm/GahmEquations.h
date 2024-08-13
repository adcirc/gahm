//
// Created by Zach Cobell on 8/12/24.
//

#ifndef GAHM_GAHMEQUATIONS_H
#define GAHM_GAHMEQUATIONS_H

#include <cassert>
#include <cmath>

#include "physical/Atmospheric.h"

namespace Gahm::Solver::GahmEquations {

[[nodiscard]] auto GahmFunction(double radius_to_max_winds,
                                double vmax_at_boundary_layer,
                                double isotach_windspeed_at_boundary_layer,
                                double distance, double coriolis_force,
                                double gahm_holland_b, double phi) -> double;

[[nodiscard]] auto GahmFunction(double radius_to_max_winds,
                                double vmax_at_boundary_layer,
                                double isotach_windspeed_at_boundary_layer,
                                double distance, double coriolis_force,
                                double gahm_holland_b) -> double;

[[nodiscard]] auto GahmFunctionDerivative(
    double radius_to_max_winds, double vmax_at_boundary_layer,
    double isotach_radius, double coriolis_force,
    double gahm_holland_b, double phi) -> double;

[[nodiscard]] auto GahmFunctionDerivative(
    double radius_to_max_winds, double vmax_at_boundary_layer,
    double isotach_radius, double coriolis_force,
    double gahm_holland_b) -> double;

[[nodiscard]] auto GahmPressure(double central_pressure,
                                double background_pressure, double distance,
                                double radius_to_max_winds,
                                double gahm_holland_b, double phi) -> double;

[[nodiscard]] auto GahmWindSpeed(double radius_to_max_winds,
                                 double vmax_at_boundary_layer, double distance,
                                 double coriolis,
                                 double gahm_holland_b) -> double;

/**
 * Compute the GAHM phi parameter
 * @param vmax maximum storm wind velocity
 * @param rmax radius to max winds
 * @param b GAHM holland b parameter
 * @param f_coriolis coriolis force
 * @return phi
 */
[[nodiscard]] constexpr auto phi(double vmax, double rmax, double gahm_b,
                                 double f_coriolis) -> double {
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
[[nodiscard]] static auto gahm_b(double vmax, double rmax, double dp,
                                 double f_coriolis, double phi) -> double {
  const auto holland_b = Gahm::Physical::Atmospheric::holland_b(vmax, dp);
  const auto rossby =
      Gahm::Physical::Atmospheric::rossbyNumber(vmax, rmax, f_coriolis);
  const auto gahm_b =
      (holland_b * ((1 + 1 / rossby) * std::exp(phi - 1)) / phi);
  return gahm_b;
}
}  // namespace Gahm::Solver::GahmEquations

#endif  // GAHM_GAHMEQUATIONS_H
