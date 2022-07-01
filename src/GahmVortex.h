//
// Created by Zach Cobell on 5/10/22.
//

#ifndef GAHM2_SRC_GAHMVORTEX_H_
#define GAHM2_SRC_GAHMVORTEX_H_

#include <string>

#include "Vortex.h"

namespace Gahm {
class GahmVortex : public Vortex {
 public:
  explicit GahmVortex(std::string filename);

  void solve() override;

 private:
  static void computeQuadrantSolution(const Gahm::ForecastPeriod &period,
                                      const Gahm::StormIsotach &isotach,
                                      Gahm::StormQuadrant &quadrant);
};
}  // namespace Gahm
#endif  // GAHM2_SRC_GAHMVORTEX_H_
