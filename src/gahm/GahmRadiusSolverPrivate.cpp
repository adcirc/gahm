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
#include "gahm/GahmRadiusSolverPrivate.h"

#include <utility>

#include "gahm/GahmEquations.h"

namespace Gahm::Solver {

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
 * @return std::pair containing the solution to Vg and Vg'
 */
std::pair<double, double> GahmRadiusSolverPrivate::operator()(
    const double &rmax) const {
  const double vg = GahmRadiusSolverPrivate::f(rmax, m_vmax, m_isotachSpeed,
                                               m_isotachRadius, m_f_coriolis, m_gahm_b);
  const double vgp = GahmRadiusSolverPrivate::f_prime(
      rmax, m_vmax, m_isotachRadius, m_f_coriolis, m_gahm_b);
  return {vg, vgp};
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
double GahmRadiusSolverPrivate::gahm_b() const { return m_gahm_b; }

/**
 * Solves the gahm gradient wind function
 * @param rmax radius to max winds
 * @param vmax maximum wind speed
 * @param isotach_speed speed of the current isotach
 * @param isotach_radius radius of the current isotach
 * @param f_coriolis coriolis force
 * @param gahm_b GAHM Holland B
 * @return Solution to gradient wind
 */
auto GahmRadiusSolverPrivate::f(double rmax, double vmax, double isotach_speed,
                                double isotach_radius, double f_coriolis, double gahm_b)
    -> double {
  return Gahm::Solver::GahmEquations::GahmFunction(rmax, vmax, isotach_speed,
                                                   isotach_radius, f_coriolis, gahm_b);
}

/**
 * Function to computeRadiusToMaxWind first derivative of gahm function
 * @param rmax Radius to max winds
 * @param vmax Maximum wind speed
 * @param isotach_radius radius of the current isotach
 * @param f_coriolis coriolis force
 * @param gahm_b GAHM Holland B parameter
 * @return Solution to first derivative
 */
auto GahmRadiusSolverPrivate::f_prime(double rmax, double vmax,
                                      double isotach_radius, double f_coriolis,
                                      double gahm_b) -> double {
  return Gahm::Solver::GahmEquations::GahmFunctionDerivative(
      rmax, vmax, isotach_radius, f_coriolis, gahm_b);
}
}  // namespace Gahm::Solver