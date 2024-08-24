//
// Created by Zach Cobell on 8/12/24.
//

#ifndef GAHM_GAHMRADIUSSOLVER_HPP
#define GAHM_GAHMRADIUSSOLVER_HPP

#include "GahmRadiusSolverPrivate.hpp"

namespace Gahm::Solver {

class GahmRadiusSolver {
 public:
  GahmRadiusSolver(double isotach_radius, double isotach_speed, double vmax,
                   double f_coriolis, double gahm_b);

  [[nodiscard]] auto solve(double lower, double upper,
                           double guess) const -> double;

  void setGahmB(double gahm_b);

  [[nodiscard]] auto gahm_b() const -> double;

  [[nodiscard]] auto v_max() const -> double { return m_solver.v_max(); }

  [[nodiscard]] auto coriolis() const -> double { return m_solver.coriolis(); }

  [[nodiscard]] auto isotach_speed() const -> double {
    return m_solver.isotach_speed();
  }

  [[nodiscard]] auto isotach_radius() const -> double {
    return m_solver.isotach_radius();
  }

 private:
  detail::GahmRadiusSolverPrivate m_solver;
  size_t m_max_it;
};

}  // namespace Gahm::Solver
#endif  // GAHM_GAHMRADIUSSOLVER_HPP
