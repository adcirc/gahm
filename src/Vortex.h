//
// Created by Zach Cobell on 5/10/22.
//

#ifndef GAHM2_SRC_VORTEX_H_
#define GAHM2_SRC_VORTEX_H_

#include <string>

#include "Atcf.h"

namespace Gahm {
class Vortex {
 public:
  explicit Vortex(std::string filename);

  virtual void solve() = 0;

  [[nodiscard]] const Gahm::Atcf &atcf() const;

  [[nodiscard]] Gahm::Atcf &atcf();

  [[nodiscard]] static double gamma(const StormPosition &position, double vmax,
                                    double vr, double u_vr, double v_vr,
                                    double u_transit, double v_transit);

 private:
  Gahm::Atcf m_atcf;
};
}  // namespace Gahm
#endif  // GAHM2_SRC_VORTEX_H_
