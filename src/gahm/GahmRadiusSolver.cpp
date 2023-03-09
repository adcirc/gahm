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
#include "gahm/GahmRadiusSolver.h"

#include "boost/math/tools/roots.hpp"

namespace Gahm::Solver {

/**
 * Constructor for the Radius solver object
 * @param isotach_radius radius of the current isotach
 * @param isotach_speed speed of the current isotach
 * @param vmax maximum wind velocity
 * @param fc coriolis force
 * @param bg GAHM Holland B parameter
 */
GahmRadiusSolver::GahmRadiusSolver(double isotach_radius, double isotach_speed,
                                   double vmax, double fc, double bg)
    : m_solver(isotach_radius, isotach_speed, vmax, fc, bg), m_max_it(200) {}

/**
 * Runs the solver
 * @param lower lower bound for solution
 * @param upper upper bound for solution
 * @param guess guess for solution
 * @return radius to maximum winds
 */
double GahmRadiusSolver::solve(double lower, double upper, double guess) const {
  auto it = m_max_it;
  try {
    return boost::math::tools::newton_raphson_iterate(
        m_solver, guess, lower, upper, std::numeric_limits<double>::digits, it);
  } catch (const std::exception &e) {
    throw std::runtime_error("Unable to solve for radius to maximum winds: " +
                             std::string(e.what()));
  }
}

/**
 * Allows user to change the GAHM Holland B parameter for additional solver
 * iterations
 * @param bg GAHM Holland B
 */
void GahmRadiusSolver::setBg(double bg) { m_solver.setBg(bg); }

/**
 * Returns the current GAHM Holland B parameter
 * @return GAHM Holland B
 */
double GahmRadiusSolver::bg() const { return m_solver.bg(); }
}  // namespace Gahm::Solver