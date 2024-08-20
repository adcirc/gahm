//
// Created by Zach Cobell on 7/30/24.
//

#ifndef GAHM_ISOTACH_H
#define GAHM_ISOTACH_H

#include <cassert>
#include <ostream>

#include "datatypes/Point.h"
#include "datatypes/Vec.h"
#include "gahm/GahmParameters.h"
#include "storm/StormTranslation.h"

namespace Gahm::Storm {

class Isotach {
 public:
  constexpr Isotach() : m_wind_speed(0.0), m_distance_to_isotach(0.0) {}

  constexpr explicit Isotach(double wind_speed, double radius)
      : m_wind_speed(wind_speed), m_distance_to_isotach(radius) {}

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

  [[nodiscard]] constexpr auto gahm_parameters() const
      -> const Gahm::Solver::GahmParamPack & {
    return m_gahm_params;
  }

  void compute_gahm_parameters(const StormTranslation &translation,
                               const Types::Point &eye_location,
                               const Types::Vec &unit_vector,
                               double central_pressure,
                               double background_pressure, double v_max);

 private:
  double m_wind_speed;
  double m_distance_to_isotach;
  Gahm::Solver::GahmParamPack m_gahm_params;
};

}  // namespace Gahm::Storm

auto operator<<(std::ostream &stream,
                const Gahm::Storm::Isotach &isotach) -> std::ostream &;

#endif  // GAHM_ISOTACH_H
