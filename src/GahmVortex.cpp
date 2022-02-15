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
#include <utility>

#include "Assumptions.h"
#include "Constants.h"
#include "Gahm.h"
#include "Interpolation.h"
#include "Logging.h"
#include "Vortex.h"

using namespace Gahm;

GahmVortex::GahmVortex(std::string filename, const std::vector<double> &x,
                       const std::vector<double> &y, Atcf::AtcfFormat format)
    : m_filename(std::move(filename)),
      m_assumptions(std::make_shared<Assumptions>()),
      m_atcf(std::make_unique<Atcf>(m_filename, format, m_assumptions)),
      m_state(std::make_unique<GahmState>(m_atcf.get(), x, y)) {
  int ierr = this->m_atcf->read();
  if (ierr != 0) {
    gahm_throw_exception("Could not read the ATCF file");
  }
  this->m_preprocessor = std::make_unique<Preprocessor>(m_atcf.get());
  if (format == Atcf::AtcfFormat::BEST_TRACK) this->m_preprocessor->run();
}

std::string GahmVortex::filename() const { return m_filename; }

Assumptions *GahmVortex::assumptions() { return m_assumptions.get(); }

WindData GahmVortex::get(const Date &d) {
  WindData g(m_state->size(), {0.0, 0.0, Physical::backgroundPressure()});

  if (d < m_atcf->begin_time() || d > m_atcf->end_time()) {
    return g;
  }

  m_state->query(d);
  const auto sp = m_state->stormParameters();
  g.setStormParameters(sp);

  //...TODO: Need to add handling for 'CALM' type
  //   Should just set the storm position, velocities to zero, and
  //   pressure to background (Constants::backgroundPressure)

  //..Generate storm parameters
  const Vortex v1(m_atcf->record(sp.cycle()), m_assumptions);
  const size_t cycle2 =
      sp.cycle() + 1 > m_atcf->nRecords() - 1 ? sp.cycle() : sp.cycle() + 1;
  const Vortex v2(m_atcf->record(cycle2), m_assumptions);

  for (auto i = 0; i < m_state->size(); ++i) {
    auto param = this->generateStormParameterPackForLocation(sp, v1, v2, i);
    auto uvp = GahmVortex::getUvpr(m_state->distance(i), m_state->azimuth(i),
                                   param, m_state->stormMotionU(),
                                   m_state->stormMotionV(), sp);
    g.set(i, uvp);
  }

  return g;
}

constexpr double GahmVortex::computePhi(const ParameterPack &p,
                                        const double corio) {
  constexpr double km2m = Units::convert(Units::Kilometer, Units::Meter);
  return 1.0 +
         (p.vmaxBoundaryLayer() * p.radiusToMaxWinds() * km2m * corio) /
             (p.hollandB() * p.vmaxBoundaryLayer() * p.vmaxBoundaryLayer() +
              p.vmaxBoundaryLayer() * p.radiusToMaxWinds() * km2m * corio);
}

ParameterPack GahmVortex::generateStormParameterPackForLocation(
    const StormParameters &sp, const Vortex &v1, const Vortex &v2,
    int i) const {
  auto pack1 = v1.getParameters(m_state->azimuth(i), m_state->distance(i));
  auto pack2 = v2.getParameters(m_state->azimuth(i), m_state->distance(i));
  double rmax = Interpolation::linearInterp(
      sp.wtratio(), pack1.radiusToMaxWinds(), pack2.radiusToMaxWinds());
  double rmaxtrue = Interpolation::linearInterp(
      sp.wtratio(), pack1.radiusToMaxWindsTrue(), pack2.radiusToMaxWindsTrue());
  double b = Interpolation::linearInterp(sp.wtratio(), pack1.hollandB(),
                                         pack2.hollandB());
  double vmaxbl = Interpolation::linearInterp(
      sp.wtratio(), pack1.vmaxBoundaryLayer(), pack2.vmaxBoundaryLayer());
  return {vmaxbl, rmax, rmaxtrue, b};
}

Gahm::Uvp GahmVortex::getUvpr(const double distance, const double angle,
                              const ParameterPack &pack, const double utrans,
                              const double vtrans, const StormParameters &s) {
  constexpr double km2m = Units::convert(Units::Kilometer, Units::Meter);
  constexpr double deg2rad = Units::convert(Units::Radian, Units::Degree);
  constexpr double thresholdRadius =
      1.0 * Units::convert(Units::NauticalMile, Units::Meter);

  if (distance < thresholdRadius) {
    return {0.0, 0.0, s.centralPressure()};
  }

  const double phi = GahmVortex::computePhi(pack, s.corio());
  const double rmaxmeters = pack.radiusToMaxWinds() * km2m;
  const double vmaxsq = pack.vmaxBoundaryLayer() * pack.vmaxBoundaryLayer();
  const double vmaxrmax = pack.vmaxBoundaryLayer() * rmaxmeters * s.corio();
  const double c = (distance * s.corio()) / 2.0;
  const double csq = c * c;
  const double rmaxdisb = std::pow(rmaxmeters / distance, pack.hollandB());

  double speed = std::sqrt((vmaxsq + vmaxrmax) * rmaxdisb *
                               std::exp(phi * (1.0 - rmaxdisb)) +
                           csq) -
                 c;

  const double speedOverVmax = std::abs(speed / pack.vmaxBoundaryLayer());
  const double tsx = speedOverVmax * utrans;
  const double tsy = speedOverVmax * vtrans;
  speed *= Physical::windReduction();
  const double u = -speed * std::cos(angle);
  const double v = speed * std::sin(angle);

  const double friction_angle =
      Physical::frictionAngle(distance, pack.radiusToMaxWindsTrue() * km2m);

  double uf, vf;
  std::tie(uf, vf) = Vortex::rotateWinds(u, v, friction_angle, s.latitude());

  uf = (uf + tsx) * Physical::one2ten();
  vf = (vf + tsy) * Physical::one2ten();

  const double p =
      s.centralPressure() + (s.backgroundPressure() - s.centralPressure()) *
                                std::exp(-phi * rmaxdisb);
  return {uf, vf, p};
}

Atcf *GahmVortex::atcf() { return m_atcf.get(); }
