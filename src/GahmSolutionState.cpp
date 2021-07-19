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
#include "GahmSolutionState.h"

#include <cassert>

GahmSolutionState::GahmSolutionState(Atcf *atcf, std::vector<double> x_points,
                                     std::vector<double> y_points)
    : m_atcf(atcf),
      m_xpoints(std::move(x_points)),
      m_ypoints(std::move(y_points)),
      m_initialized(false),
      m_date1(1970, 1, 1),
      m_date2(1970, 1, 1) {
  assert(m_xpoints.size() == m_ypoints.size());
  m_distanceQuery.resize(m_xpoints.size());
  m_azimuthQuery.resize(m_ypoints.size());
}

void GahmSolutionState::query(const Date &d) {
  // if (d < m_date1 || d > m_date2 || !m_initialized) {
  this->generateUpdatedParameters(d);
  m_initialized = true;
  //}

  m_stormParametersQuery = m_atcf->getStormParameters(d);

  for (auto i = 0; i < m_xpoints.size(); ++i) {
    m_distanceQuery[i] = Interpolation::linearInterp(
        m_stormParametersQuery.wtratio(), m_distance1[i], m_distance2[i]);
    m_azimuthQuery[i] = Interpolation::angleInterp(
        m_stormParametersQuery.wtratio(), m_azimuth1[i], m_azimuth2[i]);
  }

  m_stormMotion =
      1.5 * std::pow(std::sqrt(std::pow(m_stormParametersQuery.utrans(), 2.0) +
                               std::pow(m_stormParametersQuery.vtrans(), 2.0)),
                     0.63);

  m_direction = std::atan2(m_stormParametersQuery.utrans(),
                           m_stormParametersQuery.vtrans());

  m_stormMotionU = std::sin(m_direction) * m_stormMotion;
  m_stormMotionV = std::cos(m_direction) * m_stormMotion;
}

void GahmSolutionState::generateUpdatedParameters(const Date &d) {
  auto cycle0 = m_atcf->getCycleNumber(d).first;
  auto cycle1 = cycle0 + 1;

  auto stormParameters1 = m_atcf->getStormParameters(cycle0, 1.0);
  auto stormParameters2 = m_atcf->getStormParameters(cycle1, 1.0);

  std::tie(m_distance1, m_azimuth1) = this->computeDistanceToStormCenter(
      stormParameters1.longitude(), stormParameters1.latitude());
  std::tie(m_distance2, m_azimuth2) = this->computeDistanceToStormCenter(
      stormParameters2.longitude(), stormParameters2.latitude());
}

std::tuple<std::vector<double>, std::vector<double>>
GahmSolutionState::computeDistanceToStormCenter(const double stormCenterX,
                                                const double stormCenterY) {
  std::vector<double> distance, azimuth;
  distance.reserve(m_xpoints.size());
  azimuth.reserve(m_ypoints.size());

  constexpr double deg2rad = Units::convert(Units::Degree, Units::Radian);

  for (auto i = 0; i < m_xpoints.size(); ++i) {
    double radius_earth = Constants::radiusEarth(m_ypoints[i], stormCenterY);
    double dx = deg2rad * radius_earth * (m_xpoints[i] - stormCenterX) *
                std::cos(deg2rad * stormCenterY);
    double dy = deg2rad * radius_earth * (m_ypoints[i] - stormCenterY);
    distance.push_back(std::sqrt(dx * dx + dy * dy));

    double azi = std::atan2(dx, dy);

    assert(azi >= -Constants::pi());
    assert(azi <= Constants::pi());

    azimuth.push_back(azi);
  }
  return std::make_tuple(distance, azimuth);
}

double GahmSolutionState::distance(size_t index) const {
  assert(m_initialized);
  assert(index < m_distanceQuery.size());
  return m_distanceQuery[index];
}

double GahmSolutionState::azimuth(size_t index) const {
  assert(m_initialized);
  assert(index < m_azimuthQuery.size());
  return m_azimuthQuery[index];
}
StormParameters GahmSolutionState::stormParameters() const {
  assert(m_initialized);
  return m_stormParametersQuery;
}

double GahmSolutionState::x(size_t index) const {
  assert(index < m_xpoints.size());
  return m_xpoints[index];
}

double GahmSolutionState::y(size_t index) const {
  assert(index < m_ypoints.size());
  return m_ypoints[index];
}

size_t GahmSolutionState::size() { return m_xpoints.size(); }

double GahmSolutionState::stormDirection() const { return m_direction; }

double GahmSolutionState::stormMotion() const { return m_stormMotion; }

double GahmSolutionState::stormMotionU() const { return m_stormMotionU; }

double GahmSolutionState::stormMotionV() const { return m_stormMotionV; }
