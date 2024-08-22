//
// Created by Zach Cobell on 8/20/24.
//

#ifndef GAHM_ATCFPERIODPLOT_H
#define GAHM_ATCFPERIODPLOT_H

#include "atcf/AtcfPeriod.h"

namespace Gahm::Plotting {

class AtcfPeriodPlot {
 public:
  explicit AtcfPeriodPlot(const Atcf::AtcfPeriod& period);

  void show() const;
  // void save(const std::string& filename);

  [[nodiscard]] auto period() const -> const Atcf::AtcfPeriod& {
    return m_period;
  }

 private:
  Atcf::AtcfPeriod m_period;
};

}  // namespace Gahm::Plotting

#endif  // GAHM_ATCFPERIODPLOT_H
