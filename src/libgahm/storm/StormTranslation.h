//
// Created by Zach Cobell on 7/31/24.
//

#ifndef GAHM_STORMTRANSLATION_H
#define GAHM_STORMTRANSLATION_H

#include <array>
#include <cmath>
#include <ostream>
#include <tuple>

#include "datatypes/Point.h"
#include "datatypes/Vec.h"
#include "physical/Constants.h"
#include "physical/Earth.h"
#include "physical/Units.h"

namespace Gahm::Storm {

class StormTranslation {
 public:
  constexpr StormTranslation()
      : m_translation_speed(0.0),
        m_translation_direction(0.0),
        m_unit_vector(Types::Vec{0.0, 0.0}),
        m_translation_vector(Types::Vec{0.0, 0.0}) {}
  StormTranslation(double translation_speed, double translation_direction)
      : m_translation_speed(translation_speed),
        m_translation_direction(translation_direction),
        m_unit_vector(
            Types::Vec{m_translation_speed * std::cos(translation_direction),
                       m_translation_speed * std::sin(translation_direction)}),
        m_translation_vector(
            Types::Vec{m_unit_vector.u() * m_translation_speed,
                       m_unit_vector.v() * m_translation_speed}) {};

  StormTranslation(Types::Point point_0, Types::Point point_1,
                   double time_delta)
      : m_translation_speed(1.5 *
                            std::pow(std::abs(Gahm::Physical::Earth::distance(
                                                  point_0, point_1) /
                                              time_delta),
                                     0.63)),
        m_translation_direction(
            std::fmod(Gahm::Physical::Constants::twoPi() -
                          Gahm::Physical::Earth::azimuth(point_0, point_1) +
                          Gahm::Physical::Constants::halfPi(),
                      Gahm::Physical::Constants::twoPi())),
        m_unit_vector(Types::Vec{std::cos(m_translation_direction),
                                 std::sin(m_translation_direction)}),
        m_translation_vector(
            Types::Vec{m_unit_vector.u() * m_translation_speed,
                       m_unit_vector.v() * m_translation_speed}) {}

  StormTranslation(Types::Point point_0, Types::Point point_1, long time_delta)
      : StormTranslation(point_0, point_1, static_cast<double>(time_delta)) {}

  [[nodiscard]] constexpr auto speed() const -> double {
    return m_translation_speed;
  }

  [[nodiscard]] constexpr auto direction() const -> double {
    return m_translation_direction;
  }

  [[nodiscard]] constexpr auto velocity() const -> Types::Vec {
    return m_translation_vector;
  }

  [[nodiscard]] constexpr auto unit_vector() const -> Types::Vec {
    return m_unit_vector;
  }

  [[nodiscard]] constexpr auto operator==(const StormTranslation &rhs) const
      -> bool {
    return m_translation_speed == rhs.m_translation_speed &&
           m_translation_direction == rhs.m_translation_direction &&
           m_unit_vector == rhs.m_unit_vector &&
           m_translation_vector == rhs.m_translation_vector;
  }

 private:
  double m_translation_speed;
  double m_translation_direction;
  Types::Vec m_unit_vector;
  Types::Vec m_translation_vector;
};

}  // namespace Gahm::Storm

auto operator<<(std::ostream &stream,
                const Gahm::Storm::StormTranslation &translation)
    -> std::ostream &;

#endif  // GAHM_STORMTRANSLATION_H
