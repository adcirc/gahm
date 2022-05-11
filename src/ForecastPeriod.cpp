//
// Created by Zach Cobell on 5/8/22.
//

#include "ForecastPeriod.h"

#include <utility>

#include "GahmSolver.h"
#include "Logging.h"
#include "boost/algorithm/string.hpp"

using namespace Gahm;

ForecastPeriod::ForecastPeriod()
    : m_date(1970, 1, 1),
      m_stormPosition(0, 0, 0, 0),
      m_storm_max_wind_velocity(0.0),
      m_radius_to_max_winds(0.0),
      m_central_pressure(Physical::backgroundPressure()),
      m_vmax_at_boundary_layer(0.0),
      m_background_pressure(Physical::backgroundPressure()),
      m_stormId(0),
      m_basin(BASIN::NONE),
      m_stormName("NONE") {}

ForecastPeriod::ForecastPeriod(BASIN basin, const Gahm::Date &date,
                               double longitude, double latitude,
                               double forward_speed, double transit_direction,
                               double radius_to_max_winds,
                               double max_wind_velocity, double centralPressure,
                               double backgroundPressure, int stormId,
                               std::string storm_name)
    : m_basin(basin),
      m_date(date),
      m_central_pressure(centralPressure),
      m_background_pressure(backgroundPressure),
      m_radius_to_max_winds(radius_to_max_winds),
      m_storm_max_wind_velocity(max_wind_velocity),
      m_stormPosition(
          longitude, latitude, forward_speed,
          transit_direction * Units::convert(Units::Degree, Units::Radian)),
      m_vmax_at_boundary_layer(
          ForecastPeriod::compute_simple_vmax_at_boundary_layer(
              m_storm_max_wind_velocity, m_stormPosition.transitSpeed())),
      m_stormId(stormId),
      m_stormName(std::move(storm_name)) {}

size_t ForecastPeriod::nIsotachs() const { return m_isotachs.size(); }

double ForecastPeriod::centralPressure() const { return m_central_pressure; }

double ForecastPeriod::backgroundPressure() const {
  return m_background_pressure;
}

const StormPosition &ForecastPeriod::stormPosition() const {
  return m_stormPosition;
}
StormPosition &ForecastPeriod::stormPosition() { return m_stormPosition; }

Date ForecastPeriod::date() const { return m_date; }

void ForecastPeriod::addIsotach(const StormIsotach &isotach) {
  //...Check if this isotach exists before adding it
  for (const auto &iso : m_isotachs) {
    if (iso.windSpeed() == isotach.windSpeed()) {
      Logging::warning(
          "Duplicate isotach found. Ignoring additional entries and keeping "
          "only first entry.");
      return;
    }
  }

  //...Add the isotach
  m_isotachs.push_back(isotach);
  m_isotachs.back().fillMissingData(m_radius_to_max_winds);

  //...Put the isotachs in order (They probably are, but this isn't done often)
  std::sort(m_isotachs.begin(), m_isotachs.end(), std::greater<>());
}

StormIsotach &ForecastPeriod::isotach(size_t index) {
  assert(index < m_isotachs.size());
  return m_isotachs[index];
}

const StormIsotach &ForecastPeriod::isotach(size_t index) const {
  assert(index < m_isotachs.size());
  return m_isotachs[index];
}

double ForecastPeriod::maximumWindVelocity() const {
  return m_storm_max_wind_velocity;
}

double ForecastPeriod::radiusToMaxWinds() const {
  return m_radius_to_max_winds;
}

std::tuple<double, double> ForecastPeriod::distanceAndAzimuth(
    double longitude, double latitude, double stormCenterX,
    double stormCenterY) {
  constexpr double deg2rad = Units::convert(Units::Degree, Units::Radian);
  const double radius_earth = Physical::radiusEarth(stormCenterY, latitude);
  const double cosStormCenterY = std::cos(deg2rad * stormCenterY);

  const double dx =
      deg2rad * radius_earth * (longitude - stormCenterX) * cosStormCenterY;
  const double dy = deg2rad * radius_earth * (latitude - stormCenterY);

  const double distance = std::sqrt(dx * dx + dy * dy);
  double azimuth = Constants::twopi() + std::atan2(dx, dy);
  if (azimuth > Constants::twopi()) azimuth -= Constants::twopi();

  return {distance, azimuth};
}

PositionWeights ForecastPeriod::selectIsotachAndQuadrant(double distance,
                                                         double angle) const {
  const auto [quadrant, delta_angle] = StormQuadrant::getBaseQuadrant(angle);
  const auto [isotach, isotach_weight] =
      ForecastPeriod::selectIsotach(distance, quadrant);
  return {isotach, isotach_weight, quadrant, delta_angle};
}

std::tuple<int, double> ForecastPeriod::selectIsotach(double distance,
                                                      int quadrant) const {
  if (this->m_isotachs.empty()) {
    return {-1, 0.0};
  } else if (distance >=
             m_isotachs.front().quadrant(quadrant).isotach_radius()) {
    return {0, 1.0};
  } else if (distance <=
             m_isotachs.back().quadrant(quadrant).isotach_radius()) {
    return {static_cast<int>(m_isotachs.size()) - 1, 1.0};
  } else {
    for (auto i = 0; i < m_isotachs.size() - 1; ++i) {
      const double r1 = m_isotachs[i].quadrant(quadrant).isotach_radius();
      const double r2 = m_isotachs[i + 1].quadrant(quadrant).isotach_radius();
      if (distance <= r1 && distance >= r2) {
        const double weight = (distance - r1) / (r2 - r1);
        return {i, weight};
      }
    }
  }
  return {-1, 0.0};
}

std::vector<StormIsotach> &ForecastPeriod::isotachs() { return m_isotachs; }

const std::vector<StormIsotach> &ForecastPeriod::isotachs() const {
  return m_isotachs;
}

bool ForecastPeriod::operator<(const ForecastPeriod &a) const {
  return this->date() < a.date();
}

bool ForecastPeriod::operator<(const Date &d) const { return this->date() < d; }

Gahm::ForecastPeriod::BASIN ForecastPeriod::stringToBasinId(
    const std::string &basin) {
  auto basin_caps = boost::to_upper_copy(basin);
  if (basin == "AL") {
    return BASIN::AL;
  } else if (basin == "EP") {
    return BASIN::EP;
  } else if (basin == "WP") {
    return BASIN::WP;
  } else if (basin == "IO") {
    return BASIN::IO;
  } else if (basin == "SH") {
    return BASIN::SH;
  } else if (basin == "CP") {
    return BASIN::CP;
  } else if (basin == "SL") {
    return BASIN::SL;
  } else {
    return BASIN::NONE;
  }
}
std::string ForecastPeriod::basinIdToString(
    const ForecastPeriod::BASIN &basin) {
  switch (basin) {
    case AL:
      return "AL";
    case WP:
      return "WP";
    case IO:
      return "IO";
    case SH:
      return "SH";
    case CP:
      return "CP";
    case EP:
      return "EP";
    case SL:
      return "SL";
    default:
      return "NONE";
  }
}

int ForecastPeriod::stormId() const { return m_stormId; }

Gahm::ForecastPeriod::BASIN ForecastPeriod::basin() const { return m_basin; }

std::string ForecastPeriod::stormName() const { return m_stormName; }

double ForecastPeriod::vmaxAtBoundaryLayer() const {
  return m_vmax_at_boundary_layer;
}
