//
// Created by Zach Cobell on 7/30/24.
//

#ifndef GAHM_ATCFTRACK_H
#define GAHM_ATCFTRACK_H

#include <utility>
#include <vector>

#include "AtcfPeriod.h"

namespace Gahm::Atcf {
class AtcfTrack
{
public:
  AtcfTrack() = default;

  explicit AtcfTrack(std::vector<AtcfPeriod> periods) : m_periods(std::move(periods))
  {
    this->compute_storm_translation_velocity();
  }

  [[nodiscard]] auto periods() const -> const std::vector<AtcfPeriod> & { return m_periods; }

private:
  void compute_storm_translation_velocity();

  std::vector<AtcfPeriod> m_periods;
};

}// namespace Gahm::Atcf

auto operator<<(std::ostream &os, const Gahm::Atcf::AtcfTrack &track) -> std::ostream &;

#endif// GAHM_ATCFTRACK_H
