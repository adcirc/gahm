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
#ifndef GAHM_UNITS_H
#define GAHM_UNITS_H

#include <cmath>
#include <type_traits>

namespace Gahm::Physical::Units {

enum AngleNormalizationType { ZERO_TO_360, NEG_180_TO_180 };

template <typename T, AngleNormalizationType AngleType>
constexpr auto normalizeAngle(const T &angle) noexcept -> T {
  if (AngleType == ZERO_TO_360) {
    if (angle < 0.0) {
      return angle + Physical::Constants::twoPi();
    } else if (angle > Physical::Constants::twoPi()) {
      return angle - Physical::Constants::twoPi();
    } else {
      return angle;
    }
  } else {
    if (angle < -Physical::Constants::pi()) {
      return angle + Physical::Constants::twoPi();
    } else if (angle > Physical::Constants::pi()) {
      return angle - Physical::Constants::twoPi();
    } else {
      return angle;
    }
  }
}

class UnitInternal {
 public:
  class UnitType {
   protected:
    constexpr explicit UnitType(const double v) : m_value(v) {}

   public:
    constexpr auto operator()() const { return m_value; }

   private:
    double m_value;
  };

  class Length : public UnitType {
   public:
    constexpr explicit Length(const double value) : UnitType(value) {}
  };

  class Speed : public UnitType {
   public:
    constexpr explicit Speed(const double value) : UnitType(value) {}
  };

  class Pressure : public UnitType {
   public:
    constexpr explicit Pressure(const double value) : UnitType(value) {}
  };

  class Angle : public UnitType {
   public:
    constexpr explicit Angle(const double value) : UnitType(value) {}
  };
};

constexpr UnitInternal::Length Meter(1.0);
constexpr UnitInternal::Length Kilometer(1.0 / 1000.0);
constexpr UnitInternal::Length Foot(3.28084);
constexpr UnitInternal::Length Mile(1.0 / 1609.34);
constexpr UnitInternal::Length NauticalMile(1.0 / 1851.995396854);

constexpr UnitInternal::Angle Radian(1.0);
constexpr UnitInternal::Angle Degree(180.0 / M_PI);

constexpr UnitInternal::Speed MetersPerSecond(1.0);
constexpr UnitInternal::Speed MilesPerHour(2.23694);
constexpr UnitInternal::Speed Knot(1.94384);
constexpr UnitInternal::Speed KilometersPerHour(3.6);

constexpr UnitInternal::Pressure Millibar(1000.0);
constexpr UnitInternal::Pressure Pascal(100000.0);
constexpr UnitInternal::Pressure Bar(1.0);
constexpr UnitInternal::Pressure MetersH20(10.197442889221);

template <
    typename T,
    std::enable_if_t<std::is_base_of_v<UnitInternal::UnitType, T>, bool> = true>
constexpr auto convert(const T &lhs, const T &rhs) -> double {
  return rhs() / lhs();
}

};  // namespace Gahm::Physical::Units

#endif  // GAHM_UNITS_H
