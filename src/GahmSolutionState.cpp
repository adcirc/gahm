//
// Created by Zach Cobell on 7/6/21.
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
  if (d < m_date1 || d > m_date2 || !m_initialized) {
    this->generateUpdatedParameters(d);
    m_initialized = true;
  }

  m_stormParametersQuery = m_atcf->getStormParameters(d);

  for (auto i = 0; i < m_xpoints.size(); ++i) {
    m_distanceQuery[i] = Interpolation::linearInterp(
        m_stormParametersQuery.wtratio(), m_distance1[i], m_distance2[2]);
    m_azimuthQuery[i] = Interpolation::angleInterp(
        m_stormParametersQuery.wtratio(), m_azimuth1[i], m_azimuth2[i]);
  }
}

void GahmSolutionState::generateUpdatedParameters(const Date &d) {
  auto cycle0 = m_atcf->getCycleNumber(d).first;
  auto cycle1 = cycle0 + 1;

  m_date1 = m_atcf->crecord(cycle0)->datetime();
  m_date2 = m_atcf->crecord(cycle1)->datetime();

  auto stormParameters1 = m_atcf->getStormParameters(m_date1);
  auto stormParameters2 = m_atcf->getStormParameters(m_date2);

  auto v1 = this->computeDistanceToStormCenter(stormParameters1.longitude(),
                                               stormParameters1.latitude());
  auto v2 = this->computeDistanceToStormCenter(stormParameters2.longitude(),
                                               stormParameters2.latitude());
  m_distance1 = std::get<0>(v1);
  m_azimuth1 = std::get<1>(v1);
  m_distance2 = std::get<0>(v2);
  m_azimuth2 = std::get<1>(v2);
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
    distance.push_back(std::sqrt(dx * dx + dy * dy) *
                       Units::convert(Units::Meter, Units::Kilometer));
    double azi = std::atan2(dx, dy);

    if (azi < 0.0) {
      azi += Constants::twopi();
    }
    azimuth.push_back(azi);
  }
  return std::make_tuple(distance, azimuth);
}

double GahmSolutionState::distance(size_t index) {
  assert(m_initialized);
  assert(index < m_distanceQuery.size());
  return m_distanceQuery[index];
}

double GahmSolutionState::azimuth(size_t index) {
  assert(m_initialized);
  assert(index < m_azimuthQuery.size());
  return m_azimuthQuery[index];
}
StormParameters GahmSolutionState::stormParameters() {
  assert(m_initialized);
  return m_stormParametersQuery;
}

double GahmSolutionState::x(size_t index) {
  assert(index < m_xpoints.size());
  return m_xpoints[index];
}

double GahmSolutionState::y(size_t index) {
  assert(index < m_ypoints.size());
  return m_ypoints[index];
}

size_t GahmSolutionState::size() { return m_xpoints.size(); }
