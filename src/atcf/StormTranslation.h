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
#include <tuple>

#include "physical/Constants.h"
#include "util/Interpolation.h"

#ifdef SWIG
#define NODISCARD
#else
#define NODISCARD [[nodiscard]]
#endif

namespace Gahm::Atcf {

class StormTranslation {
 public:
  StormTranslation() = default;

  StormTranslation(double translation_speed, double translation_direction)
      : m_translation_speed(translation_speed),
        m_translation_direction(translation_direction),
        m_translation_speed_u(m_translation_speed *
                              std::sin(m_translation_direction)),
        m_translation_speed_v(m_translation_speed *
                              std::cos(m_translation_direction)) {}

  NODISCARD double translationSpeed() const { return m_translation_speed; }

  void setTranslationSpeed(double translation_speed) {
    m_translation_speed = translation_speed;
  }

  NODISCARD double translationDirection() const {
    return m_translation_direction;
  }

  NODISCARD std::tuple<double, double> translationComponents() const {
    return {m_translation_speed_u, m_translation_speed_v};
  }

  NODISCARD double transitSpeed() const { return m_translation_speed; }

  static StormTranslation interpolate(const StormTranslation &a,
                                      const StormTranslation &b,
                                      const double &weight) {
    return {Interpolation::linear(a.translationSpeed(), b.translationSpeed(),
                                  weight),
            Interpolation::angle(a.translationDirection(),
                                 b.translationDirection(), weight)};
  }

 private:
  double m_translation_speed;
  double m_translation_direction;
  double m_translation_speed_u;
  double m_translation_speed_v;
};

}  // namespace Gahm::Atcf

#endif  // GAHM_SRC_ATCF_STORMTRANSLATION_H_
