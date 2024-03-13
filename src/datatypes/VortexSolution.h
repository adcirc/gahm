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

#ifdef SWIG
#define NODISCARD
#else
#define NODISCARD [[nodiscard]]
#endif

namespace Gahm::Datatypes {
class VortexSolution {
 public:
  VortexSolution() = default;

  explicit VortexSolution(size_t size) { m_uvp.reserve(size); }

  NODISCARD auto size() const -> size_t { return m_uvp.size(); }

  NODISCARD auto empty() const -> bool { return m_uvp.empty(); }

  void resize(size_t size, const Gahm::Datatypes::Uvp &value) {
    m_uvp.resize(size, value);
  }

  void reserve(size_t size) { m_uvp.reserve(size); }

#ifndef SWIG
  auto operator[](size_t index) -> Gahm::Datatypes::Uvp & {
    return m_uvp[index];
  }
  NODISCARD auto operator[](size_t index) const
      -> const Gahm::Datatypes::Uvp & {
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

  template <class... Args>
  void emplace_back(Args &&...args) {
    m_uvp.emplace_back(std::forward<Args>(args)...);
  }

#ifndef SWIG
  auto front() { return m_uvp.front(); }
  auto back() { return m_uvp.back(); }
#endif

  NODISCARD auto u() const -> std::vector<double> {
    std::vector<double> u;
    u.reserve(m_uvp.size());
    for (const auto &i : m_uvp) {
      u.push_back(i.u());
    }
    return u;
  }

  NODISCARD auto v() const -> std::vector<double> {
    std::vector<double> v;
    v.reserve(m_uvp.size());
    for (const auto &i : m_uvp) {
      v.push_back(i.v());
    }
    return v;
  }

  NODISCARD auto p() const -> std::vector<double> {
    std::vector<double> p;
    p.reserve(m_uvp.size());
    for (const auto &i : m_uvp) {
      p.push_back(i.p());
    }
    return p;
  }

  NODISCARD auto azimuth() const -> std::vector<double> {
    std::vector<double> azimuth;
    azimuth.reserve(m_uvp.size());
    for (const auto &i : m_uvp) {
      azimuth.push_back(i.azimuth());
    }
    return azimuth;
  }

  NODISCARD auto distance() const -> std::vector<double> {
    std::vector<double> distance;
    distance.reserve(m_uvp.size());
    for (const auto &i : m_uvp) {
      distance.push_back(i.distance());
    }
    return distance;
  }

  NODISCARD auto tsx() const -> std::vector<double> {
    std::vector<double> tsx;
    tsx.reserve(m_uvp.size());
    for (const auto &i : m_uvp) {
      tsx.push_back(i.tsx());
    }
    return tsx;
  }

  NODISCARD auto tsy() const -> std::vector<double> {
    std::vector<double> tsy;
    tsy.reserve(m_uvp.size());
    for (const auto &i : m_uvp) {
      tsy.push_back(i.tsy());
    }
    return tsy;
  }

  NODISCARD auto quadrant_weight() const -> std::vector<double> {
    std::vector<double> quadrant_weight;
    quadrant_weight.reserve(m_uvp.size());
    for (const auto &i : m_uvp) {
      quadrant_weight.push_back(i.quadrant_weight());
    }
    return quadrant_weight;
  }

  NODISCARD auto isotach_weight() const -> std::vector<double> {
    std::vector<double> isotach_weight;
    isotach_weight.reserve(m_uvp.size());
    for (const auto &i : m_uvp) {
      isotach_weight.push_back(i.isotach_weight());
    }
    return isotach_weight;
  }

  NODISCARD auto quadrant() const -> std::vector<double> {
    std::vector<double> quadrant;
    quadrant.reserve(m_uvp.size());
    for (const auto &iter : m_uvp) {
      quadrant.push_back(iter.quadrant());
    }
    return quadrant;
  }

  NODISCARD auto isotach() const -> std::vector<double> {
    std::vector<double> isotach;
    isotach.reserve(m_uvp.size());
    for (const auto &iter : m_uvp) {
      isotach.push_back(iter.isotach());
    }
    return isotach;
  }

 private:
  std::vector<Gahm::Datatypes::Uvp> m_uvp;
};
}  // namespace Gahm::Datatypes

#endif  // GAHM_VORTEXSOLUTION_H
