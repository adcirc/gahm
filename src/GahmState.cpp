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
#include "GahmState.h"

#include <cassert>

GahmState::GahmState(Atcf *atcf, std::vector<double> x_points,
                     std::vector<double> y_points)
    : m_atcf(atcf),
      m_xpoints(std::move(x_points)),
      m_ypoints(std::move(y_points)),
      m_distance(m_xpoints.size(), 0.0),
      m_azimuth(m_xpoints.size(), 0.0) {
  assert(m_xpoints.size() == m_ypoints.size());
}

void GahmState::query(const Date &d) {
  m_stormParametersQuery = m_atcf->getStormParameters(d);
  this->computeDistanceToStormCenter(m_stormParametersQuery.longitude(),
                                     m_stormParametersQuery.latitude());

  m_stormMotion =
      1.5 * std::pow(std::sqrt(std::pow(m_stormParametersQuery.utrans(), 2.0) +
                               std::pow(m_stormParametersQuery.vtrans(), 2.0)),
                     0.63);

  m_direction = std::atan2(m_stormParametersQuery.utrans(),
                           m_stormParametersQuery.vtrans());

  m_stormMotionU = std::sin(m_direction) * m_stormMotion;
  m_stormMotionV = std::cos(m_direction) * m_stormMotion;
}

void GahmState::computeDistanceToStormCenter(const double stormCenterX,
                                             const double stormCenterY) {
  constexpr double deg2rad = Units::convert(Units::Degree, Units::Radian);

  for (auto i = 0; i < m_xpoints.size(); ++i) {
    double radius_earth = Constants::radiusEarth(m_ypoints[i], stormCenterY);
    double dx = deg2rad * radius_earth * (m_xpoints[i] - stormCenterX) *
                std::cos(deg2rad * stormCenterY);
    double dy = deg2rad * radius_earth * (m_ypoints[i] - stormCenterY);
    m_distance[i] = std::sqrt(dx * dx + dy * dy);

    double azi = std::atan2(dx, dy);

    assert(azi >= -Constants::pi());
    assert(azi <= Constants::pi());

    m_azimuth[i] = azi;
  }
}

double GahmState::distance(size_t index) const {
  assert(m_initialized);
  assert(index < m_distance.size());
  return m_distance[index];
}

double GahmState::azimuth(size_t index) const {
  assert(m_initialized);
  assert(index < m_azimuth.size());
  return m_azimuth[index];
}
StormParameters GahmState::stormParameters() const {
  assert(m_initialized);
  return m_stormParametersQuery;
}

double GahmState::x(size_t index) const {
  assert(index < m_xpoints.size());
  return m_xpoints[index];
}

double GahmState::y(size_t index) const {
  assert(index < m_ypoints.size());
  return m_ypoints[index];
}

size_t GahmState::size() { return m_xpoints.size(); }

double GahmState::stormDirection() const { return m_direction; }

double GahmState::stormMotion() const { return m_stormMotion; }

double GahmState::stormMotionU() const { return m_stormMotionU; }

double GahmState::stormMotionV() const { return m_stormMotionV; }
