//
// Created by Zach Cobell on 7/30/24.
//

#ifndef GAHM_ATCFPERIOD_H
#define GAHM_ATCFPERIOD_H

#include <array>
#include <cassert>
#include <ostream>

#include "datatypes/Datetime.h"
#include "datatypes/Point.h"
#include "storm/Quadrant.h"
#include "storm/StormTranslation.h"

namespace Gahm::Atcf {

class AtcfPeriod {
 public:
  AtcfPeriod()
      : m_central_pressure(0),
        m_background_pressure(0),
        m_v_max(0),
        m_r_max(0),
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
        m_quadrants(quadrants) {
  }

  [[nodiscard]] auto central_pressure() const -> double {
    return m_central_pressure;
  }
  [[nodiscard]] auto background_pressure() const -> double {
    return m_background_pressure;
  }
  [[nodiscard]] auto eye_location() const -> Types::Point {
    return m_eye_location;
  }

  [[nodiscard]] auto datetime() const -> Types::Datetime { return m_datetime; }

  [[nodiscard]] auto v_max() const -> double { return m_v_max; }

  [[nodiscard]] auto r_max() const -> double { return m_r_max; }

  [[nodiscard]] auto quadrants() const
      -> const std::array<Gahm::Storm::Quadrant, 4> & {
    return m_quadrants;
  }

  [[nodiscard]] auto quadrant(size_t index) -> Gahm::Storm::Quadrant & {
    assert(index < 4);
    return m_quadrants.at(index);
  }

  [[nodiscard]] auto quadrant(size_t index) const
      -> const Gahm::Storm::Quadrant & {
    assert(index < 4);
    return m_quadrants.at(index);
  }

  [[nodiscard]] auto translation() const -> const Storm::StormTranslation & {
    return m_translation;
  }

  void set_translation(const Storm::StormTranslation &translation) {
    m_translation = translation;
  }

  void compute_gahm_parameters();

 private:
  Types::Datetime m_datetime;    // Datetime of the period
  double m_central_pressure;     // Central pressure in millibars
  double m_background_pressure;  // Background pressure in millibars
  double m_v_max;                // Maximum wind speed
  double m_r_max;                // Maximum radius
  Types::Point m_eye_location;   // Eye location in latitude and longitude
  Storm::StormTranslation m_translation;  // Storm translation
  std::array<Gahm::Storm::Quadrant, 4>
      m_quadrants;  // Quadrant data for the period
};

}  // namespace Gahm::Atcf

auto operator<<(std::ostream &stream,
                const Gahm::Atcf::AtcfPeriod &period) -> std::ostream &;

#endif  // GAHM_ATCFPERIOD_H
