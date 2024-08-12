//
// Created by Zach Cobell on 7/30/24.
//

#ifndef GAHM_DATETIME_H
#define GAHM_DATETIME_H

#include <ostream>
#include <string>

namespace Gahm::Types {
class Datetime {
 public:
  struct s_DateTime {
    short unsigned int year;
    short unsigned int month;
    short unsigned int day;
    long long hour;
    long long minute;
    long long second;
  };

  Datetime();

  explicit Datetime(const std::string &date_string, int hours = 0);

  explicit Datetime(s_DateTime in_datetime);

  [[nodiscard]] constexpr auto operator==(const Datetime &rhs) const -> bool {
    return m_second_since_epoch == rhs.m_second_since_epoch;
  }

  [[nodiscard]] constexpr auto operator<(const Datetime &rhs) const -> bool {
    return m_second_since_epoch < rhs.m_second_since_epoch;
  }

  [[nodiscard]] constexpr auto operator>(const Datetime &rhs) const -> bool {
    return m_second_since_epoch > rhs.m_second_since_epoch;
  }

  [[nodiscard]] constexpr auto operator<=(const Datetime &rhs) const -> bool {
    return m_second_since_epoch <= rhs.m_second_since_epoch;
  }

  [[nodiscard]] constexpr auto operator>=(const Datetime &rhs) const -> bool {
    return m_second_since_epoch >= rhs.m_second_since_epoch;
  }

  [[nodiscard]] constexpr auto operator!=(const Datetime &rhs) const -> bool {
    return m_second_since_epoch != rhs.m_second_since_epoch;
  }

  [[nodiscard]] auto seconds_since_epoch() const -> long {
    return m_second_since_epoch;
  }

  [[nodiscard]] auto to_struct() const -> s_DateTime;

  [[nodiscard]] static auto from_struct(const Datetime::s_DateTime &in_datetime)
      -> long;

  [[nodiscard]] auto to_string() const -> std::string;

  friend auto operator<<(std::ostream &stream,
                         const Datetime &datetime) -> std::ostream &;

 private:
  long m_second_since_epoch;
};

auto operator<<(std::ostream &stream,
                const Datetime &datetime) -> std::ostream &;

}  // namespace Gahm::Types
#endif  // GAHM_DATETIME_H
