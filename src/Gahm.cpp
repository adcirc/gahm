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

  //..Generate storm parameters
  Vortex v1(m_atcf->record(sp.cycle()), m_assumptions.get());
  const size_t cycle2 =
      sp.cycle() + 1 > m_atcf->nRecords() - 1 ? sp.cycle() : sp.cycle() + 1;
  Vortex v2(m_atcf->record(cycle2), m_assumptions.get());

  GahmSolution g;
  g.reserve(m_state->size());

  for (auto i = 0; i < m_state->size(); ++i) {
    auto param = this->generateStormParameterPackForLocation(sp, v1, v2, i);

    auto phi = 1.0 + (param.vmaxBoundaryLayer * param.radiusToMaxWinds * km2m *
                      sp.corio()) /
                         (param.hollandB * param.vmaxBoundaryLayer *
                              param.vmaxBoundaryLayer +
                          param.vmaxBoundaryLayer * param.radiusToMaxWinds *
                              km2m * sp.corio());

    GahmSolutionPoint p =
        Gahm::getUvpr(m_state->distance(i), m_state->azimuth(i), param, phi,
                      m_state->stormMotionU(), m_state->stormMotionV(), sp);
    g.push_back(p);
  }

  return g;
}

Vortex::ParameterPack Gahm::generateStormParameterPackForLocation(
    const StormParameters &sp, const Vortex &v1, const Vortex &v2,
    int i) const {
  auto pack1 = v1.getParameters(m_state->azimuth(i), m_state->distance(i));
  auto pack2 = v2.getParameters(m_state->azimuth(i), m_state->distance(i));
  double rmax = Interpolation::linearInterp(
      sp.wtratio(), pack1.radiusToMaxWinds, pack2.radiusToMaxWinds);
  double rmaxtrue = Interpolation::linearInterp(
      sp.wtratio(), pack1.radiusToMaxWindsTrue, pack2.radiusToMaxWindsTrue);
  double b =
      Interpolation::linearInterp(sp.wtratio(), pack1.hollandB, pack2.hollandB);
  double vmaxbl = Interpolation::linearInterp(
      sp.wtratio(), pack1.vmaxBoundaryLayer, pack2.vmaxBoundaryLayer);

  return {vmaxbl, rmax, rmaxtrue, b};
}

GahmSolutionPoint Gahm::getUvpr(const double distance, const double angle,
                                const Vortex::ParameterPack &pack,
                                const double phi, const double utrans,
                                const double vtrans, const StormParameters &s) {
  constexpr double km2m = Units::convert(Units::Kilometer, Units::Meter);

  if (distance < 1.0 * Units::convert(Units::NauticalMile, Units::Meter)) {
    return {0.0, 0.0, s.centralPressure()};
  }

  const double rmaxmeters = pack.radiusToMaxWinds * km2m;
  const double vmaxsq = pack.vmaxBoundaryLayer * pack.vmaxBoundaryLayer;
  const double vmaxrmax = pack.vmaxBoundaryLayer * rmaxmeters * s.corio();
  const double c = (distance * s.corio()) / 2.0;
  const double csq = c * c;
  const double rmaxdisb = std::pow(rmaxmeters / distance, pack.hollandB);

  double speed = std::sqrt((vmaxsq + vmaxrmax) * rmaxdisb *
                               std::exp(phi * (1.0 - rmaxdisb)) +
                           csq) -
                 c;

  const double speedOverVmax = std::abs(speed / pack.vmaxBoundaryLayer);
  const double tsx = speedOverVmax * utrans;
  const double tsy = speedOverVmax * vtrans;
  speed *= Constants::windReduction();
  const double u = -speed * std::cos(angle);
  const double v = speed * std::sin(angle);

  double uf, vf;
  std::tie(uf, vf) = Vortex::rotateWinds(
      u, v,
      Constants::frictionAngle(distance, pack.radiusToMaxWindsTrue * km2m),
      s.latitude());

  uf = (uf + tsx) * Constants::one2ten();
  vf = (vf + tsy) * Constants::one2ten();

  const double p =
      s.centralPressure() + (s.backgroundPressure() - s.centralPressure()) *
                                std::exp(-phi * rmaxdisb);

  return {uf, vf, p};
}
