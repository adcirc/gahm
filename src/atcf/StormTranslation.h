// GNU General Public License v3.0
//
// This file is part of the GAHM model (https://github.com/adcirc/gahm).
// Copyright (c) 2023 ADCIRC Development Group.
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, version 3.
//
// This program is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program. If not, see <http://www.gnu.org/licenses/>.
//
// Author: Zach Cobell
// Contact: zcobell@thewaterinstitute.org
//
#ifndef GAHM_SRC_ATCF_STORMTRANSLATION_H_
#define GAHM_SRC_ATCF_STORMTRANSLATION_H_

#include <cmath>

#include "physical/Constants.h"

namespace Gahm::Atcf {

class StormTranslation {
 public:
  StormTranslation() = default;

  StormTranslation(double translation_speed, double translation_direction)
      : m_raw_translation_speed(translation_speed),
        m_raw_translation_direction(translation_direction),
        m_transit_speed(0.0),
        m_transit_speed_u(0.0),
        m_transit_speed_v(0.0) {
    this->computeDefaultTranslationComponents();
  }

  [[nodiscard]] double translationSpeed() const {
    return m_raw_translation_speed;
  }

  void setTranslationSpeed(double translation_speed) {
    m_raw_translation_speed = translation_speed;
  }

  [[nodiscard]] double translationDirection() const {
    return m_raw_translation_direction;
  }

  void setRawTranslationDirection(double translation_direction) {
    m_raw_translation_direction = translation_direction;
  }

  [[nodiscard]] double transitSpeed() const { return m_transit_speed; }

  [[nodiscard]] double transitSpeedU() const { return m_transit_speed_u; }

  [[nodiscard]] double transitSpeedV() const { return m_transit_speed_v; }

  void setRelativeTransitComponents(const double transitSpeed,
                                    const double transitSpeedU,
                                    const double transitSpeedV) {
    m_transit_speed = transitSpeed;
    m_transit_speed_u = transitSpeedU;
    m_transit_speed_v = transitSpeedV;
  }

  void computeDefaultTranslationComponents() {
    m_transit_speed_u = m_raw_translation_speed *
                        std::cos(m_raw_translation_direction *
                                 Gahm::Physical::Constants::deg2rad());
    m_transit_speed_v = m_raw_translation_direction *
                        std::sin(m_raw_translation_direction *
                                 Gahm::Physical::Constants::deg2rad());
    m_transit_speed = m_raw_translation_speed;
  }

 private:
  double m_raw_translation_speed;
  double m_raw_translation_direction;
  double m_transit_speed;
  double m_transit_speed_u;
  double m_transit_speed_v;
};

}  // namespace Gahm::Atcf

#endif  // GAHM_SRC_ATCF_STORMTRANSLATION_H_
