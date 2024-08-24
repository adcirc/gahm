//
// Created by Zach Cobell on 8/20/24.
//

#ifndef GAHM_ATCFPERIODPLOT_HPP
#define GAHM_ATCFPERIODPLOT_HPP

#include "atcf/AtcfPeriod.hpp"

namespace Gahm::Plotting {

class AtcfPeriodPlot {
 public:
  explicit AtcfPeriodPlot(const Atcf::AtcfPeriod& period);

  void show() const;

  void save(const std::string& filename) const;


 private:
  Atcf::AtcfPeriod m_period;
};

}  // namespace Gahm::Plotting

#endif  // GAHM_ATCFPERIODPLOT_HPP
