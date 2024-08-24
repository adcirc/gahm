//
// Created by Zach Cobell on 8/12/24.
//

#ifndef GAHM_GAHMRADIUSSOLVERPRIVATE_HPP
#define GAHM_GAHMRADIUSSOLVERPRIVATE_HPP

#include <cassert>
#include <cmath>
#include <tuple>

namespace Gahm::Solver::detail {

class GahmRadiusSolverPrivate {
 public:
  GahmRadiusSolverPrivate(double isotachRadius, double isotachSpeed,
                          double vmax, double f_coriolis, double gahm_b);

  [[nodiscard]] auto operator()(double radius_to_max_winds) const
      -> std::tuple<double, double>;

  void setGahmB(double gahm_b);
  [[nodiscard]] auto gahm_b() const -> double;

  [[nodiscard]] auto isotach_radius() const -> double {
    return m_isotachRadius;
  }

  [[nodiscard]] auto v_max() const -> double { return m_vmax; }

  [[nodiscard]] auto coriolis() const -> double { return m_f_coriolis; }

  [[nodiscard]] auto isotach_speed() const -> double { return m_isotachSpeed; }

 private:
  static auto f(double radius_to_max_winds, double vmax, double isotach_speed,
                double isotach_radius, double f_coriolis,
                double gahm_b) -> double;
  static auto f_prime(double radius_to_max_winds, double vmax,
                      double isotach_radius, double f_coriolis,
                      double gahm_b) -> double;

  double m_isotachRadius;
  double m_vmax;
  double m_f_coriolis;
  double m_isotachSpeed;
  double m_gahm_b;
};

}  // namespace Gahm::Solver::detail

#endif  // GAHM_GAHMRADIUSSOLVERPRIVATE_HPP
