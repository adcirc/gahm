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

#ifdef SWIG
#define NODISCARD
#else
#define NODISCARD [[nodiscard]]
#endif

namespace Gahm::Datatypes {

class Uvp {
 public:
  Uvp()
      : m_u(0.0),
        m_v(0.0),
        m_p(Gahm::Physical::Constants::backgroundPressure()),
        m_azimuth(0.0),
        m_distance(0.0),
        m_tsx(0.0),
        m_tsy(0.0),
        m_quadrant_weight(0.0),
        m_isotach_weight(0.0),
        m_quadrant(0),
        m_isotach(0) {}

  Uvp(double u, double v, double p)
      : m_u(u),
        m_v(v),
        m_p(p),
        m_azimuth(0.0),
        m_distance(0.0),
        m_tsx(0.0),
        m_tsy(0.0),
        m_quadrant_weight(0.0),
        m_isotach_weight(0.0),
        m_quadrant(0),
        m_isotach(0) {}

  Uvp(double u, double v, double p, double azimuth, double distance, double tsx,
      double tsy, double quadrant_weight, double isotach_weight, int quadrant,
      int isotach)
      : m_u(u),
        m_v(v),
        m_p(p),
        m_azimuth(azimuth),
        m_distance(distance),
        m_tsx(tsx),
        m_tsy(tsy),
        m_quadrant_weight(quadrant_weight),
        m_isotach_weight(isotach_weight),
        m_quadrant(quadrant),
        m_isotach(isotach) {}

  NODISCARD auto u() const -> double { return m_u; }

  NODISCARD auto v() const -> double { return m_v; }

  NODISCARD auto p() const -> double { return m_p; }

  NODISCARD auto azimuth() const -> double { return m_azimuth; }

  NODISCARD auto distance() const -> double { return m_distance; }

  NODISCARD auto tsx() const -> double { return m_tsx; }

  NODISCARD auto tsy() const -> double { return m_tsy; }

  NODISCARD auto quadrant_weight() const -> double { return m_quadrant_weight; }

  NODISCARD auto isotach_weight() const -> double { return m_isotach_weight; }

  NODISCARD auto quadrant() const -> int { return m_quadrant; }

  NODISCARD auto isotach() const -> int { return m_isotach; }

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

  double m_azimuth;
  double m_distance;
  double m_tsx;
  double m_tsy;
  double m_quadrant_weight;
  double m_isotach_weight;
  int m_quadrant;
  int m_isotach;
};

}  // namespace Gahm::Datatypes

#endif  // GAHM_UVP_H
