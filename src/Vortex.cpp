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
#include "Vortex.h"

#include <algorithm>
#include <cassert>
#include <utility>

#include "GahmSolver.h"
#include "Interpolation.h"
#include "Logging.h"
#include "Physical.h"

using namespace Gahm;

Vortex::Vortex(AtcfLine *atcf, std::shared_ptr<Assumptions> assumptions)
    : m_stormData(atcf), m_assumptions(std::move(assumptions)) {
  if (!m_assumptions) m_assumptions = std::make_shared<Assumptions>();
}

void Vortex::setStormData(AtcfLine *atcf) { m_stormData = atcf; }

std::pair<double, double> Vortex::rotateWinds(const double x, const double y,
                                              double angle,
                                              const double latitude) noexcept {
  if (latitude < 0.0) angle *= -1.0;
  const double cosA = gahm_cos(angle);
  const double sinA = gahm_sin(angle);
  return std::make_pair(x * cosA - y * sinA, x * sinA + y * cosA);
}

std::pair<int, double> Vortex::getBaseQuadrant(double angle) {
  auto angle2 = angle + Constants::quarterpi();
  if (angle2 > Constants::twopi()) angle2 -= Constants::twopi();
  auto quad = std::fmod(angle2 / Constants::halfpi(), 4);
  const double rem = angle2 - quad * Constants::halfpi();
  return {quad, rem};
}

ParameterPack Vortex::getParameters(const double angle,
                                    const double distance) const {
  static constexpr double deg2rad =
      Units::convert(Units::Degree, Units::Radian);
  static constexpr double angle_1 = deg2rad;
  static constexpr double angle_89 = 89.0 * deg2rad;
  static constexpr double angle_90 = 90.0 * deg2rad;

  const auto [base_quadrant, delta_angle] = Vortex::getBaseQuadrant(angle);

  if (delta_angle < angle_1) {
    return this->interpolateParameters(base_quadrant - 1, distance,
                                       delta_angle);
  } else if (delta_angle > angle_89) {
    return this->interpolateParameters(base_quadrant, distance, delta_angle);
  } else {
    auto pack1 =
        this->interpolateParameters(base_quadrant - 1, distance, delta_angle);
    auto pack2 =
        this->interpolateParameters(base_quadrant, distance, delta_angle);
    auto d1 = 1.0 / (delta_angle * delta_angle);
    auto d2 = 1.0 / ((angle_90 - delta_angle) * (angle_90 - delta_angle));

    double rmax = Interpolation::quadrantInterp(
        d1, d2, pack1.radiusToMaxWinds(), pack2.radiusToMaxWinds());
    double rmaxtrue = Interpolation::quadrantInterp(
        d1, d2, pack1.radiusToMaxWindsTrue(), pack2.radiusToMaxWindsTrue());
    double vmaxbl = Interpolation::quadrantInterp(
        d1, d2, pack1.vmaxBoundaryLayer(), pack2.vmaxBoundaryLayer());
    double b = Interpolation::quadrantInterp(d1, d2, pack1.hollandB(),
                                             pack2.hollandB());
    double isorad = Interpolation::quadrantInterp(d1, d2, pack1.isotachRadius(),
                                                  pack2.isotachRadius());
    return {vmaxbl, rmax, rmaxtrue, b, pack1.isotachSpeed(), isorad};
  }
}

ParameterPack Vortex::interpolateParameters(int quad, double distance,
                                            double angle) const {
  const auto radii = m_stormData->isotachRadii(quad);

  if (radii->empty()) {
    return {m_stormData->vmax(),           m_stormData->radiusMaxWinds(),
            m_stormData->radiusMaxWinds(), m_stormData->hollandB(),
            m_stormData->vmax(),           m_stormData->radiusMaxWinds()};
  } else if (distance >= radii->front()) {
    return m_stormData->isotach(0)->parameterPack(quad);
  } else if (distance <= radii->back()) {
    return m_stormData->isotach(radii->size() - 1)->parameterPack(quad);
  } else {
    const auto it =
        std::upper_bound(radii->rbegin(), radii->rend(), distance).base();
    const auto pos1 = it - radii->begin();
    const auto pos2 = pos1 - 1;
    const auto r1 = *(it);
    const auto r2 = *(it - 1);
    const double fac = (distance - r1) / (r2 - r1);
    const auto p1 = m_stormData->isotach(pos1)->parameterPack(quad);
    const auto p2 = m_stormData->isotach(pos2)->parameterPack(quad);
    double vmbl = Interpolation::linearInterp(fac, p1.vmaxBoundaryLayer(),
                                              p2.vmaxBoundaryLayer());
    double rmax = Interpolation::linearInterp(fac, p1.radiusToMaxWinds(),
                                              p2.radiusToMaxWinds());
    double b = Interpolation::linearInterp(fac, p1.hollandB(), p2.hollandB());
    double rmaxtrue = Interpolation::linearInterp(
        fac, p1.radiusToMaxWindsTrue(), p2.radiusToMaxWindsTrue());
    double isorad = Interpolation::linearInterp(fac, p1.isotachRadius(),
                                                p2.isotachRadius());
    return {vmbl, rmax, rmaxtrue, b, p1.isotachSpeed(), isorad};
  }
}

Date Vortex::datetime() const { return m_stormData->datetime(); }

void Vortex::computeRadiusToMaxWind() {
  for (auto iso = 0; iso < m_stormData->nIsotach(); ++iso) {
    for (auto quad = 0; quad < 4; ++quad) {
      GahmSolver g(
          m_stormData->isotach(iso)->isotach_radius().at(quad),
          m_stormData->isotach(iso)->quadrant_isotach_wind_speed().at(quad),
          m_stormData->vmaxAtBoundaryLayer(), m_stormData->centralPressure(),
          Physical::backgroundPressure(), m_stormData->lat());
      g.solve();
      m_stormData->isotach(iso)->quadrant_radius_to_max_winds().set(quad,
                                                                    g.rmax());
      m_stormData->isotach(iso)->quadrant_holland_b().set(quad, g.bg());
      m_stormData->isotach(iso)->quadrant_phi().set(quad, g.phi());
    }
  }
}
