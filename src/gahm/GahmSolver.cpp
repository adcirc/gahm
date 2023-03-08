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
#include "GahmSolver.h"

#include <algorithm>
#include <cmath>
#include <stdexcept>

#include "gahm/GahmEquations.h"
#include "physical/Atmospheric.h"
#include "physical/Earth.h"

using namespace Gahm::Solver;

/**
 * Construct a GAHM solver object
 * @param isotach_radius Radius of the current isotach
 * @param isotach_speed Speed of the current isotach
 * @param vmax Maximum wind speed
 * @param p_center Minimum pressure for the storm
 * @param p_background Background atmospheric pressure
 * @param latitude latitude of storm center
 */
GahmSolver::GahmSolver(double isotach_radius, double isotach_speed, double vmax,
                       double p_center, double p_background, double latitude)
    : m_isotachRadius(isotach_radius),
      m_isotachSpeed(isotach_speed),
      m_vmax(vmax),
      m_pc(p_center),
      m_pbk(p_background),
      m_latitude(latitude),
      m_fc(Gahm::Physical::Earth::coriolis(m_latitude)),
      m_rmax_guess(
          GahmSolver::estimateRmax(m_pbk - m_pc, latitude, isotach_radius)),
      m_rmax(std::numeric_limits<double>::max()),
      m_bg(Gahm::Physical::Atmospheric::calcHollandB(m_vmax, m_pc, m_pbk)),
      m_max_it(100),
      m_it(0),
      m_phi(1.0),
      m_bg_tol(1e-6),
      m_solver(m_isotachRadius, m_isotachSpeed, m_vmax, m_fc, m_bg) {}

/**
 * Runs the solver and stores the solution internally
 */
void GahmSolver::solve() {
  const auto guess = m_rmax_guess;
  const auto dp = m_pbk - m_pc;
  for (auto i = 0; i < m_max_it; ++i) {
    auto new_rmax = m_solver.solve(1.0, m_isotachRadius, guess);
    if (new_rmax != NAN && new_rmax != INFINITY &&
        new_rmax != std::numeric_limits<double>::max()) {
      m_rmax = new_rmax;
    }
    m_phi = GahmEquations::phi(m_vmax, m_rmax, m_bg, m_fc);
    m_bg = GahmEquations::bg(m_vmax, m_rmax, m_pc, m_pbk, m_fc, m_phi);
    if (std::abs(m_bg - m_solver.bg()) < m_bg_tol) {
      m_it = i;
      break;
    }
    if (new_rmax == NAN || new_rmax == INFINITY || m_bg == NAN ||
        m_bg == INFINITY || m_phi == NAN || m_phi == INFINITY) {
      throw std::runtime_error("ERROR: Solution did not converge");
    }
    m_solver.setBg(m_bg);
  }
}

/**
 * Returns the isotach radius
 * @return isotach radius
 */
double GahmSolver::isotachRadius() const { return m_isotachRadius; }

/**
 * Returns the isotach speed
 * @return isotach speed
 */
double GahmSolver::isotachSpeed() const { return m_isotachSpeed; }

/**
 * Returns the storm latitude
 * @return storm latitude
 */
double GahmSolver::latitude() const { return m_latitude; }

/**
 * Returns the central pressure
 * @return central pressure
 */
double GahmSolver::pc() const { return m_pc; }

/**
 * Returns the background pressure
 * @return background pressure
 */
double GahmSolver::pbk() const { return m_pbk; }

/**
 * Returns the coriolis force
 * @return coriolis force
 */
double GahmSolver::fc() const { return m_fc; }

/**
 * Returns the maximum wind speed
 * @return maximum wind speed
 */
double GahmSolver::vmax() const { return m_vmax; }

/**
 * Returns the solution to the radius to maximum winds. Note that when the
 * solver has not been run, the solution is std::numeric_limits<double>::max()
 * @return radius to maximum winds
 */
double GahmSolver::rmax() const {
  if (m_it == 0) {
    throw std::runtime_error("GAHM Solver WARNING: Solver has not run");
  }
  return m_rmax;
}

/**
 * Returns the solution to the GAHM Holland B parameter. Note that when the
 * solver has not run, the solution is the standard Holland B
 * @return GAHM holland B
 */
double GahmSolver::bg() const {
  if (m_it == 0) {
    throw std::runtime_error("GAHM Solver WARNING: Solver has not run");
  }
  return m_bg;
}

/**
 * Returns the number of iterations used in the GAHM Holland B solver
 * @return
 */
size_t GahmSolver::it() const { return m_it; }

double GahmSolver::phi() const { return m_phi; }

/**
 * Estimates the rmax. Used as the initial guess for the solver
 * @param dp pressure deficit
 * @param lat latitude of the storm center
 * @param isorad isotach radius that we are solving for (upper bound)
 * @return estimate of rmax
 */
double GahmSolver::estimateRmax(const double dp, const double lat,
                                const double isorad) {
  assert(dp >= 0);
  assert(isorad > 0);
  auto r1 = std::exp(3.015 - 6.291e-5 * std::pow(dp, 2.0) + 0.337 * lat);
  auto r2 = 0.99 * isorad;
  return std::min(r1, r2);
}
