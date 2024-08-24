//
// Created by Zach Cobell on 7/30/24.
//

#ifndef GAHM_ISOTACH_HPP
#define GAHM_ISOTACH_HPP

#include <cassert>
#include <ostream>

#include "datatypes/Point.hpp"
#include "datatypes/Vec.hpp"
#include "gahm/GahmParameters.hpp"
#include "storm/StormTranslation.hpp"

namespace Gahm::Storm {

class Isotach {
 public:
  constexpr Isotach() : m_wind_speed(0.0), m_distance_to_isotach(0.0) {}

  constexpr explicit Isotach(double wind_speed, double radius)
      : m_wind_speed(wind_speed), m_distance_to_isotach(radius) {}

  constexpr explicit Isotach(double wind_speed, double radius,
                             Gahm::Solver::GahmParamPack params)
      : m_wind_speed(wind_speed),
        m_distance_to_isotach(radius),
        m_gahm_params(params) {}

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

  [[nodiscard]] constexpr const Gahm::Solver::GahmParamPack &gahm_parameters()
      const {
    return m_gahm_params;
  }

  void set_radius(double radius) { m_distance_to_isotach = radius; }

  void set_gahm_parameters(const Gahm::Solver::GahmParamPack &params) {
    m_gahm_params = params;
  }

  void compute_gahm_parameters(const Gahm::Storm::StormTranslation &translation,
                               const Gahm::Types::Point &eye_location,
                               const Gahm::Types::Vec &unit_vector,
                               double central_pressure,
                               double background_pressure, double v_max);

  [[nodiscard]] static constexpr auto interpolate(const Isotach &isotach_1,
                                                  const Isotach &isotach_2,
                                                  double weight) -> Isotach {
    assert(isotach_1.wind_speed() == isotach_2.wind_speed());
    const auto this_radius = Util::Interpolation::linear(
        isotach_1.radius(), isotach_2.radius(), weight);
    return Isotach(isotach_1.wind_speed(), this_radius);
  }

 private:
  double m_wind_speed;
  double m_distance_to_isotach;
  Gahm::Solver::GahmParamPack m_gahm_params;
};

}  // namespace Gahm::Storm

#ifndef SWIG
auto operator<<(std::ostream &stream,
                const Gahm::Storm::Isotach &isotach) -> std::ostream &;
#endif

#endif  // GAHM_ISOTACH_HPP
