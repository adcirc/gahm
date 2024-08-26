//
// Created by Zach Cobell on 7/30/24.
//

#ifndef GAHM_ATCFTRACK_HPP
#define GAHM_ATCFTRACK_HPP

#include <algorithm>
#include <numeric>
#include <tuple>
#include <utility>
#include <vector>

#include "AtcfPeriod.hpp"
#include "datatypes/Datetime.hpp"

namespace Gahm::Atcf {
class AtcfTrack {
 public:
  AtcfTrack() = default;

  explicit AtcfTrack(std::vector<AtcfPeriod> periods)
      : m_periods(std::move(periods)) {
    this->compute_storm_translation_velocity();
    this->compute_gahm_parameters();
  }

  [[nodiscard]] const std::vector<AtcfPeriod> &periods() const {
    return m_periods;
  }

  void compute_gahm_parameters() {
    std::ranges::for_each(m_periods, [](AtcfPeriod &period) {
      period.compute_gahm_parameters();
    });
  }

#ifndef SWIG
  [[nodiscard]] std::tuple<const Gahm::Atcf::AtcfPeriod,
                           const Gahm::Atcf::AtcfPeriod, const double>
  select_periods(const Gahm::Types::Datetime &select_time) const {
    const auto period_2 =
        std::lower_bound(m_periods.begin(), m_periods.end(), select_time,
                         [](const AtcfPeriod &period,
                            const Gahm::Types::Datetime &time) -> bool {
                           return period.datetime() < time;
                         });

    if (period_2 == m_periods.begin()) {
      return {m_periods.front(), m_periods.front(), 0.0};
    } else if (period_2 == m_periods.end()) {
      return {m_periods.back(), m_periods.back(), 0.0};
    } else {
      const auto secs = select_time.seconds_since_epoch();
      const auto period_1 = std::prev(period_2);
      const auto secs_p1 = period_1->datetime().seconds_since_epoch();
      const auto secs_p2 = period_2->datetime().seconds_since_epoch();
      const auto weight = static_cast<double>(secs - secs_p1) /
                          static_cast<double>(secs_p2 - secs_p1);
      return {*period_1, *period_2, weight};
    }
  }
#endif

 private:
  void compute_storm_translation_velocity();

  std::vector<AtcfPeriod> m_periods;
};

}  // namespace Gahm::Atcf

#ifndef SWIG
auto operator<<(std::ostream &os,
                const Gahm::Atcf::AtcfTrack &track) -> std::ostream &;
#endif

#endif  // GAHM_ATCFTRACK_HPP
