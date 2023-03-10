// MIT License
//
// Copyright (c) 2023 ADCIRC Development Group
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
#include "Date.h"

#include <chrono>
#include <iostream>

#include "date.hpp"

namespace Gahm::Datatypes {

struct s_date {
 private:
  date::year_month_day dd;

 public:
  explicit s_date(const std::chrono::system_clock::time_point &t)
      : dd(date::year_month_day(date::floor<date::days>(t))) {}
  [[nodiscard]] int year() const { return int(dd.year()); }
  [[nodiscard]] unsigned month() const { return unsigned(dd.month()); }
  [[nodiscard]] unsigned day() const { return unsigned(dd.day()); }
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
  date::year_month_day ymd() { return dd; }
  [[nodiscard]] int year() const { return int(dd.year()); }
  [[nodiscard]] unsigned month() const { return unsigned(dd.month()); }
  [[nodiscard]] unsigned day() const { return unsigned(dd.day()); }
  [[nodiscard]] long long hour() const { return tt.hours().count(); }
  [[nodiscard]] long long minute() const { return tt.minutes().count(); }
  [[nodiscard]] long long second() const { return tt.seconds().count(); }
  [[nodiscard]] long long milliseconds() const {
    Date c(year(), month(), day(), hour(), minute(), second());
    return std::chrono::duration_cast<std::chrono::milliseconds>(t -
                                                                 c.time_point())
        .count();
  }
};

date::year_month_day normalize(date::year_month_day ymd) {
  ymd += date::months{0};
  ymd = date::sys_days{ymd};
  return ymd;
}

constexpr date::year_month_day c_epoch() { return {date::year(1970) / 1 / 1}; }

Date::Date(const long long secondsSinceEpoch) {
  Date d = Date::fromSeconds(secondsSinceEpoch);
  m_datetime = d.m_datetime;
}

Date::Date() { this->set(1970, 1, 1, 0, 0, 0, 0); }

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

bool Date::operator<(const Date &d) const {
  return this->time_point() < d.time_point();
}

bool Date::operator>(const Date &d) const {
  return this->time_point() > d.time_point();
}

bool Date::operator==(const Date &d) const {
  return this->time_point() == d.time_point();
}

bool Date::operator<=(const Date &d) const {
  return this->time_point() <= d.time_point();
}

bool Date::operator>=(const Date &d) const {
  return this->time_point() >= d.time_point();
}

bool Date::operator!=(const Date &d) const { return !(*(this) == d); }

Date &Date::operator-=(const Date::months &rhs) {
  s_datetime d(this->m_datetime);
  date::year_month_day dd = d.ymd();
  dd -= date::months(rhs);
  dd = normalize(dd);
  this->set(int(dd.year()), unsigned(dd.month()), unsigned(dd.day()),
            this->hour(), this->minute(), this->second(), this->millisecond());
  return *this;
}

Date &Date::operator-=(const Date::years &rhs) {
  s_datetime d(this->m_datetime);
  date::year_month_day dd = d.ymd();
  dd -= date::years(rhs);
  dd = normalize(dd);
  this->set(int(dd.year()), unsigned(dd.month()), unsigned(dd.day()),
            this->hour(), this->minute(), this->second(), this->millisecond());
  return *this;
}

Date &Date::operator+=(const Date::months &rhs) {
  s_datetime d(this->m_datetime);
  date::year_month_day dd = d.ymd();
  dd += date::months(rhs);
  dd = normalize(dd);
  this->set(int(dd.year()), unsigned(dd.month()), unsigned(dd.day()),
            this->hour(), this->minute(), this->second(), this->millisecond());
  return *this;
}

Date &Date::operator+=(const Date::years &rhs) {
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

std::vector<long long> Date::get() const {
  s_datetime time(this->m_datetime);
  std::vector<long long> v(7);
  v[0] = time.year();
  v[1] = time.month();
  v[2] = time.day();
  v[3] = time.hour();
  v[4] = time.minute();
  v[5] = time.second();
  v[6] = time.milliseconds();
  return v;
}

void Date::set(const std::vector<long long> &v) {
  std::vector<long long> v2(7);
  std::copy(v.begin(), v.end(), v2.begin());

  this->set(static_cast<int>(v[0]), static_cast<unsigned>(v[1]),
            static_cast<unsigned>(v[2]), v[3], v[4], v[5], v[6]);
}

void Date::set(const std::chrono::system_clock::time_point &t) {
  this->m_datetime = t;
}

void Date::set(const Date &v) { this->set(v.time_point()); }

Date Date::fromSeconds(long seconds) {
  Date d;
  d.m_datetime = date::sys_days(c_epoch()) + std::chrono::seconds(seconds);
  return d;
}

void Date::fromMSeconds(long long mseconds) {
  this->fromSeconds(mseconds / 1000);
}

long Date::toSeconds() const {
  return std::chrono::duration_cast<std::chrono::seconds>(
             this->m_datetime - date::sys_days(c_epoch()))
      .count();
}

long long Date::toMSeconds() const { return this->toSeconds() * 1000; }

int Date::year() const {
  s_date d(this->m_datetime);
  return d.year();
}

void Date::setYear(int year) {
  s_datetime d(this->m_datetime);
  this->set(year, d.month(), d.day(), d.hour(), d.minute(), d.second());
}

unsigned Date::month() const {
  s_date d(this->m_datetime);
  return d.month();
}

void Date::setMonth(unsigned month) {
  s_datetime d(this->m_datetime);
  this->set(d.year(), month, d.day(), d.hour(), d.minute(), d.second());
}

unsigned Date::day() const {
  s_date d(this->m_datetime);
  return d.day();
}

void Date::setDay(unsigned day) {
  s_datetime d(this->m_datetime);
  this->set(d.year(), d.month(), day, d.hour(), d.minute(), d.second());
}

long long Date::hour() const {
  s_datetime d(this->m_datetime);
  return d.hour();
}

void Date::setHour(long long hour) {
  s_datetime d(this->m_datetime);
  this->set(d.year(), d.month(), d.day(), hour, d.minute(), d.second());
}

long long Date::minute() const {
  s_datetime d(this->m_datetime);
  return d.minute();
}

void Date::setMinute(long long minute) {
  s_datetime d(this->m_datetime);
  this->set(d.year(), d.month(), d.day(), d.hour(), minute, d.second());
}

long long Date::second() const {
  s_datetime d(this->m_datetime);
  return d.second();
}

void Date::setSecond(long long second) {
  s_datetime d(this->m_datetime);
  this->set(d.year(), d.month(), d.day(), d.hour(), d.minute(), second);
}

long long Date::millisecond() const {
  s_datetime d(this->m_datetime);
  return d.milliseconds();
}

void Date::setMillisecond(long long ms) {
  s_datetime d(this->m_datetime);
  this->set(d.year(), d.month(), d.day(), d.hour(), d.minute(), d.second(), ms);
}

Date Date::fromString(const std::string &datestr, const std::string &format) {
  Date d;
  std::stringstream ss(datestr);
  date::from_stream(ss, format.c_str(), d.m_datetime);
  return d;
}

std::string Date::toString(const std::string &format) const {
  return date::format(format, this->m_datetime);
}

std::chrono::system_clock::time_point Date::time_point() const {
  return this->m_datetime;
}

Date Date::now() { return Date(std::chrono::system_clock::now()); }
}  // namespace Gahm::Datatypes

std::ostream &operator<<(std::ostream &os, const Gahm::Datatypes::Date &dt) {
  os << dt.toString();
  return os;
}