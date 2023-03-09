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
