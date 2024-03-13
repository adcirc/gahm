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
#ifndef GAHMDATE_H
#define GAHMDATE_H

#include <chrono>
#include <cmath>
#include <iostream>
#include <ratio>
#include <string>
#include <type_traits>
#include <vector>

#ifdef SWIG
#define NODISCARD
#else
#define NODISCARD [[nodiscard]]
#endif

namespace Gahm::Datatypes {
class Date {
 public:
  static constexpr int hours_per_day = 24;
  static constexpr int days_per_week = 7;
  static constexpr int months_per_year = 12;
  static constexpr int days_per_year_numerator = 146097;
  static constexpr int days_per_year_denominator = 400;

  using milliseconds = std::chrono::milliseconds;
  using seconds = std::chrono::seconds;
  using minutes = std::chrono::minutes;
  using hours = std::chrono::hours;
  using days =
      std::chrono::duration<int,
                            std::ratio_multiply<std::ratio<hours_per_day>,
                                                std::chrono::hours::period>>;
  using weeks = std::chrono::duration<
      int, std::ratio_multiply<std::ratio<days_per_week>, days::period>>;
  using years = std::chrono::duration<
      int, std::ratio_multiply<
               std::ratio<days_per_year_numerator, days_per_year_denominator>,
               days::period>>;
  using months = std::chrono::duration<
      int, std::ratio_divide<years::period, std::ratio<months_per_year>>>;

  Date();

  explicit Date(long long secondsSinceEpoch);

  explicit Date(const std::chrono::system_clock::time_point &t);

  explicit Date(const std::vector<long long> &v);

  explicit Date(int year, unsigned month, unsigned day, long long hour = 0,
                long long minute = 0, long long second = 0,
                long long millisecond = 0);

  Date(const Date &d);

  //...operator overloads
#ifndef SWIG
  auto operator<(const Date &d) const -> bool;
  auto operator>(const Date &d) const -> bool;
  auto operator<=(const Date &d) const -> bool;
  auto operator>=(const Date &d) const -> bool;
  auto operator==(const Date &d) const -> bool;
  auto operator!=(const Date &d) const -> bool;
  auto operator=(const Date &d) -> Date & = default;

  template <class T, std::enable_if_t<std::is_integral_v<T>> * = nullptr>
  auto operator+=(const T &rhs) -> Date & {
    this->m_datetime += Date::seconds(rhs);
    return *this;
  }

  template <class T, std::enable_if_t<std::is_floating_point_v<T>> * = nullptr>
  auto operator+=(const T &rhs) -> Date & {
    constexpr int milliseconds_per_second = 1000;
    this->m_datetime += Date::milliseconds(
        static_cast<long>(std::floor(rhs * milliseconds_per_second)));
    return *this;
  }

  template <
      class T,
      std::enable_if_t<!std::is_integral_v<T> && !std::is_floating_point_v<T> &&
                       !std::is_same_v<T, Date::years> &&
                       !std::is_same_v<T, Date::months>> * = nullptr>
  auto operator+=(const T &rhs) -> Date & {
    this->m_datetime += rhs;
    return *this;
  }

  auto operator+=(const Date::years &rhs) -> Date &;
  auto operator+=(const Date::months &rhs) -> Date &;

  template <class T, std::enable_if_t<std::is_integral_v<T>> * = nullptr>
  auto operator-=(const T &rhs) -> Date & {
    this->m_datetime -= Date::seconds(rhs);
    return *this;
  }

  template <class T, std::enable_if_t<std::is_floating_point_v<T>> * = nullptr>
  auto operator-=(const T &rhs) -> Date & {
    constexpr int milliseconds_per_second = 1000;
    this->m_datetime -= Date::milliseconds(
        static_cast<long>(std::floor(rhs * milliseconds_per_second)));
    return *this;
  }

  template <
      class T,
      std::enable_if_t<!std::is_integral_v<T> && !std::is_floating_point_v<T> &&
                       !std::is_same_v<T, Date::years> &&
                       !std::is_same_v<T, Date::months>> * = nullptr>
  auto operator-=(const T &rhs) -> Date & {
    this->m_datetime -= rhs;
    return *this;
  }

  auto operator-=(const Date::years &rhs) -> Date &;
  auto operator-=(const Date::months &rhs) -> Date &;

#endif

  void addSeconds(const long &value);
  void addMinutes(const long &value);
  void addHours(const long &value);
  void addDays(const long &value);
  void addWeeks(const long &value);
  void addMonths(const long &value);
  void addYears(const long &value);

  static auto maxDate() -> Date {
    constexpr int max_year = 3000;
    return Date(max_year, 1, 1, 0, 0, 0);
  }
  static auto minDate() -> Date {
    constexpr int min_year = 1900;
    return Date(min_year, 1, 1, 0, 0, 0);
  }

  NODISCARD auto get() const -> std::vector<long long>;

  void set(const std::vector<long long> &v);
  void set(const std::chrono::system_clock::time_point &t);
  void set(const Date &v);
  void set(int year, unsigned month = 1, unsigned day = 1, long long hour = 0,
           long long minute = 0, long long second = 0,
           long long millisecond = 0);

  static auto fromSeconds(long seconds) -> Date;

  void fromMSeconds(long long mseconds);

  NODISCARD auto toSeconds() const -> long;

  NODISCARD auto toMSeconds() const -> long long;

  NODISCARD auto year() const -> int;
  void setYear(int year);

  NODISCARD auto month() const -> unsigned;
  void setMonth(unsigned month);

  NODISCARD auto day() const -> unsigned;
  void setDay(unsigned day);

  NODISCARD auto hour() const -> long long;
  void setHour(long long hour);

  NODISCARD auto minute() const -> long long;
  void setMinute(long long minute);

  NODISCARD auto second() const -> long long;
  void setSecond(long long second);

  NODISCARD auto millisecond() const -> long long;
  void setMillisecond(long long milliseconds);

  NODISCARD static auto fromString(
      const std::string &datestr,
      const std::string &format = "%Y-%m-%d %H:%M:%OS") -> Date;

  NODISCARD auto toString(
      const std::string &format = "%Y-%m-%d %H:%M:%OS") const -> std::string;

  NODISCARD auto time_point() const -> std::chrono::system_clock::time_point;

  static auto now() -> Date;

 private:
  std::chrono::system_clock::time_point m_datetime;
};

template <typename T>
auto operator+(Date lhs, const T &rhs) -> Date {
  lhs += rhs;
  return lhs;
}

template <typename T>
auto operator-(Date lhs, const T &rhs) -> Date {
  lhs -= rhs;
  return lhs;
}
}  // namespace Gahm::Datatypes

#ifndef SWIG
auto operator<<(std::ostream &os, const Gahm::Datatypes::Date &dt)
    -> std::ostream &;
#endif

#endif  // GAHMDATE_H
