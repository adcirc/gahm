// GNU General Public License v3.0
//
// This file is part of the GAHM model (https://github.com/adcirc/gahm).
// Copyright (c) 2023 ADCIRC Development Group.
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, version 3.
//
// This program is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program. If not, see <http://www.gnu.org/licenses/>.
//
// Author: Zach Cobell
// Contact: zcobell@thewaterinstitute.org
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

  [[nodiscard]] std::vector<double> u() const {
    std::vector<double> u;
    u.reserve(m_uvp.size());
    for (const auto &i : m_uvp) {
      u.push_back(i.u);
    }
    return u;
  }

  [[nodiscard]] std::vector<double> v() const {
    std::vector<double> v;
    v.reserve(m_uvp.size());
    for (const auto &i : m_uvp) {
      v.push_back(i.v);
    }
    return v;
  }

  [[nodiscard]] std::vector<double> p() const {
    std::vector<double> p;
    p.reserve(m_uvp.size());
    for (const auto &i : m_uvp) {
      p.push_back(i.p);
    }
    return p;
  }

 private:
  std::vector<VortexSolution::t_uvp> m_uvp;
};
}  // namespace Gahm::Datatypes

#endif  // GAHM_VORTEXSOLUTION_H
