//
// Created by Zach Cobell on 8/12/24.
//

#include "GahmEquations.h"

#include <cmath>

#include "physical/Atmospheric.h"

/**
 * Function to compute radius to max wind \f$V_g(r)\f$
 * \f[
 * V_g(r) =
 * \sqrt{\frac{f^2r^2}{4}+{v_m}^2{e}^\beta\left(\gamma+1\right)\alpha^{b_g}}-\frac{fr}{2}
 * \f]
 * where:
 * \f$\alpha = \frac{r_m}{r}\f$
 * \f$\beta  = -\phi \left(\alpha^{b_g}-1\right)\f$
 * \f$\gamma = \frac{fr_m}{v_m}\f$
 *
 * @param radius_to_max_winds radius to max winds
 * @param vmax_at_boundary_layer maximum wind speed
 * @param isotach_windspeed_at_boundary_layer speed of the current isotach
 * @param distance radius of the current isotach
 * @param coriolis_force coriolis force
 * @param gahm_holland_b GAHM Holland B
 * @return Solution to gradient wind
 */
auto Gahm::Solver::GahmEquations::GahmFunction(
    double radius_to_max_winds, double vmax_at_boundary_layer,
    double isotach_windspeed_at_boundary_layer, double distance,
    double coriolis_force, double gahm_holland_b, double phi) -> double {
  const auto rossby = Gahm::Physical::Atmospheric::rossbyNumber(
      vmax_at_boundary_layer, radius_to_max_winds, coriolis_force);
  const auto rmbg = std::pow(radius_to_max_winds / distance, gahm_holland_b);
  const auto sign_of_coriolis = copysign(1.0, coriolis_force);
  return (sign_of_coriolis *
              std::sqrt(std::pow(vmax_at_boundary_layer, 2.0) *
                            (1.0 + 1.0 / rossby) *
                            std::exp(phi * (1.0 - rmbg)) * rmbg +
                        std::pow((distance * coriolis_force) / 2.0, 2.0)) -
          (distance * coriolis_force) / 2.0) -
         isotach_windspeed_at_boundary_layer;
}

/**
 * @overload GahmFunction
 *
 * @param radius_to_max_winds radius to max winds
 * @param vmax_at_boundary_layer maximum wind speed
 * @param isotach_windspeed_at_boundary_layer speed of the current isotach
 * @param distance radius of the current isotach
 * @param coriolis_force coriolis force
 * @param gahm_holland_b GAHM Holland B
 * @return Solution to gradient wind
 *
 * @see GahmFunction
 *
 * This function is called when the phi value is not known and must be
 * computed
 */
auto Gahm::Solver::GahmEquations::GahmFunction(
    double radius_to_max_winds, double vmax_at_boundary_layer,
    double isotach_windspeed_at_boundary_layer, double distance,
    double coriolis_force, double gahm_holland_b) -> double {
  const auto phi_local =
      GahmEquations::phi(vmax_at_boundary_layer, radius_to_max_winds,
                         gahm_holland_b, coriolis_force);
  return GahmFunction(radius_to_max_winds, vmax_at_boundary_layer,
                      isotach_windspeed_at_boundary_layer, distance,
                      coriolis_force, gahm_holland_b, phi_local);
}

/**
 * Function to computeRadiusToMaxWind \f$V_g^{\prime}(r)\f$
 * \f[ V_g^{\prime}(r) = \frac{f v_m
 * e^{\beta}{\alpha}^{b_g}+\frac{b_g{v_m}^2{e}^{\beta}\left(\gamma+1\right)\alpha^{b_g-1}}{r}-\frac{b_g\phi
 * {v_m}^2{e}^{\beta}\left(\gamma+1\right)\alpha^{b_g}\alpha^{b_g-1}}{r}}{2\sqrt{\frac{f^2r^2}{4}+{v_m}^2{e}^{\beta}\left(\gamma+1\right)\alpha^{b_g}}}
 * \f]
 * * where:
 * \f$\alpha = \frac{r_m}{r}\f$
 * \f$\beta  = -\phi \left(\alpha^{b_g}-1\right)\f$
 * \f$\gamma = \frac{fr_m}{v_m}\f$
 *
 * @param radius_to_max_winds Radius to max winds
 * @param vmax Maximum wind speed
 * @param isotach_radius radius of the current isotach
 * @param coriolis_force coriolis force
 * @param gahm_holland_b GAHM Holland B parameter
 * @return Solution to first derivative
 */
auto Gahm::Solver::GahmEquations::GahmFunctionDerivative(
    double radius_to_max_winds, double vmax_at_boundary_layer,
    double isotach_radius, double coriolis_force, double gahm_holland_b,
    double phi) -> double {
  const auto f3 = std::pow(radius_to_max_winds / isotach_radius, gahm_holland_b);
  const auto f4 =
      std::pow(radius_to_max_winds / isotach_radius, gahm_holland_b - 1.0);
  const auto f1 = std::exp(-phi * (f3 - 1));
  const auto f2 =
      ((coriolis_force * radius_to_max_winds) / vmax_at_boundary_layer) + 1;
  const auto a = coriolis_force * vmax_at_boundary_layer * f1 * f3;
  const auto b = (gahm_holland_b * vmax_at_boundary_layer *
                  vmax_at_boundary_layer * f1 * f2 * f4) /
                 isotach_radius;
  const auto c = (gahm_holland_b * phi * vmax_at_boundary_layer *
                  vmax_at_boundary_layer * f1 * f2 * f3 * f4) /
                 isotach_radius;
  const auto d =
      2.0 *
      std::sqrt(
          (coriolis_force * coriolis_force * isotach_radius * isotach_radius) /
              4.0 +
          (vmax_at_boundary_layer * vmax_at_boundary_layer * f1 * f2 * f3));
  return (a + b - c) / d;
}

/**
 * @overload GahmFunctionDerivative
 *
 * @param radius_to_max_winds Radius to max winds
 * @param vmax_at_boundary_layer Maximum wind speed
 * @param isotach_radius radius of the current isotach
 * @param coriolis_force coriolis force
 * @param gahm_holland_b GAHM Holland B parameter
 * @return Solution to first derivative
 */
auto Gahm::Solver::GahmEquations::GahmFunctionDerivative(
    double radius_to_max_winds, double vmax_at_boundary_layer,
    double isotach_radius, double coriolis_force,
    double gahm_holland_b) -> double {
  return GahmFunctionDerivative(
      radius_to_max_winds, vmax_at_boundary_layer, isotach_radius,
      coriolis_force, gahm_holland_b,
      GahmEquations::phi(vmax_at_boundary_layer, radius_to_max_winds,
                         gahm_holland_b, coriolis_force));
}

/**
 * Compute the GAHM pressure field
 * @param central_pressure Storm central pressure
 * @param background_pressure Background pressure
 * @param distance Distance from the storm center
 * @param radius_to_max_winds Radius to max winds
 * @param gahm_holland_b gahm holland b parameter
 * @param phi GAHM phi parameter
 * @return Pressure at distance from the storm center
 */
auto Gahm::Solver::GahmEquations::GahmPressure(
    double central_pressure, double background_pressure, double distance,
    double radius_to_max_winds, double gahm_holland_b, double phi) -> double {
  return central_pressure +
         (background_pressure - central_pressure) *
             std::exp(-phi *
                      std::pow(radius_to_max_winds / distance, gahm_holland_b));
}

/**
 * @brief Compute the GAHM wind speed
 * @param radius_to_max_winds Radius to max winds
 * @param vmax_at_boundary_layer Maximum wind speed
 * @param distance Distance from the storm center
 * @param coriolis Coriolis force
 * @param gahm_holland_b GAHM Holland B
 * @return Wind speed at distance from the storm center
 */
auto Gahm::Solver::GahmEquations::GahmWindSpeed(
    double radius_to_max_winds, double vmax_at_boundary_layer, double distance,
    double coriolis, double gahm_holland_b) -> double {
  return Gahm::Solver::GahmEquations::GahmFunction(
      radius_to_max_winds, vmax_at_boundary_layer, 0.0, distance, coriolis,
      gahm_holland_b);
}