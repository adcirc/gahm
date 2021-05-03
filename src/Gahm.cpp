// MIT License
//
// Copyright (c) 2020 ADCIRC Development Group
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//
// Author: Zach Cobell
// Contact: zcobell@thewaterinstitute.org
//
#include "Gahm.h"

#include <iostream>
#include <utility>

#include "Logging.h"
#include "Physical.h"
#include "Vortex.h"

#ifdef GAHM_USE_FAST_MATH
#define gahm_exp Physical::fast_exp
#else
#define gahm_exp std::exp
#endif

Gahm::Gahm(std::string filename) : m_filename(std::move(filename)) {}

std::string Gahm::filename() const { return m_filename; }

int Gahm::read() {
  this->m_atcf = std::make_unique<Atcf>(m_filename, &m_assumptions);
  int ierr = this->m_atcf->read();
  if (ierr != 0) {
    gahm_throw_exception("Could not read the ATCF file");
  }

  this->m_assumptions.log(Assumption::MINOR);

  return 0;
}

int Gahm::get(const Date &d, const std::vector<double> &x,
              const std::vector<double> &y, std::vector<double> &u,
              std::vector<double> &v, std::vector<double> &p) {
  const Atcf::StormParameters sp = m_atcf->getStormParameters(d);
  const double stormMotion =
      1.5 *
      std::pow(std::sqrt(std::pow(sp.utrans, 2.0) + std::pow(sp.vtrans, 2.0)),
               0.63);

  const auto directionNow = [sp]() {
    double d = std::atan2(sp.utrans, sp.vtrans);
    if (d < 0.0) {
      d += Physical::twopi();
    }
    return d;
  }();

  const double stormMotionU = std::sin(directionNow) * stormMotion;
  const double stormMotionV = std::cos(directionNow) * stormMotion;

  Vortex vortex(sp.central_pressure, sp.background_pressure, sp.longitude,
                sp.latitude, sp.vmax);

  if (sp.cycle < 0) {
    for (size_t i = 0; i < m_atcf->crecord(0)->nIsotach(); ++i) {
      //      vortex.setRadii(i, m_atcf.record(sp.cycle),
      //                      m_atcf.record(sp.cycle).isotach(i)->radii(),
      //                      m_atcf.record(sp.cycle).isotach(i)->quadflag(), )
    }
  }

  return 0;
}

template <bool geofactor>
Gahm::uvp Gahm::getUvpr(const double distance, const double angle,
                        const double rmax, const double rmax_true,
                        const double p_c, const double p_background,
                        const double b, const double vmax, const double pmin,
                        const double phi, const double utrans,
                        const double coriolis, const double vtrans,
                        const double clat) {
  if (distance < 1.0 * Physical::nmi2km()) {
    return {0.0, 0.0, pmin};
  }

  const double percentCoriolis = 1.0;
  const double vmaxsq = std::pow(vmax, 2.0);
  const double vmaxrmax = vmax * rmax * percentCoriolis * coriolis;
  const double c = distance * percentCoriolis * coriolis / 2.0;
  const double csq = std::pow(c, 2.0);
  const double rmaxdisb = std::pow(rmax / distance, b);

  double speed =
      geofactor
          ? std::sqrt((vmaxsq + vmaxrmax) * rmaxdisb *
                          gahm_exp(phi * (1.0 - rmaxdisb)) +
                      csq) -
                c
          : std::sqrt(vmaxsq * rmaxdisb * gahm_exp(1.0 - rmaxdisb) + csq) - c;

  const double speedOverVmax = std::abs(speed / vmax);
  const double tsx = speedOverVmax * utrans;
  const double tsy = speedOverVmax * vtrans;
  speed *= Physical::windReduction();
  const double u = -speed * std::cos(angle);
  const double v = speed * std::sin(angle);

  double uf, vf;
  std::tie(uf, vf) = Vortex::rotateWinds(
      u, v, Vortex::frictionAngle(distance, rmax_true), clat);

  uf = (uf + tsx) * Physical::one2ten();
  vf = (vf + tsy) * Physical::one2ten();

  const double p = geofactor
                       ? p_c + (p_background - p_c) * gahm_exp(-phi * rmaxdisb)
                       : p_c + (p_background - p_c) * gahm_exp(-rmaxdisb);

  return {uf, vf, p};
}
