//
// Created by Zach Cobell on 7/30/24.
//

#ifndef GAHM_GAHMPARAMETERS_H
#define GAHM_GAHMPARAMETERS_H

#include "datatypes/Point.h"
#include "datatypes/Vec.h"
#include "storm/StormTranslation.h"

namespace Gahm {

class GahmParameters {
 public:
  explicit GahmParameters(const Gahm::Atcf::StormTranslation& translation,
                          const Gahm::Types::Point& eye_location,
                          const Gahm::Types::Vec& unit_vector,
                          double central_pressure, double background_pressure,
                          double v_max, double isotach_speed,
                          double isotach_radius);

  [[nodiscard]] constexpr auto fc() const -> double { return m_fc; }

  [[nodiscard]] constexpr auto v_max_10_10() const -> double {
    return m_v_max_10_10;
  }

  [[nodiscard]] constexpr auto dp() const -> double { return m_dp; }

  [[nodiscard]] constexpr auto v_max_10_10_nominal() const -> double {
    return m_v_max_10_10_nominal;
  }

  [[nodiscard]] constexpr auto v_max_10_10_theta() const -> double {
    return m_v_max_10_10_theta;
  }

  [[nodiscard]] constexpr auto v_isotach_10_10() const -> double {
    return m_v_isotach_10_10;
  }

  [[nodiscard]] constexpr auto radius_to_max_winds() const -> double {
    return m_rmax;
  }

  [[nodiscard]] constexpr auto gahm_b() const -> double { return m_gahm_b; }

  [[nodiscard]] constexpr auto gahm_phi() const -> double { return m_gahm_phi; }

 private:
  void compute_gahm_parameters();

  Atcf::StormTranslation m_translation;
  Types::Point m_eye_location;
  Types::Vec m_unit_vector;
  Types::Vec m_quadrant_unit_vector;
  double m_central_pressure;
  double m_background_pressure;
  double m_isotach_speed_1_10;
  double m_isotach_radius;
  double m_fc;
  double m_dp;
  double m_v_max_10_10;
  double m_v_max_10_10_nominal;
  double m_v_max_10_10_theta;
  double m_v_isotach_10_10;
  double m_v_vortex_max_10_10;
  double m_rmax;
  double m_gahm_b;
  double m_gahm_phi;
};

}  // namespace Gahm
#endif  // GAHM_GAHMPARAMETERS_H
