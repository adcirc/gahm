//
// Created by Zach Cobell on 7/30/24.
//

#include "Datetime.h"

#include <stdexcept>
#include <string>

#include "boost/date_time/gregorian/gregorian.hpp"
#include "boost/date_time/posix_time/posix_time.hpp"

using namespace Gahm::Types;

constexpr auto epoch() -> boost::posix_time::ptime {
  return boost::posix_time::ptime(boost::gregorian::date(1970, 1, 1));
}

constexpr auto from_seconds(long seconds) -> boost::posix_time::ptime {
  return epoch() + boost::posix_time::seconds(seconds);
}

Datetime::Datetime() : m_second_since_epoch(0) {}

Datetime::Datetime(const std::string &date_string, int hours) {
  auto date_string_internal = date_string;
  if (date_string.size() == 10) {
    auto string_hours = date_string.substr(8, 2);
    hours += std::stoi(string_hours);
    date_string_internal = date_string.substr(0, 8);
  } else if (date_string.size() != 8) {
    throw std::invalid_argument("Invalid date string");
  }

  boost::gregorian::date const date =
      boost::gregorian::from_undelimited_string(date_string_internal);
  boost::posix_time::ptime ptime(date);
  ptime += boost::posix_time::hours(hours);
  m_second_since_epoch = (ptime - epoch()).total_seconds();
}

Datetime::Datetime(Datetime::s_DateTime in_datetime)
    : m_second_since_epoch(from_struct(in_datetime)) {}

auto Datetime::to_struct() const -> Datetime::s_DateTime {
  const auto ptime = from_seconds(m_second_since_epoch);
  return Datetime::s_DateTime{ptime.date().year(),
                              ptime.date().month(),
                              ptime.date().day(),
                              ptime.time_of_day().hours(),
                              ptime.time_of_day().minutes(),
                              ptime.time_of_day().seconds()};
}

auto Datetime::from_struct(const s_DateTime &in_datetime) -> long {
  auto ptime = boost::posix_time::ptime(boost::gregorian::date(
      in_datetime.year, in_datetime.month, in_datetime.day));
  ptime += boost::posix_time::hours(in_datetime.hour);
  ptime += boost::posix_time::minutes(in_datetime.minute);
  ptime += boost::posix_time::seconds(in_datetime.second);
  return ptime.time_of_day().total_seconds();
}

auto Gahm::Types::operator<<(std::ostream &stream,
                             const Datetime &datetime) -> std::ostream & {
  const auto ptime = from_seconds(datetime.m_second_since_epoch);
  stream << boost::posix_time::to_simple_string(ptime);
  return stream;
}

auto Datetime::to_string() const -> std::string {
  const auto ptime = from_seconds(m_second_since_epoch);
  return boost::posix_time::to_iso_extended_string(ptime);
}