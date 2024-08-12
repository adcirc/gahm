//
// Created by Zach Cobell on 8/12/24.
//

#ifndef GAHM_GAHMRADIUSSOLVER_H
#define GAHM_GAHMRADIUSSOLVER_H

#include "GahmRadiusSolverPrivate.h"

namespace Gahm::Solver {

class GahmRadiusSolver {
 public:
  GahmRadiusSolver(double isotach_radius, double isotach_speed, double vmax,
                   double f_coriolis, double gahm_b);

  [[nodiscard]] auto solve(double lower, double upper,
                           double guess) const -> double;

  void setGahmB(double gahm_b);

  [[nodiscard]] auto gahm_b() const -> double;

 private:
  detail::GahmRadiusSolverPrivate m_solver;
  size_t m_max_it;
};

}  // namespace Gahm::Solver
#endif  // GAHM_GAHMRADIUSSOLVER_H
