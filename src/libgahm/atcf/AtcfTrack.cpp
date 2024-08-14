//
// Created by Zach Cobell on 7/30/24.
//

#include "AtcfTrack.h"

#include <iostream>
#include <iterator>

void Gahm::Atcf::AtcfTrack::compute_storm_translation_velocity() {
  if (m_periods.size() < 2) {
    return;
  }
  for (auto this_period = std::next(m_periods.begin());
       this_period != m_periods.end(); ++this_period) {
    const auto previous_period = std::prev(this_period);
    const auto delta_time =
        static_cast<double>(previous_period->datetime().seconds_since_epoch() -
                            this_period->datetime().seconds_since_epoch());
    this_period->set_translation(
        Storm::StormTranslation(previous_period->eye_location(),
                                this_period->eye_location(), delta_time));
  }
  m_periods.at(0).set_translation(m_periods[1].translation());
}

auto operator<<(std::ostream &os,
                const Gahm::Atcf::AtcfTrack &track) -> std::ostream & {
  os << "-----------------------------------\n";
  os << " Date Range: " << track.periods().front().datetime() << " to "
     << track.periods().back().datetime() << "\n";
  os << " Number of Periods: " << track.periods().size() << "\n";
  for (const auto &period : track.periods()) {
    os << period;
  }
  os << "-----------------------------------\n";

  return os;
}