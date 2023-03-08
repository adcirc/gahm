// MIT License
//
// Copyright (c) 2023 ADCIRC Development Group
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

#include "AtcfSnap.h"

#include <optional>
#include <utility>

#include "physical/Constants.h"
#include "physical/Units.h"
#include "util/StringUtilities.h"

using namespace Gahm::Atcf;

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
      m_date(date),
      m_storm_id(storm_id),
      m_basin(basin),
      m_storm_name(std::move(storm_name)),
      m_radii({0, 0, 0, 0}) {}

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
const std::vector<AtcfIsotach>& AtcfSnap::isotachs() const {
  return m_isotachs;
}

/**
 * Returns the isotachs of the snap
 * @return Isotachs of the snap
 */
std::vector<AtcfIsotach>& AtcfSnap::isotachs() { return m_isotachs; }

/*
 * Returns the radii of the storm in the form of a circular array
 * @return Radii of the storm in the form of a circular array
 */
const Gahm::Datatypes::CircularArray<double, 4>& AtcfSnap::radii() const {
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

  const auto basin = AtcfSnap::basinFromString(tokens[0]);
  const auto storm_id = readValueCheckBlank<int>(tokens[1]);
  const auto date = AtcfSnap::parseDate(tokens[2], tokens[5]);

  const auto lat = [&]() {
    double lat = stod(tokens[6].substr(0, tokens[6].size() - 1)) / 10.0;
    if (*(tokens[6].rbegin()) == 'S') lat *= -1.0;
    return lat;
  }();

  const auto lon = [&]() {
    double lon = stod(tokens[7].substr(0, tokens[7].size() - 1)) / 10.0;
    if (*(tokens[7].rbegin()) == 'W') lon *= -1.0;
    return lon;
  }();

  const auto v_max = readValueCheckBlank<double>(tokens[8]) * kt2ms;
  const auto p_min = readValueCheckBlank<double>(tokens[9]) * 100.0;
  const auto r_max = readValueCheckBlank<double>(tokens[19]) * nmi2m;
  const auto storm_name = boost::trim_copy(tokens[27]);

  auto snap =
      AtcfSnap(basin, p_min, Gahm::Physical::Constants::backgroundPressure(),
               r_max, v_max, v_max, date, storm_id, storm_name);

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
    const std::array<double, 4> radii = {
        readValueCheckBlank<double>(tokens[13]) * nmi2m,
        readValueCheckBlank<double>(tokens[14]) * nmi2m,
        readValueCheckBlank<double>(tokens[15]) * nmi2m,
        readValueCheckBlank<double>(tokens[16]) * nmi2m};
    return {isotach_wind_speed, radii};
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
  auto date =
      Gahm::Datatypes::Date::fromString(boost::trim_copy(date_str), "%Y%m%d%H");
  const auto tau = Gahm::detail::Utilities::readValueCheckBlank<int>(tau_str);
  date.addHours(tau);
  return date;
}

/*
 * Comparison operator for an atcf snap based upon date
 * @param other Other atcf snap
 * @return True if this atcf snap is before the other atcf snap
 */
bool AtcfSnap::operator<(const AtcfSnap& other) const {
  return m_date < other.m_date;
}
