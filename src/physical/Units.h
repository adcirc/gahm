//
// Created by Zach Cobell on 7/30/24.
//

#ifndef GAHM_UNITS_H
#define GAHM_UNITS_H

namespace Gahm::Physical {

namespace detail {

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
}  // namespace detail

namespace Units {
constexpr detail::UnitInternal::Length Meter(1.0);
constexpr detail::UnitInternal::Length Kilometer(1.0 / 1000.0);
constexpr detail::UnitInternal::Length Foot(3.28084);
constexpr detail::UnitInternal::Length Mile(1.0 / 1609.34);
constexpr detail::UnitInternal::Length NauticalMile(1.0 / 1851.995396854);

constexpr detail::UnitInternal::Angle Radian(1.0);
constexpr detail::UnitInternal::Angle Degree(180.0 / M_PI);

constexpr detail::UnitInternal::Speed MetersPerSecond(1.0);
constexpr detail::UnitInternal::Speed MilesPerHour(2.23694);
constexpr detail::UnitInternal::Speed Knot(1.94384);
constexpr detail::UnitInternal::Speed KilometersPerHour(3.6);

constexpr detail::UnitInternal::Pressure Millibar(1000.0);
constexpr detail::UnitInternal::Pressure Pascal(100000.0);
constexpr detail::UnitInternal::Pressure Bar(1.0);
constexpr detail::UnitInternal::Pressure MetersH20(10.197442889221);

template <typename T>
constexpr auto convert(const T &lhs, const T &rhs) -> double
  requires(std::is_base_of_v<detail::UnitInternal::UnitType, T>)
{
  return rhs() / lhs();
}

}  // namespace Units
}  // namespace Gahm::Physical
#endif  // GAHM_UNITS_H
