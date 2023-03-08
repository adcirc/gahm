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
#include "gahm/GahmRadiusSolverPrivate.h"

#include "gahm/GahmEquations.h"

namespace Gahm::Solver {

GahmRadiusSolverPrivate::GahmRadiusSolverPrivate(double isotachRadius,
                                                 double isotachSpeed,
                                                 double vmax, double fc,
                                                 double bg)
    : m_isotachRadius(isotachRadius),
      m_vmax(vmax),
      m_fc(fc),
      m_isotachSpeed(isotachSpeed),
      m_bg(bg) {}

/**
 * Function to computeRadiusToMaxWind the Vg function and first derivative for
 * the Newton-Raphson solver
 * @param rmax radius to max winds to computeRadiusToMaxWind at
 * @return std::pair containing the solution to Vg and Vg'
 */
std::pair<double, double> GahmRadiusSolverPrivate::operator()(
    const double &rmax) const {
  const double vg = GahmRadiusSolverPrivate::f(rmax, m_vmax, m_isotachSpeed,
                                               m_isotachRadius, m_fc, m_bg);
  const double vgp = GahmRadiusSolverPrivate::f_prime(
      rmax, m_vmax, m_isotachRadius, m_fc, m_bg);
  return {vg, vgp};
}

/**
 * Set the GAHM Holland B parameter
 * @param bg GAHM Holland B
 */
void GahmRadiusSolverPrivate::setBg(double bg) { m_bg = bg; }

/**
 * Returns the solvers current GAHM Holland B
 * @return GAHM B
 */
double GahmRadiusSolverPrivate::bg() const { return m_bg; }

/**
 * Solves the gahm gradient wind function
 * @param rmax radius to max winds
 * @param vmax maximum wind speed
 * @param isotach_speed speed of the current isotach
 * @param isotach_radius radius of the current isotach
 * @param fc coriolis force
 * @param bg GAHM Holland B
 * @return Solution to gradient wind
 */
double GahmRadiusSolverPrivate::f(double rmax, double vmax,
                                  double isotach_speed, double isotach_radius,
                                  double fc, double bg) {
  return Gahm::Solver::GahmEquations::GahmFunction(rmax, vmax, isotach_speed,
                                                   isotach_radius, fc, bg);
}

/**
 * Function to computeRadiusToMaxWind first derivative of gahm function
 * @param rmax Radius to max winds
 * @param vmax Maximum wind speed
 * @param isotach_radius radius of the current isotach
 * @param fc coriolis force
 * @param bg GAHM Holland B parameter
 * @return Solution to first derivative
 */
double GahmRadiusSolverPrivate::f_prime(double rmax, double vmax,
                                        double isotach_radius, double fc,
                                        double bg) {
  return Gahm::Solver::GahmEquations::GahmFunctionDerivative(
      rmax, vmax, isotach_radius, fc, bg);
}
}  // namespace Gahm::Solver