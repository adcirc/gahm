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
#include "GahmEquations.h"

#include <iostream>

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
 * @param radius_to_max_wind radius to max winds
 * @param vmax_at_boundary_layer maximum wind speed
 * @param isotach_windspeed_at_boundary_layer speed of the current isotach
 * @param distance radius of the current isotach
 * @param coriolis_force coriolis force
 * @param gahm_holland_b GAHM Holland B
 * @return Solution to gradient wind
 */
double Gahm::Solver::GahmEquations::GahmFunction(
    double radius_to_max_wind, double vmax_at_boundary_layer,
    double isotach_windspeed_at_boundary_layer, double distance,
    double coriolis_force, double gahm_holland_b, double phi) {
  const auto ro = Gahm::Physical::Atmospheric::rossbyNumber(
      vmax_at_boundary_layer, radius_to_max_wind, coriolis_force);
  const auto rmbg = std::pow(radius_to_max_wind / distance, gahm_holland_b);
  return (std::sqrt(std::pow(vmax_at_boundary_layer, 2.0) * (1.0 + 1.0 / ro) *
                        std::exp(phi * (1.0 - rmbg)) * rmbg +
                    std::pow((distance * coriolis_force) / 2.0, 2.0)) -
          (distance * coriolis_force) / 2.0) -
         isotach_windspeed_at_boundary_layer;
}

double Gahm::Solver::GahmEquations::GahmFunction(
    double radius_to_max_wind, double vmax_at_boundary_layer,
    double isotach_windspeed_at_boundary_layer, double distance,
    double coriolis_force, double gahm_holland_b) {
//  std::cout << radius_to_max_wind << " " << vmax_at_boundary_layer << " "
//            << isotach_windspeed_at_boundary_layer << " " << distance << " "
//            << coriolis_force << " " << gahm_holland_b << std::endl;
  const auto phi =
      GahmEquations::phi(vmax_at_boundary_layer, radius_to_max_wind,
                         gahm_holland_b, coriolis_force);
  return GahmFunction(radius_to_max_wind, vmax_at_boundary_layer,
                      isotach_windspeed_at_boundary_layer, distance,
                      coriolis_force, gahm_holland_b, phi);
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
 * @param rmax Radius to max winds
 * @param vmax Maximum wind speed
 * @param isotach_radius radius of the current isotach
 * @param coriolis_force coriolis force
 * @param gahm_holland_b GAHM Holland B parameter
 * @return Solution to first derivative
 */
double Gahm::Solver::GahmEquations::GahmFunctionDerivative(
    double radius_to_max_wind, double vmax_at_boundary_layer,
    double isotach_radius, double coriolis_force, double gahm_holland_b,
    double phi) {
  const auto f3 = std::pow(radius_to_max_wind / isotach_radius, gahm_holland_b);
  const auto f4 =
      std::pow(radius_to_max_wind / isotach_radius, gahm_holland_b - 1.0);
  const auto f1 = std::exp(-phi * (f3 - 1));
  const auto f2 =
      ((coriolis_force * radius_to_max_wind) / vmax_at_boundary_layer) + 1;
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

double Gahm::Solver::GahmEquations::GahmFunctionDerivative(
    double radius_to_max_wind, double vmax_at_boundary_layer,
    double isotach_radius, double coriolis_force, double gahm_holland_b) {
  const auto phi =
      GahmEquations::phi(vmax_at_boundary_layer, radius_to_max_wind,
                         gahm_holland_b, coriolis_force);
  return GahmFunctionDerivative(radius_to_max_wind, vmax_at_boundary_layer,
                                isotach_radius, coriolis_force, gahm_holland_b,
                                phi);
}

double Gahm::Solver::GahmEquations::GahmPressure(
    double central_pressure, double background_pressure, double distance,
    double radius_to_max_winds, double gahm_holland_b, double phi) {
  return central_pressure +
         (background_pressure - central_pressure) *
             std::exp(-phi *
                      std::pow(radius_to_max_winds / distance, gahm_holland_b));
}