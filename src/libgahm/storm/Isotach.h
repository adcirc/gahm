//
// Created by Zach Cobell on 7/30/24.
//

#ifndef GAHM_ISOTACH_H
#define GAHM_ISOTACH_H

#include <cassert>
#include <ostream>

#include "datatypes/Point.h"
#include "datatypes/Vec.h"
#include "storm/StormTranslation.h"

namespace Gahm::Storm {

class Isotach {
 public:
  constexpr Isotach()
      : m_wind_speed(0.0),
        m_distance_to_isotach(0.0),
        m_radius_to_max_winds(0.0),
        m_holland_b(0.0),
        m_gahm_b(0.0),
        m_gahm_phi(0.0) {}

  constexpr explicit Isotach(double wind_speed, double radius)
      : m_wind_speed(wind_speed),
        m_distance_to_isotach(radius),
        m_radius_to_max_winds(0.0),
        m_holland_b(0.0),
        m_gahm_b(0.0),
        m_gahm_phi(0.0) {}

  [[nodiscard]] constexpr auto wind_speed() const -> double {
    return m_wind_speed;
  }

  [[nodiscard]] constexpr auto radius() const -> double {
    return m_distance_to_isotach;
  }

  [[nodiscard]] constexpr auto is_populated() const -> bool {
    return m_distance_to_isotach != 0.0;
  }

  [[nodiscard]] constexpr auto operator==(const Isotach &rhs) const -> bool {
    return m_wind_speed == rhs.m_wind_speed;
  }

  [[nodiscard]] constexpr auto radius_to_max_winds() const -> double {
    return m_radius_to_max_winds;
  }

  [[nodiscard]] constexpr auto gahm_b() const -> double { return m_gahm_b; }

  [[nodiscard]] constexpr auto gahm_phi() const -> double { return m_gahm_phi; }

  [[nodiscard]] constexpr auto holland_b() const -> double {
    return m_holland_b;
  }

  void compute_gahm_parameters(const StormTranslation &translation,
                               const Types::Point &eye_location,
                               const Types::Vec &unit_vector,
                               double central_pressure,
                               double background_pressure, double v_max);

 private:
  double m_wind_speed;
  double m_distance_to_isotach;
  double m_radius_to_max_winds;
  double m_holland_b;
  double m_gahm_b;
  double m_gahm_phi;
};

}  // namespace Gahm::Storm

auto operator<<(std::ostream &stream,
                const Gahm::Storm::Isotach &isotach) -> std::ostream &;

#endif  // GAHM_ISOTACH_H
