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
#ifndef HMDFDATE_H
#define HMDFDATE_H

#include <chrono>
#include <cmath>
#include <iostream>
#include <ratio>
#include <string>
#include <type_traits>
#include <vector>

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

  explicit Date(const std::vector<int> &v);

  explicit Date(int year, unsigned month, unsigned day, unsigned hour = 0,
                unsigned minute = 0, unsigned second = 0,
                unsigned millisecond = 0);

  Date(const Date &d);

  //...operator overloads
  bool operator<(const Date &d) const;
  bool operator>(const Date &d) const;
  bool operator<=(const Date &d) const;
  bool operator==(const Date &d) const;
  bool operator!=(const Date &d) const;

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

  friend std::ostream &operator<<(std::ostream &os, const Date &dt);

  void addSeconds(const long &value);
  void addMinutes(const long &value);
  void addHours(const long &value);
  void addDays(const long &value);
  void addWeeks(const long &value);
  void addMonths(const long &value);
  void addYears(const long &value);

  static Date maxDate() { return Date(3000, 1, 1, 0, 0, 0); }
  static Date minDate() { return Date(1900, 1, 1, 0, 0, 0); }

  std::vector<int> get() const;

  void set(const std::vector<int> &v);
  void set(const std::chrono::system_clock::time_point &t);
  void set(const Date &v);
  void set(int year, unsigned month = 1, unsigned day = 1, unsigned hour = 0,
           unsigned minute = 0, unsigned second = 0, unsigned millisecond = 0);

  void fromSeconds(long seconds);

  void fromMSeconds(long long mseconds);

  long toSeconds() const;

  long long toMSeconds() const;

  int year() const;
  void setYear(int year);

  unsigned month() const;
  void setMonth(unsigned month);

  unsigned day() const;
  void setDay(unsigned day);

  unsigned hour() const;
  void setHour(unsigned hour);

  unsigned minute() const;
  void setMinute(unsigned minute);

  unsigned second() const;
  void setSecond(unsigned second);

  unsigned millisecond() const;
  void setMillisecond(unsigned milliseconds);

  void fromString(const std::string &datestr,
                  const std::string &format = "%Y-%m-%d %H:%M:%OS");

  std::string toString(const std::string &format = "%Y-%m-%d %H:%M:%OS") const;

  std::chrono::system_clock::time_point time_point() const;

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
#endif  // DATE_H
