//
// Created by Zach Cobell on 3/15/23.
//

#ifndef GAHM_VORTEXSOLUTION_H
#define GAHM_VORTEXSOLUTION_H

#include <cstdlib>
#include <vector>

#include "physical/Constants.h"

namespace Gahm::Datatypes {
class VortexSolution {
 public:
  struct t_uvp {
    double u;
    double v;
    double p;
  };

  VortexSolution() = default;

  explicit VortexSolution(size_t size)
      : m_uvp(size, {0.0, 0.0, Physical::Constants::backgroundPressure()}) {
    m_uvp.resize(size);
  }

  VortexSolution(size_t size, const t_uvp &value) : m_uvp(size, value) {}

  [[nodiscard]] size_t size() const { return m_uvp.size(); }

  void resize(size_t size, const t_uvp &value) { m_uvp.resize(size, value); }

  void reserve(size_t size) { m_uvp.reserve(size); }

  t_uvp &operator[](size_t index) { return m_uvp[index]; }
  const t_uvp &operator[](size_t index) const { return m_uvp[index]; }

  [[nodiscard]] const std::vector<t_uvp> &uvp() const { return m_uvp; }

  void push_back(const t_uvp &value) { m_uvp.push_back(value); }

 private:
  std::vector<VortexSolution::t_uvp> m_uvp;
};
}  // namespace Gahm::Datatypes

#endif  // GAHM_VORTEXSOLUTION_H
