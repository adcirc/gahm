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

#include <cassert>
#include <numeric>

#include "Logging.h"
#include "Physical.h"

Vortex::Vortex(double pinf, double p0, double lon, double lat, double vmax)
    : m_pn(pinf),
      m_pc(p0),
      m_cLat(lat),
      m_cLon(lon),
      m_vMax(vmax),
      m_corio(Vortex::coriolis(lat)),
      m_vr(0.0),
      m_rMaxes4(Vortex::makeWindArray(0.0)),
      m_b(Vortex::makeWindArray(Vortex::calcHollandB(m_vMax, m_pc, m_pn))),
      m_phi(Vortex::makeWindArray(1.0)),
      m_vmBL4(Vortex::makeWindArray(0.0)),
      m_quadFlag4(Vortex::makeWindArray(0)),
      m_quadIr4(Vortex::makeWindArray(0.0)) {}

void Vortex::setVortex(double pinf, double p0, double lon, double lat,
                       double vmax) noexcept {
  this->m_pn = pinf;
  this->m_pc = p0;
  this->m_cLat = lat;
  this->m_cLon = lon;
  this->m_vMax = vmax;
  this->m_corio = Vortex::coriolis(lat);
}

double Vortex::frictionAngle(const double r, const double rmx) noexcept {
  if (0.0 < r && r < rmx) {
    return 10.0 * r / rmx;
  } else if (rmx <= r && r < 1.2 * rmx) {
    return 10.0 + 75.0 * (r / rmx - 1.0);
  } else if (r >= 1.2 * rmx) {
    return 25.0;
  } else {
    return 0.0;
  }
}

std::pair<double, double> Vortex::rotateWinds(const double x, const double y,
                                              const double angle,
                                              const double whichWay) noexcept {
  double a = Physical::deg2rad() * angle;
  if (whichWay >= 0.0) a *= -1.0;
  const double cosA = std::cos(a);
  const double sinA = std::sin(a);
  return {x * cosA - y * sinA, x * sinA + y * cosA};
}

std::pair<int, double> Vortex::getBaseQuadrant(const double angle) {
  assert(angle <= 360.0);
  assert(angle >= 0.0);
  if (angle <= 45.0) {
    return std::make_pair(5, 45.0 + angle);
  } else if (angle <= 135.0) {
    return std::make_pair(2, angle - 45.0);
  } else if (angle <= 225.0) {
    return std::make_pair(3, angle - 135.0);
  } else if (angle <= 315.0) {
    return std::make_pair(4, angle - 225.0);
  } else if (angle > 315.0) {
    return std::make_pair(5, angle - 315.0);
  }
  gahm_throw_exception("Invalid angle provided!");
  return std::make_pair(0, 0.0);
}

double Vortex::spInterp(const std::array<std::array<double, Vortex::nQuads>,
                                         Vortex::nPoints> &array,
                        const double angle, const double distance) const {
  int base_quadrant = 0;
  double delta_angle = 0.0;
  std::tie(base_quadrant, delta_angle) = this->getBaseQuadrant(angle);
  if (delta_angle < 1.0) {
    return this->interpR(array, base_quadrant - 1, distance);
  } else if (delta_angle > 89.0) {
    return this->interpR(array, base_quadrant, distance);
  } else {
    const double t1 = this->interpR(array, base_quadrant - 1, distance);
    const double t2 = this->interpR(array, base_quadrant, distance);
    return (t1 / std::pow(delta_angle, 2.0) +
            t2 / std::pow(90.0 - delta_angle, 2.0)) /
           (1.0 / std::pow(delta_angle, 2.0) +
            1.0 / std::pow(90.0 - delta_angle, 2.0));
  }
}

double Vortex::interpR(const WindArray<double> &array, const int quad,
                       const double r) const {
  const auto total_isotachs =
      std::accumulate(m_quadFlag4[quad].begin(), m_quadFlag4[quad].end(), 0);
  if (r < m_quadIr4[quad].front()) return array[quad][0];
  if (r > m_quadIr4[quad].back()) return array[quad].back();
  for (auto i = 0; i < total_isotachs; ++i) {
    if (r > m_quadIr4[quad][i]) {
      const double fac = (r - m_quadIr4[quad][i]) /
                         (m_quadIr4[quad][i - 1] - m_quadIr4[quad][i]);
      return fac * array[quad][i - 1] + (1.0 - fac) * array[quad][i];
    }
  }
  return array[quad].back();
}

constexpr double Vortex::coriolis(const double lat) noexcept {
  return Physical::omega() * Physical::deg2rad() * lat;
}

constexpr double Vortex::calcHollandB(double vmax, double p0,
                                      double pinf) noexcept {
  return std::max(std::min(vmax * vmax * Physical::rhoAir() * Physical::e() /
                               Physical::mb2pa() * (p0 - pinf),
                           2.50),
                  1.0);
}

void Vortex::setRadii(size_t index, std::array<double, nQuads> quadFlag,
                      std::array<double, nQuads> rmax,
                      std::array<double, nQuads> quadIr,
                      std::array<double, nQuads> b,
                      std::array<double, nQuads> vmbl) {
  for (size_t i = 0; i < nQuads; ++i) {
    m_quadFlag4[index][i] = quadFlag[i];
    m_rMaxes4[index][i] = rmax[i];
    m_quadIr4[index][i] = quadIr[i];
    m_b[index][i] = b[i];
    m_vmBL4[index][i] = vmbl[i];
  }
}

void Vortex::fillRadii() {
  Vortex::fillEdges(m_quadFlag4);
  Vortex::fillEdges(m_quadIr4);
  Vortex::fillEdges(m_rMaxes4);
  Vortex::fillEdges(m_b);
  Vortex::fillEdges(m_phi);
  Vortex::fillEdges(m_vmBL4);
}
