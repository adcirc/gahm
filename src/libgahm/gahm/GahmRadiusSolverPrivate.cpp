//
// Created by Zach Cobell on 8/12/24.
//

#include "gahm/GahmRadiusSolverPrivate.h"

#include <tuple>
#include <utility>

#include "gahm/GahmEquations.h"

namespace Gahm::Solver::detail {

GahmRadiusSolverPrivate::GahmRadiusSolverPrivate(double isotachRadius,
                                                 double isotachSpeed,
                                                 double vmax, double f_coriolis,
                                                 double gahm_b)
    : m_isotachRadius(isotachRadius),
      m_vmax(vmax),
      m_f_coriolis(f_coriolis),
      m_isotachSpeed(isotachSpeed),
      m_gahm_b(gahm_b) {}

/**
 * Function to computeRadiusToMaxWind the Vg function and first derivative for
 * the Newton-Raphson solver
 * @param rmax radius to max winds to computeRadiusToMaxWind at
 * @return std::tuple containing the solution to Vg and Vg'
 */
auto GahmRadiusSolverPrivate::operator()(const double radius_to_max_winds) const
    -> std::tuple<double, double> {
  const double f_result =
      GahmRadiusSolverPrivate::f(radius_to_max_winds, m_vmax, m_isotachSpeed,
                                 m_isotachRadius, m_f_coriolis, m_gahm_b);
  const double f_result_prime = GahmRadiusSolverPrivate::f_prime(
      radius_to_max_winds, m_vmax, m_isotachRadius, m_f_coriolis, m_gahm_b);
  return {f_result, f_result_prime};
}

/**
 * Set the GAHM Holland B parameter
 * @param gahm_b GAHM Holland B
 */
void GahmRadiusSolverPrivate::setGahmB(double gahm_b) { m_gahm_b = gahm_b; }

/**
 * Returns the solvers current GAHM Holland B
 * @return GAHM B
 */
auto GahmRadiusSolverPrivate::gahm_b() const -> double { return m_gahm_b; }

/**
 * Solves the gahm gradient wind function
 * @param radius_to_max_winds radius to max winds
 * @param vmax maximum wind speed
 * @param isotach_speed speed of the current isotach
 * @param isotach_radius radius of the current isotach
 * @param f_coriolis coriolis force
 * @param gahm_b GAHM Holland B
 * @return Solution to gradient wind
 */
auto GahmRadiusSolverPrivate::f(double radius_to_max_winds, double vmax,
                                double isotach_speed, double isotach_radius,
                                double f_coriolis, double gahm_b) -> double {
  return Gahm::Solver::GahmEquations::GahmFunction(
      radius_to_max_winds, vmax, isotach_speed, isotach_radius, f_coriolis,
      gahm_b);
}

/**
 * Function to computeRadiusToMaxWind first derivative of gahm function
 * @param radius_to_max_winds Radius to max winds
 * @param vmax Maximum wind speed
 * @param isotach_radius radius of the current isotach
 * @param f_coriolis coriolis force
 * @param gahm_b GAHM Holland B parameter
 * @return Solution to first derivative
 */
auto GahmRadiusSolverPrivate::f_prime(double radius_to_max_winds, double vmax,
                                      double isotach_radius, double f_coriolis,
                                      double gahm_b) -> double {
  return Gahm::Solver::GahmEquations::GahmFunctionDerivative(
      radius_to_max_winds, vmax, isotach_radius, f_coriolis, gahm_b);
}
}  // namespace Gahm::Solver::detail
