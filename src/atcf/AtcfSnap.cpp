// GNU General Public License v3.0
//
// This file is part of the GAHM model (https://github.com/adcirc/gahm).
// Copyright (c) 2023 ADCIRC Development Group.
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, version 3.
//
// This program is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program. If not, see <http://www.gnu.org/licenses/>.
//
// Author: Zach Cobell
// Contact: zcobell@thewaterinstitute.org
//
#include "AtcfSnap.h"

#include <optional>
#include <utility>

#include "fmt/compile.h"
#include "fmt/core.h"
#include "physical/Atmospheric.h"
#include "physical/Units.h"
#include "util/StringUtilities.h"

namespace Gahm::Atcf {

/*
 * Constructor
 * @param basin Basin of the storm
 * @param central_pressure Central pressure of the storm
 * @param background_pressure Background pressure of the storm
 * @param radius_to_max_winds Radius to maximum winds of the storm
 * @param vmax Maximum sustained wind speed of the storm
 * @param vmax_boundary_layer Maximum sustained wind speed of the storm in the
 * boundary layer
 * @param date Date of the storm
 * @param storm_id Storm ID
 * @param storm_name Storm name
 */
AtcfSnap::AtcfSnap(AtcfSnap::BASIN basin, double central_pressure,
                   double background_pressure, double radius_to_max_winds,
                   double vmax, double vmax_boundary_layer,
                   const Datatypes::Date& date, int storm_id,
                   std::string storm_name)
    : m_central_pressure(central_pressure),
      m_background_pressure(background_pressure),
      m_radius_to_max_winds(radius_to_max_winds),
      m_vmax(vmax),
      m_vmax_boundary_layer(vmax_boundary_layer),
      m_holland_b(Gahm::Physical::Atmospheric::calcHollandB(
          vmax, central_pressure, background_pressure)),
      m_date(date),
      m_storm_id(storm_id),
      m_basin(basin),
      m_storm_name(std::move(storm_name)),
      m_radii(std::array<std::vector<double>, 4>{}),
      m_position(0.0, 0.0),
      m_translation(0.0, 0.0) {}

/*
 * Converts a string to a basin enum
 * @param basin String representation of the basin
 * @return Basin enum
 */
AtcfSnap::BASIN AtcfSnap::basinFromString(const std::string& basin) {
  if (basin == "WP") {
    return AtcfSnap::BASIN::WP;
  } else if (basin == "IO") {
    return AtcfSnap::BASIN::IO;
  } else if (basin == "SH") {
    return AtcfSnap::BASIN::SH;
  } else if (basin == "CP") {
    return AtcfSnap::BASIN::CP;
  } else if (basin == "EP") {
    return AtcfSnap::BASIN::EP;
  } else if (basin == "AL") {
    return AtcfSnap::BASIN::AL;
  } else if (basin == "SL") {
    return AtcfSnap::BASIN::SL;
  } else {
    return AtcfSnap::BASIN::NONE;
  }
}

/*
 * Converts a basin enum to a string
 * @param basin Basin enum
 * @return String representation of the basin
 */
std::string AtcfSnap::basinToString(AtcfSnap::BASIN basin) {
  switch (basin) {
    case AtcfSnap::BASIN::WP:
      return "WP";
    case AtcfSnap::BASIN::IO:
      return "IO";
    case AtcfSnap::BASIN::SH:
      return "SH";
    case AtcfSnap::BASIN::CP:
      return "CP";
    case AtcfSnap::BASIN::EP:
      return "EP";
    case AtcfSnap::BASIN::AL:
      return "AL";
    case AtcfSnap::BASIN::SL:
      return "SL";
    default:
      return "XX";
  }
}

/*
 * Returns the central pressure of the storm
 * @return Central pressure of the storm
 */
double AtcfSnap::centralPressure() const { return m_central_pressure; }

/*
 * Sets the central pressure of the storm
 * @param centralPressure Central pressure of the storm
 */
void AtcfSnap::setCentralPressure(double centralPressure) {
  m_central_pressure = centralPressure;
}

/*
 * Returns the background pressure of the storm
 * @return Background pressure of the storm
 */
double AtcfSnap::backgroundPressure() const { return m_background_pressure; }

/*
 * Sets the background pressure of the storm
 * @param backgroundPressure Background pressure of the storm
 */
void AtcfSnap::setBackgroundPressure(double backgroundPressure) {
  m_background_pressure = backgroundPressure;
}

/*
 * Returns the radius to maximum winds of the storm
 * @return Radius to maximum winds of the storm
 */
double AtcfSnap::radiusToMaxWinds() const { return m_radius_to_max_winds; }

/*
 * Sets the radius to maximum winds of the storm
 * @param radiusToMaxWinds Radius to maximum winds of the storm
 */
void AtcfSnap::setRadiusToMaxWinds(double radiusToMaxWinds) {
  m_radius_to_max_winds = radiusToMaxWinds;
}

/*
 * Returns the maximum sustained wind speed of the storm
 * @return Maximum sustained wind speed of the storm
 */
double AtcfSnap::vmax() const { return m_vmax; }

/*
 * Sets the maximum sustained wind speed of the storm
 * @param vmax Maximum sustained wind speed of the storm
 */
void AtcfSnap::setVmax(double vmax) { m_vmax = vmax; }

/*
 * Returns the maximum sustained wind speed of the storm in the boundary layer
 * @return Maximum sustained wind speed of the storm in the boundary layer
 */
double AtcfSnap::vmaxBoundaryLayer() const { return m_vmax_boundary_layer; }

/*
 * Sets the maximum sustained wind speed of the storm in the boundary layer
 * @param vmaxBoundaryLayer Maximum sustained wind speed of the storm in the
 * boundary layer
 */
void AtcfSnap::setVmaxBoundaryLayer(double vmaxBoundaryLayer) {
  m_vmax_boundary_layer = vmaxBoundaryLayer;
}

/*
 * Returns the date of the storm snap
 * @return Date of the storm snap
 */
const Gahm::Datatypes::Date& AtcfSnap::date() const { return m_date; }

/*
 * Sets the date of the storm snap
 * @param date Date of the storm snap
 */
void AtcfSnap::setDate(const Gahm::Datatypes::Date& date) { m_date = date; }

/*
 * Returns the storm ID
 * @return Storm ID
 */
int AtcfSnap::stormId() const { return m_storm_id; }

/*
 * Sets the storm ID
 * @param stormId Storm ID
 */
void AtcfSnap::setStormId(int stormId) { m_storm_id = stormId; }

/*
 * Returns the basin enum of the storm
 * @return Basin enum of the storm
 */
AtcfSnap::BASIN AtcfSnap::basin() const { return m_basin; }

/*
 * Sets the basin enum of the storm
 * @param basin Basin enum of the storm
 */
void AtcfSnap::setBasin(AtcfSnap::BASIN basin) { m_basin = basin; }

/*
 * Returns the storm name
 * @return Storm name
 */
const std::string& AtcfSnap::stormName() const { return m_storm_name; }

/*
 * Sets the storm name
 * @param stormName Storm name
 */
void AtcfSnap::setStormName(const std::string& stormName) {
  m_storm_name = stormName;
}

/*
 * Returns the isotachs of the storm
 * @return Isotachs of the storm
 */
const std::vector<AtcfIsotach>& AtcfSnap::getIsotachs() const {
  return m_isotachs;
}

/*
 * Returns the Holland B of the storm
 * @return Holland B of the storm
 */
double AtcfSnap::hollandB() const { return m_holland_b; }

/**
 * Sets the radius of the storm
 * @param hollandB Holland B
 */
void AtcfSnap::setHollandB(double hollandB) { m_holland_b = hollandB; }

/**
 * Returns the isotachs of the snap
 * @return Isotachs of the snap
 */
std::vector<AtcfIsotach>& AtcfSnap::getIsotachs() { return m_isotachs; }

/*
 * Returns the radii of the storm in the form of a circular array
 * @return Radii of the storm in the form of a circular array
 */
const Gahm::Datatypes::CircularArray<std::vector<double>, 4>& AtcfSnap::radii()
    const {
  return m_radii;
}

/*
 * Returns the number of isotachs in the snap
 * @return Number of isotachs in the snap
 */
size_t AtcfSnap::numberOfIsotachs() const { return m_isotachs.size(); }

/*
 * Adds an isotach to the snap
 * @param isotach Isotach to add
 */
void AtcfSnap::addIsotach(const AtcfIsotach& isotach) {
  m_isotachs.push_back(isotach);
}

/*
 * Parses an ATCF snap from a string which was read
 * from a file in the ATCF format
 * @param line Line to parse
 * @return ATCF snap
 */
std::optional<AtcfSnap> AtcfSnap::parseAtcfSnap(const std::string& line) {
  constexpr double kt2ms = Gahm::Physical::Units::convert(
      Gahm::Physical::Units::Knot, Gahm::Physical::Units::MetersPerSecond);
  constexpr double nmi2m = Gahm::Physical::Units::convert(
      Gahm::Physical::Units::NauticalMile, Gahm::Physical::Units::Meter);

  using namespace Gahm::detail::Utilities;

  auto tokens = splitString(line);
  if (tokens.size() < 27) {
    //...The line is not valid
    return {};
  }

  const auto snap_basin = AtcfSnap::basinFromString(tokens[0]);
  const auto storm_id = readValueCheckBlank<int>(tokens[1]);
  const auto snap_date = AtcfSnap::parseDate(tokens[2], tokens[5]);

  const auto lat = [&]() {
    double l = stod(tokens[6].substr(0, tokens[6].size() - 1)) / 10.0;
    if (*(tokens[6].rbegin()) == 'S') l *= -1.0;
    return l;
  }();

  const auto lon = [&]() {
    double l = stod(tokens[7].substr(0, tokens[7].size() - 1)) / 10.0;
    if (*(tokens[7].rbegin()) == 'W') l *= -1.0;
    return l;
  }();

  const auto v_max = readValueCheckBlank<double>(tokens[8]) * kt2ms;
  const auto p_min = readValueCheckBlank<double>(tokens[9]) * 100.0;
  const auto r_max = readValueCheckBlank<double>(tokens[19]) * nmi2m;
  const auto storm_name = boost::trim_copy(tokens[27]);

  auto snap = AtcfSnap(snap_basin, p_min,
                       Gahm::Physical::Constants::backgroundPressure() * 100.0,
                       r_max, v_max, v_max, snap_date, storm_id, storm_name);
  snap.setPosition({lon, lat});

  auto isotach = AtcfSnap::parseIsotach(tokens);
  snap.addIsotach(isotach);

  return snap;
}

/*
 * Parses an isotach from a vector of tokens
 * @param tokens Tokens to parse
 * @return Isotach parsed from the tokens
 */
AtcfIsotach AtcfSnap::parseIsotach(const std::vector<std::string>& tokens) {
  using namespace Gahm::detail::Utilities;
  constexpr double kt2ms = Gahm::Physical::Units::convert(
      Gahm::Physical::Units::Knot, Gahm::Physical::Units::MetersPerSecond);
  constexpr double nmi2m = Gahm::Physical::Units::convert(
      Gahm::Physical::Units::NauticalMile, Gahm::Physical::Units::Meter);

  auto isotach_wind_speed = readValueCheckBlank<double>(tokens[11]) * kt2ms;
  if (isotach_wind_speed == 0.0) {
    // If no isotach speed is given, we set the isotach speed to vmax and the
    // radii to rmax
    const auto v_max = readValueCheckBlank<double>(tokens[8]) * kt2ms;
    const auto r_max = readValueCheckBlank<double>(tokens[19]) * nmi2m;
    return {v_max, {r_max, r_max, r_max, r_max}};
  } else {
    const std::array<double, 4> isotach_radii = {
        readValueCheckBlank<double>(tokens[13]) * nmi2m,
        readValueCheckBlank<double>(tokens[14]) * nmi2m,
        readValueCheckBlank<double>(tokens[15]) * nmi2m,
        readValueCheckBlank<double>(tokens[16]) * nmi2m};
    return {isotach_wind_speed, isotach_radii};
  }
}

/*
 * Parses the date from the ATCF snap
 * @param date_str Date string
 * @param tau_str Time string
 * @return Date
 */
Gahm::Datatypes::Date AtcfSnap::parseDate(const std::string& date_str,
                                          const std::string& tau_str) {
  auto base_date =
      Gahm::Datatypes::Date::fromString(boost::trim_copy(date_str), "%Y%m%d%H");
  const auto tau = Gahm::detail::Utilities::readValueCheckBlank<int>(tau_str);
  base_date.addHours(tau);
  return base_date;
}

/*
 * Comparison operator for an atcf snap based upon date
 * @param other Other atcf snap
 * @return True if this atcf snap is before the other atcf snap
 */
bool AtcfSnap::operator<(const AtcfSnap& other) const {
  return m_date < other.m_date;
}

/*
 * Returns the current position of the storm
 * @return Current position of the storm
 */
const StormPosition& AtcfSnap::position() const { return m_position; }

/*
 * Sets the current position of the storm
 * @param position Current position of the storm
 */
void AtcfSnap::setPosition(const StormPosition& position) {
  m_position = position;
}

/*
 * Returns if the snap has a valid set of data
 */
bool AtcfSnap::isValid() const {
  if (m_isotachs.empty()) {
    return false;
  }
  if (m_position.x() == 0.0 && m_position.y() == 0.0) {
    return false;
  }
  if (m_date == Gahm::Datatypes::Date()) {
    return false;
  }
  if (std::any_of(
          m_isotachs.begin(), m_isotachs.end(),
          [](const auto& isotach) { return isotach.getWindSpeed() == 0.0; })) {
    return false;
  }
  return true;
}

/*
 * Returns the storm translation for the snap
 * @return Storm translation for the snap
 */
const StormTranslation& AtcfSnap::translation() const { return m_translation; }

/*
 * Sets the storm translation for the snap
 * @param translation Storm translation for the snap
 */
void AtcfSnap::setTranslation(const StormTranslation& translation) {
  m_translation = translation;
}

/*
 * Returns a string representation of the snap in GAHM format
 */
std::string AtcfSnap::to_string(size_t cycle,
                                const Gahm::Datatypes::Date& start_date,
                                size_t isotach_index) const {
  constexpr double ms2kt = Gahm::Physical::Units::convert(
      Gahm::Physical::Units::MetersPerSecond, Gahm::Physical::Units::Knot);
  constexpr double m2nmi = Gahm::Physical::Units::convert(
      Gahm::Physical::Units::Meter, Gahm::Physical::Units::NauticalMile);

  auto isotach = m_isotachs[isotach_index];
  auto snap_date = m_date.toString("%Y%m%d%H");
  auto snap_basin = AtcfSnap::basinToString(m_basin);
  auto lat = fmt::format(
      "{:3d}", static_cast<int>(std::floor(std::abs(m_position.y() * 10.0))));
  if (m_position.y() <= 0.0) {
    lat = fmt::format("{}S", lat);
  } else {
    lat = fmt::format("{}N", lat);
  }
  auto lon = fmt::format(
      "{:4d}", static_cast<int>(std::floor(std::abs(m_position.x() * 10.0))));
  if (m_position.x() <= 0.0) {
    lon = fmt::format("{}W", lon);
  } else {
    lon = fmt::format("{}E", lon);
  }
  auto snap_vmax =
      fmt::format("{:3d}", static_cast<int>(std::round(m_vmax * ms2kt)));
  auto mslp = fmt::format(
      "{:4d}", static_cast<int>(std::round(m_central_pressure / 100.0)));
  auto prbk = fmt::format(
      "{:4d}", static_cast<int>(std::round(m_background_pressure / 100.0)));
  auto rmax = fmt::format(
      "{:4d}", static_cast<int>(std::round(m_radius_to_max_winds * m2nmi)));
  auto heading = fmt::format(
      "{:3d}", static_cast<int>(m_translation.translationDirection() *
                                Physical::Constants::rad2deg()));
  if (heading == " -0") heading = "  0";
  auto speed = fmt::format(
      "{:4d}", static_cast<int>(m_translation.translationSpeed() * ms2kt));
  auto forecast_hour = fmt::format(
      "{:3d}", (m_date.toSeconds() - start_date.toSeconds()) / 3600);
  auto isotach_wind_speed =
      fmt::format("{:5.0f}", std::round(isotach.getWindSpeed() * ms2kt));
  auto isotach_radius_0 = fmt::format(
      "{:5.0f}", std::round(isotach.getQuadrant(0).getIsotachRadius() * m2nmi));
  auto isotach_radius_1 = fmt::format(
      "{:5.0f}", std::round(isotach.getQuadrant(1).getIsotachRadius() * m2nmi));
  auto isotach_radius_2 = fmt::format(
      "{:5.0f}", std::round(isotach.getQuadrant(2).getIsotachRadius() * m2nmi));
  auto isotach_radius_3 = fmt::format(
      "{:5.0f}", std::round(isotach.getQuadrant(3).getIsotachRadius() * m2nmi));

  auto rmx0 = fmt::format(
      "{:9.4f}", isotach.getQuadrant(0).getRadiusToMaxWindSpeed() * m2nmi);
  auto rmx1 = fmt::format(
      "{:9.4f}", isotach.getQuadrant(1).getRadiusToMaxWindSpeed() * m2nmi);
  auto rmx2 = fmt::format(
      "{:9.4f}", isotach.getQuadrant(2).getRadiusToMaxWindSpeed() * m2nmi);
  auto rmx3 = fmt::format(
      "{:9.4f}", isotach.getQuadrant(3).getRadiusToMaxWindSpeed() * m2nmi);

  auto b = fmt::format("{:9.4f}", m_holland_b);
  auto bg0 = fmt::format("{:9.4f}", isotach.getQuadrant(0).getGahmHollandB());
  auto bg1 = fmt::format("{:9.4f}", isotach.getQuadrant(1).getGahmHollandB());
  auto bg2 = fmt::format("{:9.4f}", isotach.getQuadrant(2).getGahmHollandB());
  auto bg3 = fmt::format("{:9.4f}", isotach.getQuadrant(3).getGahmHollandB());

  auto vmbl0 = fmt::format(
      "{:9.4f}", isotach.getQuadrant(0).getIsotachSpeedAtBoundaryLayer());
  auto vmbl1 = fmt::format(
      "{:9.4f}", isotach.getQuadrant(1).getIsotachSpeedAtBoundaryLayer());
  auto vmbl2 = fmt::format(
      "{:9.4f}", isotach.getQuadrant(2).getIsotachSpeedAtBoundaryLayer());
  auto vmbl3 = fmt::format(
      "{:9.4f}", isotach.getQuadrant(3).getIsotachSpeedAtBoundaryLayer());

  return fmt::format(
      FMT_COMPILE(
          "{:>2s}, {:>02d}, {:>10s},   ,     "
          ",{:>4s},{:>5s},{:>6s},{:>4s},{:>5s},{:>4s},{:>4s},{:>5s},{:>5s},{:>"
          "5s},{:>5s},{:>5s},     ,{:>4s},     ,    ,    ,    ,    "
          ",{:>3s},{:>4s},{:>12s},{:>4d},{:>5d},{:>2d},{:>2d},{:>2d},{:>2d},{:>"
          "9s},{:>7s},{:>7s},{:>7s},{:>10s},{:>9s},{:>9s},{:>9s},{:>9s},{:>9s},"
          "{:>9s},{:>9s},{:>9s}"),
      snap_basin, m_storm_id, snap_date, forecast_hour, lat, lon, snap_vmax,
      mslp, isotach_wind_speed, std::string("NEQ"), isotach_radius_0,
      isotach_radius_1, isotach_radius_2, isotach_radius_3, prbk, rmax, heading,
      speed, m_storm_name, cycle, numberOfIsotachs(), 1, 1, 1, 1, rmx0, rmx1,
      rmx2, rmx3, b, bg0, bg1, bg2, bg3, vmbl0, vmbl1, vmbl2, vmbl3);
}

/**
 * @brief Get the radii of the isotachs in each quadrant.
 * This preprocessing allows all of the data to be ready
 * for the solver in a friendly manner
 */
void AtcfSnap::processIsotachRadii() {
  for (int i = 0; i < 4; ++i) {
    for (auto& isotach : m_isotachs) {
      m_radii[i].push_back(isotach.getQuadrant(i).getIsotachRadius());
    }
  }
}

/**
 * @brief Order the isotachs by wind speed so they are
 * more easily processed by the solver
 */
void AtcfSnap::orderIsotachs() {
  std::sort(m_isotachs.begin(), m_isotachs.end(),
            [](const AtcfIsotach& a, const AtcfIsotach& b) {
              return a.getWindSpeed() > b.getWindSpeed();
            });
}

}  // namespace Gahm::Atcf