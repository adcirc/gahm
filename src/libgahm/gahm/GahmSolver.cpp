//
// Created by Zach Cobell on 8/12/24.
//

#include "GahmSolver.hpp"

#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstddef>
#include <limits>
#include <string>

#include "boost/math/policies/error_handling.hpp"
#include "gahm/GahmEquations.hpp"
#include "physical/Atmospheric.hpp"
#include "physical/Earth.hpp"

namespace Gahm::Solver {

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
                       double dp, double latitude)
    : m_isotachRadius(isotach_radius),
      m_isotachSpeed(isotach_speed),
      m_vmax(vmax),
      m_dp(dp),
      m_latitude(latitude),
      m_fc(Gahm::Physical::Earth::coriolis(m_latitude)),
      m_rmax_guess(GahmSolver::estimateRmax(m_dp, latitude, isotach_radius)),
      m_rmax(std::numeric_limits<double>::max()),
      m_bg(Gahm::Physical::Atmospheric::holland_b(m_vmax, m_dp)),
      m_bg_tol(1e-9),
      m_phi(1.0),
      m_max_it(200),
      m_it(0),
      m_solver(m_isotachRadius, m_isotachSpeed, m_vmax, m_fc, m_bg) {}

/**
 * Runs the solver and stores the solution internally
 */
void GahmSolver::solve() {
  const auto guess = m_rmax_guess;
  for (size_t i = 0; i < m_max_it; ++i) {
    auto new_rmax = m_solver.solve(0.1, m_isotachRadius, guess);
    if (!std::isnan(new_rmax) && !std::isinf(new_rmax) &&
        new_rmax != std::numeric_limits<double>::max()) {
      m_rmax = new_rmax;
    }
    assert(new_rmax > 0.0);
    m_phi = GahmEquations::phi(m_vmax, m_rmax, m_bg, m_fc);
    m_bg = GahmEquations::gahm_b(m_vmax, m_rmax, m_dp, m_fc, m_phi);
    if (std::abs(m_bg - m_solver.gahm_b()) < m_bg_tol) {
      m_it = i;
      break;
    }
    if (std::isnan(new_rmax) || std::isinf(new_rmax) || std::isnan(m_bg) ||
        std::isinf(m_bg) || std::isnan(m_phi) || std::isinf(m_phi)) {
      throw boost::math::evaluation_error(
          std::string("Solution did not converge."));
    }
    m_solver.setGahmB(m_bg);
  }
  assert(this->rmax() > 0.0);
  assert(this->gahm_b() > 0.0);
}

/**
 * Estimates the rmax. Used as the initial guess for the solver
 * @param dp pressure deficit in Pascals
 * @param lat latitude of the storm center
 * @param isorad isotach radius that we are solving for (upper bound)
 * @return estimate of rmax
 */
auto GahmSolver::estimateRmax(const double dp, const double lat,
                              const double isorad) -> double {
  assert(dp >= 0.0);
  assert(isorad > 0.0);
  auto r1 =
      std::exp(3.015 - 6.291e-5 * std::pow(dp / 100.0, 2.0) + 0.337 * lat);
  auto r2 = 0.99 * isorad;
  return std::min(r1, r2);
}
}  // namespace Gahm::Solver
