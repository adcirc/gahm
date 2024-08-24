//
// Created by Zach Cobell on 8/12/24.
//

#ifndef GAHM_GAHMSOLVER_HPP
#define GAHM_GAHMSOLVER_HPP

#include <cstddef>

#include "gahm/GahmRadiusSolver.hpp"

namespace Gahm::Solver {

class GahmSolver {
 public:
  GahmSolver(double isotach_radius, double isotach_speed, double vmax,
             double dp, double latitude);

  void solve();

  [[nodiscard]] constexpr auto isotachRadius() const -> double {
        return m_isotachRadius;
  }

  [[nodiscard]] constexpr auto isotachSpeed() const -> double {
        return m_isotachSpeed;
  }

  [[nodiscard]] constexpr auto latitude() const -> double {
        return m_latitude;
  }

  [[nodiscard]] constexpr auto dp() const -> double {
      return m_dp;
  }

  [[nodiscard]] constexpr auto f_coriolis() const -> double {
    return m_fc;
  }

  [[nodiscard]] constexpr auto vmax() const -> double {
        return m_vmax;
  }

  [[nodiscard]] constexpr auto rmax() const -> double {
    return m_rmax;
  }

  [[nodiscard]] constexpr auto gahm_b() const -> double {
    return m_bg;
  }

  [[nodiscard]] constexpr auto phi() const -> double {
    return m_phi;
  }

  [[nodiscard]] constexpr auto it() const -> size_t {
    return m_it;
  }

 private:
  static auto estimateRmax(double dp, double lat, double isorad) -> double;

  double m_isotachRadius;
  double m_isotachSpeed;
  double m_vmax;
  double m_dp;
  double m_latitude;
  double m_fc;
  double m_rmax_guess;
  double m_rmax;
  double m_bg;
  double m_bg_tol;
  double m_phi;
  size_t m_max_it;
  size_t m_it;
  GahmRadiusSolver m_solver;
};
}  // namespace Gahm::Solver

#endif  // GAHM_GAHMSOLVER_HPP
