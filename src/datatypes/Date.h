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
namespace Gahm::Datatypes {
class Date {
 public:
  using milliseconds = std::chrono::milliseconds;
  using seconds = std::chrono::seconds;
  using minutes = std::chrono::minutes;
  using hours = std::chrono::hours;
  using days = std::chrono::duration<
      int, std::ratio_multiply<std::ratio<24>, std::chrono::hours::period>>;
  using weeks =
      std::chrono::duration<int,
                            std::ratio_multiply<std::ratio<7>, days::period>>;
  using years = std::chrono::duration<
      int, std::ratio_multiply<std::ratio<146097, 400>, days::period>>;
  using months =
      std::chrono::duration<int,
                            std::ratio_divide<years::period, std::ratio<12>>>;

  Date();

  explicit Date(long long secondsSinceEpoch);

  explicit Date(const std::chrono::system_clock::time_point &t);

  explicit Date(const std::vector<long long> &v);

  explicit Date(int year, unsigned month, unsigned day, long long hour = 0,
                long long minute = 0, long long second = 0,
                long long millisecond = 0);

  Date(const Date &d);

  //...operator overloads
  bool operator<(const Date &d) const;
  bool operator>(const Date &d) const;
  bool operator<=(const Date &d) const;
  bool operator>=(const Date &d) const;
  bool operator==(const Date &d) const;
  bool operator!=(const Date &d) const;
  Date &operator=(const Date &d) = default;

  template <class T, typename std::enable_if<std::is_integral<T>::value>::type
                         * = nullptr>
  Date &operator+=(const T &rhs) {
    this->m_datetime += Date::seconds(rhs);
    return *this;
  }

  template <class T, typename std::enable_if<
                         std::is_floating_point<T>::value>::type * = nullptr>
  Date &operator+=(const T &rhs) {
    this->m_datetime +=
        Date::milliseconds(static_cast<long>(std::floor(rhs * 1000.0)));
    return *this;
  }

  template <class T,
            typename std::enable_if<!std::is_integral<T>::value &&
                                    !std::is_floating_point<T>::value &&
                                    !std::is_same<T, Date::years>::value &&
                                    !std::is_same<T, Date::months>::value>::type
                * = nullptr>
  Date &operator+=(const T &rhs) {
    this->m_datetime += rhs;
    return *this;
  }

  Date &operator+=(const Date::years &rhs);
  Date &operator+=(const Date::months &rhs);

  template <class T, typename std::enable_if<std::is_integral<T>::value>::type
                         * = nullptr>
  Date &operator-=(const T &rhs) {
    this->m_datetime -= Date::seconds(rhs);
    return *this;
  }

  template <class T, typename std::enable_if<
                         std::is_floating_point<T>::value>::type * = nullptr>
  Date &operator-=(const T &rhs) {
    this->m_datetime -=
        Date::milliseconds(static_cast<long>(std::floor(rhs * 1000.0)));
    return *this;
  }

  template <class T,
            typename std::enable_if<!std::is_integral<T>::value &&
                                    !std::is_floating_point<T>::value &&
                                    !std::is_same<T, Date::years>::value &&
                                    !std::is_same<T, Date::months>::value>::type
                * = nullptr>
  Date &operator-=(const T &rhs) {
    this->m_datetime -= rhs;
    return *this;
  }

  Date &operator-=(const Date::years &rhs);
  Date &operator-=(const Date::months &rhs);

  void addSeconds(const long &value);
  void addMinutes(const long &value);
  void addHours(const long &value);
  void addDays(const long &value);
  void addWeeks(const long &value);
  void addMonths(const long &value);
  void addYears(const long &value);

  static Date maxDate() { return Date(3000, 1, 1, 0, 0, 0); }
  static Date minDate() { return Date(1900, 1, 1, 0, 0, 0); }

  [[nodiscard]] std::vector<long long> get() const;

  void set(const std::vector<long long> &v);
  void set(const std::chrono::system_clock::time_point &t);
  void set(const Date &v);
  void set(int year, unsigned month = 1, unsigned day = 1, long long hour = 0,
           long long minute = 0, long long second = 0,
           long long millisecond = 0);

  static Date fromSeconds(long seconds);

  void fromMSeconds(long long mseconds);

  [[nodiscard]] long toSeconds() const;

  [[nodiscard]] long long toMSeconds() const;

  [[nodiscard]] int year() const;
  void setYear(int year);

  [[nodiscard]] unsigned month() const;
  void setMonth(unsigned month);

  [[nodiscard]] unsigned day() const;
  void setDay(unsigned day);

  [[nodiscard]] long long hour() const;
  void setHour(long long hour);

  [[nodiscard]] long long minute() const;
  void setMinute(long long minute);

  [[nodiscard]] long long second() const;
  void setSecond(long long second);

  [[nodiscard]] long long millisecond() const;
  void setMillisecond(long long milliseconds);

  [[nodiscard]] static Date fromString(
      const std::string &datestr,
      const std::string &format = "%Y-%m-%d %H:%M:%OS");

  [[nodiscard]] std::string toString(
      const std::string &format = "%Y-%m-%d %H:%M:%OS") const;

  [[nodiscard]] std::chrono::system_clock::time_point time_point() const;

  static Date now();

 private:
  std::chrono::system_clock::time_point m_datetime;
};

template <typename T>
Date operator+(Date lhs, const T &rhs) {
  lhs += rhs;
  return lhs;
}

template <typename T>
Date operator-(Date lhs, const T &rhs) {
  lhs -= rhs;
  return lhs;
}
}  // namespace Gahm::Datatypes

std::ostream &operator<<(std::ostream &os, const Gahm::Datatypes::Date &dt);
#endif  // GAHMDATE_H
