//
// Created by Zach Cobell on 5/8/22.
//

#ifndef GAHM2__FORECASTPERIOD_H_
#define GAHM2__FORECASTPERIOD_H_

#include <array>
#include <cstdlib>
#include <vector>

#include "Date.h"
#include "Physical.h"
#include "PositionWeights.h"
#include "StormIsotach.h"
#include "StormPosition.h"

namespace Gahm {
class ForecastPeriod {
 public:
  enum BASIN { NONE, WP, IO, SH, CP, EP, AL, SL };

  ForecastPeriod();

  ForecastPeriod(
      BASIN basin, const Gahm::Date &date, double longitude, double latitude,
      double forward_speed, double transit_direction,
      double radius_to_max_winds, double max_wind_velocity,
      double centralPressure,
      double backgroundPressure = Gahm::Physical::backgroundPressure(),
      int stormId = 0, std::string storm_name = "none");

  [[nodiscard]] size_t nIsotachs() const;

  [[nodiscard]] double centralPressure() const;

  [[nodiscard]] double backgroundPressure() const;

  [[nodiscard]] Date date() const;

  [[nodiscard]] const StormPosition &stormPosition() const;
  [[nodiscard]] StormPosition &stormPosition();

  [[nodiscard]] double maximumWindVelocity() const;

  [[nodiscard]] double vmaxAtBoundaryLayer() const;

  [[nodiscard]] double radiusToMaxWinds() const;

  [[nodiscard]] std::vector<StormIsotach> &isotachs();
  [[nodiscard]] const std::vector<StormIsotach> &isotachs() const;

  void addIsotach(const Gahm::StormIsotach &isotach);
  [[nodiscard]] StormIsotach &isotach(size_t index);
  [[nodiscard]] const StormIsotach &isotach(size_t index) const;

  static constexpr double compute_simple_vmax_at_boundary_layer(
      double wind_speed, double transit_speed) {
    return (wind_speed - transit_speed) / Physical::windReduction();
  }

  [[nodiscard]] Gahm::PositionWeights selectIsotachAndQuadrant(
      double distance, double angle) const;

  [[nodiscard]] static std::tuple<double, double> distanceAndAzimuth(
      double longitude, double latitude, double stormCenterX,
      double stormCenterY);

  bool operator<(const ForecastPeriod &a) const;
  bool operator<(const Date &d) const;

  [[nodiscard]] int stormId() const;

  [[nodiscard]] Gahm::ForecastPeriod::BASIN basin() const;

  [[nodiscard]] std::string stormName() const;

  [[nodiscard]] const std::vector<double> &radii(int quadrant) const;

  static Gahm::ForecastPeriod::BASIN stringToBasinId(const std::string &basin);

  static std::string basinIdToString(const Gahm::ForecastPeriod::BASIN &basin);

  [[nodiscard]] std::tuple<StormIsotach::isotach_it, StormIsotach::isotach_it,
                           int, double>
  selectIsotach(double distance, int quadrant) const;

 private:
  void compute_radii();

  StormPosition m_stormPosition;
  double m_central_pressure;
  double m_background_pressure;
  double m_radius_to_max_winds;
  double m_storm_max_wind_velocity;
  double m_vmax_at_boundary_layer;
  Date m_date;
  int m_stormId;
  ForecastPeriod::BASIN m_basin;
  std::string m_stormName;
  std::vector<StormIsotach> m_isotachs;
  CircularArray<std::vector<double>, 4> m_radii;
};
}  // namespace Gahm

#endif  // GAHM2__FORECASTPERIOD_H_
