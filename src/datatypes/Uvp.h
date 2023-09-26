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
#ifndef GAHM_UVP_H
#define GAHM_UVP_H

#include <tuple>

#include "physical/Constants.h"

namespace Gahm::Datatypes {

class Uvp {
 public:
  Uvp()
      : m_u(0.0),
        m_v(0.0),
        m_p(Gahm::Physical::Constants::backgroundPressure()) {}
  Uvp(double u, double v, double p) : m_u(u), m_v(v), m_p(p) {}

  [[nodiscard]] double u() const { return m_u; }

  [[nodiscard]] double v() const { return m_v; }

  [[nodiscard]] double p() const { return m_p; }

  [[nodiscard]] std::tuple<double, double, double> get() const {
    return std::make_tuple(m_u, m_v, m_p);
  }

  void setU(double u) { m_u = u; }

  void setV(double v) { m_v = v; }

  void setP(double p) { m_p = p; }

  void set(double u, double v, double p) {
    m_u = u;
    m_v = v;
    m_p = p;
  }

  void set(const Uvp &uvp) {
    m_u = uvp.u();
    m_v = uvp.v();
    m_p = uvp.p();
  }

 private:
  double m_u;
  double m_v;
  double m_p;
};

}  // namespace Gahm::Datatypes

#endif  // GAHM_UVP_H
