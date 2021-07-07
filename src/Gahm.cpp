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

#include <utility>

#include "Assumptions.h"
#include "Constants.h"
#include "Interpolation.h"
#include "Logging.h"
#include "Vortex.h"

Gahm::Gahm(std::string filename, const std::vector<double> &x,
           const std::vector<double> &y)
    : m_filename(std::move(filename)),
      m_assumptions(std::make_unique<Assumptions>()),
      m_atcf(std::make_unique<Atcf>(m_filename, m_assumptions.get())),
      m_state(std::make_unique<GahmSolutionState>(m_atcf.get(), x, y)) {
  int ierr = this->m_atcf->read();
  if (ierr != 0) {
    gahm_throw_exception("Could not read the ATCF file");
  }
  this->m_preprocessor =
      std::make_unique<Preprocessor>(m_atcf->data(), m_assumptions.get());
  this->m_preprocessor->run();
}

std::string Gahm::filename() const { return m_filename; }

Assumptions *Gahm::assumptions() { return m_assumptions.get(); }

GahmSolution Gahm::get(const Date &d) {
  constexpr double km2m = Units::convert(Units::Kilometer, Units::Meter);

  m_state->query(d);
  auto sp = m_state->stormParameters();

  //...TODO: Need to add handling for 'CALM' type
  //   Should just set the storm position, velocities to zero, and
  //   pressure to background (Constants::backgroundPressure)

  const double stormMotion =
      1.5 * std::pow(std::sqrt(std::pow(sp.utrans(), 2.0) +
                               std::pow(sp.vtrans(), 2.0)),
                     0.63);

  const auto directionNow = [sp]() {
    double d = std::atan2(sp.utrans(), sp.vtrans());
    if (d < 0.0) {
      d += Constants::twopi();
    }
    return d;
  }();

  const double stormMotionU = std::sin(directionNow) * stormMotion;
  const double stormMotionV = std::cos(directionNow) * stormMotion;

  //..Generate storm parameters
  Vortex v1(m_atcf->record(sp.cycle()), m_assumptions.get());
  const size_t cycle2 =
      sp.cycle() + 1 > m_atcf->nRecords() - 1 ? sp.cycle() : sp.cycle() + 1;
  Vortex v2(m_atcf->record(cycle2), m_assumptions.get());

  GahmSolution g;
  g.reserve(m_state->size());

  for (auto i = 0; i < m_state->size(); ++i) {

    double rmax1, rmax2;
    double vmaxbl1, vmaxbl2;
    double b1, b2;
    double rmaxtrue1, rmaxtrue2;

    std::tie(vmaxbl1, rmax1, rmaxtrue1, b1) =
        v1.getParameters(m_state->azimuth(i), m_state->distance(i));
    std::tie(vmaxbl2, rmax2, rmaxtrue2, b2) =
        v2.getParameters(m_state->azimuth(i), m_state->distance(i));

    double rmax = Interpolation::linearInterp(sp.wtratio(), rmax1, rmax2);
    double rmaxtrue =
        Interpolation::linearInterp(sp.wtratio(), rmaxtrue1, rmaxtrue2);
    double b = Interpolation::linearInterp(sp.wtratio(), b1, b2);
    double vmaxbl = Interpolation::linearInterp(sp.wtratio(), vmaxbl1, vmaxbl2);

    auto phi =
        1.0 + (vmaxbl * rmax * km2m * sp.corio()) /
                  (b * vmaxbl * vmaxbl + vmaxbl * rmax * km2m * sp.corio());

    GahmSolutionPoint p =
        Gahm::getUvpr(m_state->distance(i), m_state->azimuth(i), rmax, rmaxtrue,
                      b, vmaxbl, phi, stormMotionU, stormMotionV, sp);
    g.push_back(std::move(p));
  }

  return g;
}

GahmSolutionPoint Gahm::getUvpr(const double distance, const double angle,
                                const double rmax, const double rmax_true,
                                const double b, const double vmax,
                                const double phi, const double utrans,
                                const double vtrans, const StormParameters &s) {
  if (distance <
               1.0 * Units::convert(Units::NauticalMile, Units::Meter)) {
    return {0.0, 0.0, s.centralPressure()};
  }

  const double vmaxsq = std::pow(vmax, 2.0);
  const double vmaxrmax = vmax * rmax * s.corio();
  const double c = distance * s.corio() / 2.0;
  const double csq = std::pow(c, 2.0);
  const double rmaxdisb = std::pow(rmax / distance, b);

  double speed = std::sqrt((vmaxsq + vmaxrmax) * rmaxdisb *
                               std::exp(phi * (1.0 - rmaxdisb)) +
                           csq) -
                 c;

  const double speedOverVmax = std::abs(speed / vmax);
  const double tsx = speedOverVmax * utrans;
  const double tsy = speedOverVmax * vtrans;
  speed *= Constants::windReduction();
  const double u = -speed * std::cos(angle);
  const double v = speed * std::sin(angle);

  double uf, vf;
  std::tie(uf, vf) = Vortex::rotateWinds(
      u, v, Constants::frictionAngle(distance, rmax_true), s.latitude());

  uf = (uf + tsx) * Constants::one2ten();
  vf = (vf + tsy) * Constants::one2ten();

  const double p =
      s.centralPressure() + (s.backgroundPressure() - s.centralPressure()) *
                                std::exp(-phi * rmaxdisb);

  return {uf, vf, p};
}
