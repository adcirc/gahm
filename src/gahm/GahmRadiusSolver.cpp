//
// Created by Zach Cobell on 8/12/24.
//

#include "gahm/GahmRadiusSolver.h"

#include <cstddef>
#include <limits>
#include <string>

#include "boost/math/policies/error_handling.hpp"
#include "boost/math/tools/roots.hpp"
#include "boost/throw_exception.hpp"

constexpr size_t c_max_solver_iterations = 200;

namespace Gahm::Solver {

/**
 * Constructor for the Radius solver object
 * @param isotach_radius radius of the current isotach
 * @param isotach_speed speed of the current isotach
 * @param vmax maximum wind velocity
 * @param f_coriolis coriolis force
 * @param gahm_b GAHM Holland B parameter
 */
GahmRadiusSolver::GahmRadiusSolver(double isotach_radius, double isotach_speed,
                                   double vmax, double f_coriolis,
                                   double gahm_b)
    : m_solver(isotach_radius, isotach_speed, vmax, f_coriolis, gahm_b),
      m_max_it(c_max_solver_iterations) {}

/**
 * Runs the solver
 * @param lower lower bound for solution
 * @param upper upper bound for solution
 * @param guess guess for solution
 * @return radius to maximum winds
 */
auto GahmRadiusSolver::solve(double lower, double upper, double guess) const
    -> double {
  auto iter = m_max_it;
  try {
    return boost::math::tools::newton_raphson_iterate(
        m_solver, guess, lower, upper, std::numeric_limits<double>::digits,
        iter);
  } catch (const boost::wrapexcept<boost::math::evaluation_error> &e) {
    auto error =
        "Unable to solve for radius to maximum winds: " + std::string(e.what());
    throw boost::math::evaluation_error(error);
  }
}

/**
 * Allows user to change the GAHM Holland B parameter for additional solver
 * iterations
 * @param bg GAHM Holland B
 */
void GahmRadiusSolver::setGahmB(double gahm_b) { m_solver.setGahmB(gahm_b); }

/**
 * Returns the current GAHM Holland B parameter
 * @return GAHM Holland B
 */
auto GahmRadiusSolver::gahm_b() const -> double { return m_solver.gahm_b(); }
}  // namespace Gahm::Solver