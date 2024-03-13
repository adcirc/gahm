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
#include "Date.h"

#include <chrono>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <vector>

#include "date.hpp"

namespace Gahm::Datatypes {

struct s_date {
 private:
  date::year_month_day dd;

 public:
  explicit s_date(const std::chrono::system_clock::time_point &t)
      : dd(date::year_month_day(date::floor<date::days>(t))) {}
  [[nodiscard]] auto year() const -> int { return int(dd.year()); }
  [[nodiscard]] auto month() const -> unsigned { return unsigned(dd.month()); }
  [[nodiscard]] auto day() const -> unsigned { return unsigned(dd.day()); }
};

struct s_datetime {
 private:
  std::chrono::system_clock::time_point t;
  date::year_month_day dd;
  date::hh_mm_ss<std::chrono::system_clock::duration> tt;

 public:
  explicit s_datetime(const std::chrono::system_clock::time_point &tp)
      : t(tp),
        dd(date::year_month_day(date::floor<date::days>(t))),
        tt(date::make_time(t - date::sys_days(dd))) {}
  auto ymd() -> date::year_month_day { return dd; }
  [[nodiscard]] auto year() const -> int { return int(dd.year()); }
  [[nodiscard]] auto month() const -> unsigned { return unsigned(dd.month()); }
  [[nodiscard]] auto day() const -> unsigned { return unsigned(dd.day()); }
  [[nodiscard]] auto hour() const -> long long { return tt.hours().count(); }
  [[nodiscard]] auto minute() const -> long long {
    return tt.minutes().count();
  }
  [[nodiscard]] auto second() const -> long long {
    return tt.seconds().count();
  }
  [[nodiscard]] auto milliseconds() const -> long long {
    const Date c(year(), month(), day(), hour(), minute(), second());
    return std::chrono::duration_cast<std::chrono::milliseconds>(t -
                                                                 c.time_point())
        .count();
  }
};

auto normalize(date::year_month_day ymd) -> date::year_month_day {
  ymd += date::months{0};
  ymd = date::sys_days{ymd};
  return ymd;
}

constexpr auto c_epoch() -> date::year_month_day {
  constexpr int default_year = 1970;
  return {date::year(default_year) / 1 / 1};
}

Date::Date(const long long secondsSinceEpoch) {
  const Date d = Date::fromSeconds(secondsSinceEpoch);
  m_datetime = d.m_datetime;
}

Date::Date() {
  constexpr int default_year = 1970;
  this->set(default_year, 1, 1, 0, 0, 0, 0);
}

Date::Date(const std::chrono::system_clock::time_point &t) { this->set(t); }

Date::Date(const std::vector<long long> &v) { this->set(v); }

Date::Date(const Date &d) { this->set(d.get()); }

Date::Date(int year, unsigned month, unsigned day, long long hour,
           long long minute, long long second, long long millisecond) {
  this->set(year, month, day, hour, minute, second, millisecond);
}

void Date::addSeconds(const long &value) { this->m_datetime += seconds(value); }

void Date::addMinutes(const long &value) { this->m_datetime += minutes(value); }

void Date::addHours(const long &value) { this->m_datetime += hours(value); }

void Date::addDays(const long &value) { this->m_datetime += days(value); }

void Date::addWeeks(const long &value) { this->m_datetime += weeks(value); }

void Date::addMonths(const long &value) { this->m_datetime += months(value); }

void Date::addYears(const long &value) { this->m_datetime += years(value); }

auto Date::operator<(const Date &d) const -> bool {
  return this->time_point() < d.time_point();
}

auto Date::operator>(const Date &d) const -> bool {
  return this->time_point() > d.time_point();
}

auto Date::operator==(const Date &d) const -> bool {
  return this->time_point() == d.time_point();
}

auto Date::operator<=(const Date &d) const -> bool {
  return this->time_point() <= d.time_point();
}

auto Date::operator>=(const Date &d) const -> bool {
  return this->time_point() >= d.time_point();
}

auto Date::operator!=(const Date &d) const -> bool { return !(*(this) == d); }

auto Date::operator-=(const Date::months &rhs) -> Date & {
  s_datetime d(this->m_datetime);
  date::year_month_day dd = d.ymd();
  dd -= date::months(rhs);
  dd = normalize(dd);
  this->set(int(dd.year()), unsigned(dd.month()), unsigned(dd.day()),
            this->hour(), this->minute(), this->second(), this->millisecond());
  return *this;
}

auto Date::operator-=(const Date::years &rhs) -> Date & {
  s_datetime d(this->m_datetime);
  date::year_month_day dd = d.ymd();
  dd -= date::years(rhs);
  dd = normalize(dd);
  this->set(int(dd.year()), unsigned(dd.month()), unsigned(dd.day()),
            this->hour(), this->minute(), this->second(), this->millisecond());
  return *this;
}

auto Date::operator+=(const Date::months &rhs) -> Date & {
  s_datetime d(this->m_datetime);
  date::year_month_day dd = d.ymd();
  dd += date::months(rhs);
  dd = normalize(dd);
  this->set(int(dd.year()), unsigned(dd.month()), unsigned(dd.day()),
            this->hour(), this->minute(), this->second(), this->millisecond());
  return *this;
}

auto Date::operator+=(const Date::years &rhs) -> Date & {
  s_datetime d(this->m_datetime);
  date::year_month_day dd = d.ymd();
  dd += date::years(rhs);
  dd = normalize(dd);
  this->set(int(dd.year()), unsigned(dd.month()), unsigned(dd.day()),
            this->hour(), this->minute(), this->second(), this->millisecond());
  return *this;
}

void Date::set(int year, unsigned month, unsigned day, long long hour,
               long long minute, long long second, long long millisecond) {
  auto ymd = date::year(year) / date::month(month) / date::day(day);
  if (!ymd.ok()) {
    throw std::runtime_error("Invalid date");
  }
  this->m_datetime = date::sys_days(ymd) + std::chrono::hours(hour) +
                     std::chrono::minutes(minute) +
                     std::chrono::seconds(second) +
                     std::chrono::milliseconds(millisecond);
}

auto Date::get() const -> std::vector<long long> {
  const s_datetime time(this->m_datetime);
  return {time.year(),   time.month(),  time.day(),         time.hour(),
          time.minute(), time.second(), time.milliseconds()};
}

void Date::set(const std::vector<long long> &v) {
  constexpr int expected_size = 7;
  std::vector<long long> v2(expected_size);
  std::copy(v.begin(), v.end(), v2.begin());

  this->set(static_cast<int>(v[0]), static_cast<unsigned>(v[1]),
            static_cast<unsigned>(v[2]), v[3], v[4], v[5], v[6]);
}

void Date::set(const std::chrono::system_clock::time_point &t) {
  this->m_datetime = t;
}

void Date::set(const Date &v) { this->set(v.time_point()); }

auto Date::fromSeconds(long seconds) -> Date {
  Date d;
  d.m_datetime = date::sys_days(c_epoch()) + std::chrono::seconds(seconds);
  return d;
}

void Date::fromMSeconds(long long mseconds) {
  constexpr int milliseconds_per_second = 1000;
  this->fromSeconds(mseconds / milliseconds_per_second);
}

auto Date::toSeconds() const -> long {
  return std::chrono::duration_cast<std::chrono::seconds>(
             this->m_datetime - date::sys_days(c_epoch()))
      .count();
}

auto Date::toMSeconds() const -> long long {
  constexpr int milliseconds_per_second = 1000;
  return this->toSeconds() * milliseconds_per_second;
}

auto Date::year() const -> int {
  const s_date d(this->m_datetime);
  return d.year();
}

void Date::setYear(int year) {
  const s_datetime d(this->m_datetime);
  this->set(year, d.month(), d.day(), d.hour(), d.minute(), d.second());
}

auto Date::month() const -> unsigned {
  const s_date d(this->m_datetime);
  return d.month();
}

void Date::setMonth(unsigned month) {
  const s_datetime d(this->m_datetime);
  this->set(d.year(), month, d.day(), d.hour(), d.minute(), d.second());
}

auto Date::day() const -> unsigned {
  const s_date d(this->m_datetime);
  return d.day();
}

void Date::setDay(unsigned day) {
  const s_datetime d(this->m_datetime);
  this->set(d.year(), d.month(), day, d.hour(), d.minute(), d.second());
}

auto Date::hour() const -> long long {
  const s_datetime d(this->m_datetime);
  return d.hour();
}

void Date::setHour(long long hour) {
  const s_datetime d(this->m_datetime);
  this->set(d.year(), d.month(), d.day(), hour, d.minute(), d.second());
}

auto Date::minute() const -> long long {
  const s_datetime d(this->m_datetime);
  return d.minute();
}

void Date::setMinute(long long minute) {
  const s_datetime d(this->m_datetime);
  this->set(d.year(), d.month(), d.day(), d.hour(), minute, d.second());
}

auto Date::second() const -> long long {
  const s_datetime d(this->m_datetime);
  return d.second();
}

void Date::setSecond(long long second) {
  const s_datetime d(this->m_datetime);
  this->set(d.year(), d.month(), d.day(), d.hour(), d.minute(), second);
}

auto Date::millisecond() const -> long long {
  const s_datetime d(this->m_datetime);
  return d.milliseconds();
}

void Date::setMillisecond(long long ms) {
  const s_datetime d(this->m_datetime);
  this->set(d.year(), d.month(), d.day(), d.hour(), d.minute(), d.second(), ms);
}

auto Date::fromString(const std::string &datestr, const std::string &format)
    -> Date {
  Date d;
  std::stringstream ss(datestr);
  date::from_stream(ss, format.c_str(), d.m_datetime);
  return d;
}

auto Date::toString(const std::string &format) const -> std::string {
  return date::format(format, this->m_datetime);
}

auto Date::time_point() const -> std::chrono::system_clock::time_point {
  return this->m_datetime;
}

auto Date::now() -> Date { return Date(std::chrono::system_clock::now()); }
}  // namespace Gahm::Datatypes

auto operator<<(std::ostream &os, const Gahm::Datatypes::Date &dt)
    -> std::ostream & {
  os << dt.toString();
  return os;
}