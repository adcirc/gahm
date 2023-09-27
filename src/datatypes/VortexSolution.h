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

#include "Uvp.h"
#include "physical/Constants.h"

#ifdef SWIG
#define NODISCARD
#else
#define NODISCARD [[nodiscard]]
#endif

namespace Gahm::Datatypes {
class VortexSolution {
 public:
  VortexSolution() = default;

  explicit VortexSolution(size_t size)
      : m_uvp(size, {0.0, 0.0, Physical::Constants::backgroundPressure()}) {
    m_uvp.resize(size);
  }

  VortexSolution(size_t size, const Gahm::Datatypes::Uvp &value)
      : m_uvp(size, value) {}

  NODISCARD size_t size() const { return m_uvp.size(); }

  NODISCARD bool empty() const { return m_uvp.empty(); }

  void resize(size_t size, const Gahm::Datatypes::Uvp &value) {
    m_uvp.resize(size, value);
  }

  void reserve(size_t size) { m_uvp.reserve(size); }

#ifndef SWIG
  Gahm::Datatypes::Uvp &operator[](size_t index) { return m_uvp[index]; }
  NODISCARD const Gahm::Datatypes::Uvp &operator[](size_t index) const {
    return m_uvp[index];
  }
#endif

  Gahm::Datatypes::Uvp &at(size_t index) { return this->operator[](index); }
  NODISCARD const Gahm::Datatypes::Uvp &at(size_t index) const {
    return this->operator[](index);
  }

  NODISCARD const std::vector<Gahm::Datatypes::Uvp> &uvp() const {
    return m_uvp;
  }

  void push_back(const Gahm::Datatypes::Uvp &value) { m_uvp.push_back(value); }

  NODISCARD std::vector<double> u() const {
    std::vector<double> u;
    u.reserve(m_uvp.size());
    for (const auto &i : m_uvp) {
      u.push_back(i.u());
    }
    return u;
  }

  NODISCARD std::vector<double> v() const {
    std::vector<double> v;
    v.reserve(m_uvp.size());
    for (const auto &i : m_uvp) {
      v.push_back(i.v());
    }
    return v;
  }

  NODISCARD std::vector<double> p() const {
    std::vector<double> p;
    p.reserve(m_uvp.size());
    for (const auto &i : m_uvp) {
      p.push_back(i.p());
    }
    return p;
  }

 private:
  std::vector<Gahm::Datatypes::Uvp> m_uvp;
};
}  // namespace Gahm::Datatypes

#endif  // GAHM_VORTEXSOLUTION_H