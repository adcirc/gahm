//
// Created by Zach Cobell on 6/30/22.
//

#ifndef GAHM2_SRC_VORTEXSOLUTION_H_
#define GAHM2_SRC_VORTEXSOLUTION_H_

#include <vector>

#include "Date.h"
#include "Vortex.h"
#include "WindData.h"

namespace Gahm {
class VortexSolution {
 public:
  VortexSolution(const Gahm::Vortex *vortex, std::vector<double> x_points,
                 std::vector<double> y_points);

  Gahm::WindData get(const Gahm::Date &d);

 private:
  using cycle_iterator = std::vector<Gahm::ForecastPeriod>::const_iterator;

  struct CycleWeight {
    cycle_iterator this_cycle;
    cycle_iterator next_cycle;
    double weight;
  };

  struct CycleParameters {
    CycleWeight cycle_weight;
    double central_pressure;
    double background_pressure;
    double max_wind_speed;
    StormPosition storm_position;
  };

  struct SolverInput {
    double vmax_at_boundary_layer;
    double radius_to_max_winds;
    double radius_to_max_winds_true;
    double holland_b;
    double isotach_speed;
    double isotach_radius;
  };

  [[nodiscard]] Gahm::VortexSolution::CycleWeight getCycle(
      const Gahm::Date &d) const;

  static CycleParameters interpolate_storm_parameters(const CycleWeight &cycle);
  void compute_distance_and_azimuth(const VortexSolution::CycleParameters &c);

  [[nodiscard]] SolverInput generate_solver_input_at_location(
      const CycleParameters &cycle, size_t query_index,
      const Gahm::PositionWeights &prior_weights,
      const Gahm::PositionWeights &next_weights) const;

  [[nodiscard]] SolverInput generate_solver_input_for_cycle(
      const cycle_iterator &cycle, size_t query_index,
      const Gahm::PositionWeights &weights) const;

  [[nodiscard]] static SolverInput interpolate_solver_parameters(
      const cycle_iterator &cycle, const Gahm::PositionWeights &weights,
      const Gahm::StormQuadrant::quadrant_it<4> &it1,
      const Gahm::StormQuadrant::quadrant_it<4> &it2);

  static std::tuple<int, double> get_base_quadrant(double angle);

  static std::tuple<double, double> rotate_winds(double u, double v,
                                                 double friction_angle,
                                                 double latitude);

  const Gahm::Vortex *m_vortex;
  std::vector<double> m_xpoints;
  std::vector<double> m_ypoints;
  std::vector<double> m_distance;
  std::vector<double> m_azimuth;
};
}  // namespace Gahm
#endif  // GAHM2_SRC_VORTEXSOLUTION_H_
