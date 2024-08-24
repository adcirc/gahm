//
// Created by Zach Cobell on 7/30/24.
//

#ifndef GAHM_ATCFPERIOD_HPP
#define GAHM_ATCFPERIOD_HPP

#include <array>
#include <cassert>
#include <cstddef>
#include <optional>
#include <ostream>

#include "datatypes/Datetime.hpp"
#include "datatypes/Point.hpp"
#include "physical/Earth.hpp"
#include "storm/Quadrant.hpp"
#include "storm/StormTranslation.hpp"

namespace Gahm::Atcf {

class AtcfPeriod {
 public:
  AtcfPeriod()
      : m_central_pressure(0),
        m_background_pressure(0),
        m_v_max(0),
        m_r_max(0),
        m_eye_location({0, 0}),
        m_coriolis_force(0),
        m_quadrants() {}

  AtcfPeriod(Gahm::Types::Datetime datetime, double central_pressure,
             double background_pressure, double v_max, double r_max,
             const Types::Point &eye_location,
             std::array<Gahm::Storm::Quadrant, 4> quadrants)
      : m_datetime(datetime),
        m_central_pressure(central_pressure),
        m_background_pressure(background_pressure),
        m_v_max(v_max),
        m_r_max(r_max),
        m_eye_location(eye_location),
        m_coriolis_force(Gahm::Physical::Earth::coriolis(eye_location.y())),
        m_quadrants(quadrants) {}

  AtcfPeriod(Gahm::Types::Datetime datetime, double central_pressure,
             double background_pressure, double v_max, double r_max,
             const Gahm::Types::Point &eye_location,
             const Gahm::Storm::StormTranslation &translation,
             std::array<Gahm::Storm::Quadrant, 4> quadrants,
             bool compute_gahm_parameters = true)
      : m_datetime(datetime),
        m_central_pressure(central_pressure),
        m_background_pressure(background_pressure),
        m_v_max(v_max),
        m_r_max(r_max),
        m_eye_location(eye_location),
        m_coriolis_force(Gahm::Physical::Earth::coriolis(eye_location.y())),
        m_translation(translation),
        m_quadrants(quadrants) {
    if (compute_gahm_parameters) {
      this->compute_gahm_parameters();
    }
  }

  [[nodiscard]] auto central_pressure() const -> double {
    return m_central_pressure;
  }
  [[nodiscard]] auto background_pressure() const -> double {
    return m_background_pressure;
  }
  [[nodiscard]] auto eye_location() const -> Gahm::Types::Point {
    return m_eye_location;
  }

  [[nodiscard]] auto datetime() const -> Gahm::Types::Datetime {
    return m_datetime;
  }

  [[nodiscard]] auto v_max() const -> double { return m_v_max; }

  [[nodiscard]] auto r_max() const -> double { return m_r_max; }

  [[nodiscard]] const std::array<Gahm::Storm::Quadrant, 4> &quadrants() const {
    return m_quadrants;
  }

  [[nodiscard]] auto quadrant(size_t index) -> Gahm::Storm::Quadrant & {
    assert(index < 4);
    return m_quadrants.at(index);
  }

#ifndef SWIG
  [[nodiscard]] const Gahm::Storm::Quadrant &quadrant(size_t index) const {
    assert(index < 4);
    return m_quadrants.at(index);
  }
#endif

  [[nodiscard]] const Gahm::Storm::StormTranslation &translation() const {
    return m_translation;
  }

  [[nodiscard]] auto coriolis_force() const -> double {
    return m_coriolis_force;
  }

  void set_translation(const Gahm::Storm::StormTranslation &translation) {
    m_translation = translation;
  }

  void compute_gahm_parameters();

#ifndef SWIG
  [[nodiscard]] static auto is_interpolatable(
      const Gahm::Atcf::AtcfPeriod &period_1,
      const Gahm::Atcf::AtcfPeriod &period_2) -> bool;

  [[nodiscard]] static auto interpolate(
      const AtcfPeriod &period_1, const Gahm::Atcf::AtcfPeriod &period_2,
      const Types::Datetime &datetime) -> std::optional<Gahm::Atcf::AtcfPeriod>;
#endif

 private:
  Types::Datetime m_datetime;    // Datetime of the period
  double m_central_pressure;     // Central pressure in millibars
  double m_background_pressure;  // Background pressure in millibars
  double m_v_max;                // Maximum wind speed
  double m_r_max;                // Maximum radius
  Types::Point m_eye_location;   // Eye location in latitude and longitude
  double m_coriolis_force;       // Coriolis force
  Storm::StormTranslation m_translation;  // Storm translation
  std::array<Gahm::Storm::Quadrant, 4>
      m_quadrants;  // Quadrant data for the period
};

}  // namespace Gahm::Atcf

#ifndef SWIG
auto operator<<(std::ostream &stream,
                const Gahm::Atcf::AtcfPeriod &period) -> std::ostream &;
#endif

#endif  // GAHM_ATCFPERIOD_HPP
