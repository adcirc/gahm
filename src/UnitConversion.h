// MIT License
//
// Copyright (c) 2020 ADCIRC Development Group
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//
// Author: Zach Cobell
// Contact: zcobell@thewaterinstitute.org
//
#ifndef UNITCONVERSION_H
#define UNITCONVERSION_H

#include <cmath>
#include <type_traits>

namespace Units {

class UnitInternal {
 public:
  class UnitType {
   protected:
    constexpr UnitType(const double v) : m_value(v) {}

   public:
    constexpr auto operator()() const { return m_value; }

   private:
    const double m_value;
  };

  class Length : public UnitType {
   public:
    constexpr Length(const double v) : UnitType(v) {}
  };

  class Speed : public UnitType {
   public:
    constexpr Speed(const double v) : UnitType(v) {}
  };

  class Pressure : public UnitType {
   public:
    constexpr Pressure(const double v) : UnitType(v) {}
  };

  class Angle : public UnitType {
   public:
    constexpr Angle(const double v) : UnitType(v) {}
  };
};

constexpr UnitInternal::Length Meter(1.0);
constexpr UnitInternal::Length Kilometer(0.001);
constexpr UnitInternal::Length Foot(3.28084);
constexpr UnitInternal::Length Mile(1.0/1609.34);
constexpr UnitInternal::Length NauticalMile(1.0/1851.995396854);

constexpr UnitInternal::Angle Radian(1.0);
constexpr UnitInternal::Angle Degree(180.0 / M_PI);

constexpr UnitInternal::Speed MetersPerSecond(1.0);
constexpr UnitInternal::Speed MilesPerHour(2.23694);
constexpr UnitInternal::Speed Knot(1.94384);
constexpr UnitInternal::Speed KilometersPerHour(0.277778);

constexpr UnitInternal::Pressure Millibar(1000.0);
constexpr UnitInternal::Pressure Pascal(100000.0);
constexpr UnitInternal::Pressure Bar(1.0);
constexpr UnitInternal::Pressure MetersH20(10.197442889221);

template <typename T,
          std::enable_if_t<std::is_base_of<UnitInternal::UnitType, T>::value,
                           bool> = true>
constexpr double convert(const T &a, const T &b) {
  return b() / a();
}

};  // namespace Units

#endif  // UNITCONVERSION_H
