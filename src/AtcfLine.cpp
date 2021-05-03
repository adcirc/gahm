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
#include "AtcfLine.h"

#include <cassert>

#include "Physical.h"
#include "boost/algorithm/string/split.hpp"
#include "boost/algorithm/string/trim.hpp"

/**
 * Default constructor
 */
AtcfLine::AtcfLine()
    : m_basin("NA"),
      m_cycloneNumber(0),
      m_refDatetime(1970, 1, 1),
      m_datetime(1970, 1, 1),
      m_technum(0),
      m_techstring("NA"),
      m_tau(0),
      m_lat(0.0),
      m_lon(0.0),
      m_vmax(0.0),
      m_mslp(1013.0),
      m_maxDevelopmentLevel("NA"),
      m_pouter(1013.0),
      m_radiusPouter(0.0),
      m_radiusMaxWinds(0.0),
      m_eyeDiameter(0.0),
      m_gusts(0.0),
      m_subregion('N'),
      m_maxSeas(0.0),
      m_initials("NA"),
      m_stormDirection(0.0),
      m_stormSpeed(0.0),
      m_stormName("NONE"),
      m_systemDepth("NA"),
      m_null(true),
      m_u(0.0),
      m_v(0.0),
      m_uv(0.0) {}

/**
 * Static function to parse an atcf line text using boost. Places data
 * into an Atcfline object and returns
 * @param[in] line line from the file being read
 * @return AtcfLine object. Will have null field set if unsuccessful
 */
AtcfLine AtcfLine::parseAtcfLine(const std::string &line) {
  if (line.size() < 150) return AtcfLine();
  AtcfLine a;
  auto split = AtcfLine::splitString(line);
  a.setBasin(split[0]);
  a.setCycloneNumber(AtcfLine::readValueCheckBlank<int>(split[1]));
  a.setReferenceDatetime(
      Date(AtcfLine::readValueCheckBlank<int>(split[2].substr(0, 4)),
           AtcfLine::readValueCheckBlank<int>(split[2].substr(4, 2)),
           AtcfLine::readValueCheckBlank<int>(split[2].substr(6, 2)),
           AtcfLine::readValueCheckBlank<int>(split[2].substr(8, 2))));
  a.setTechnum(AtcfLine::readValueCheckBlank<int>(split[3]));
  a.setTechstring(split[4]);
  a.setTau(AtcfLine::readValueCheckBlank<int>(split[5]));
  a.setDatetime(a.referenceDatetime() + a.tau() * 3600);
  auto lat = AtcfLine::readValueCheckBlank<double>(
                 split[6].substr(0, split[6].length() - 1)) /
             10.0;
  auto latns = split[6].back();
  lat = latns == 'S' ? lat * -1.0 : lat;
  a.setLat(lat);
  auto lon = AtcfLine::readValueCheckBlank<double>(
                 split[7].substr(0, split[6].length() - 1)) /
             10.0;
  auto lonew = split[7].back();
  lon = lonew == 'W' ? lon * -1.0 : lon;
  a.setLon(lon);
  a.setVmax(AtcfLine::readValueCheckBlank<double>(split[8]) *
            Physical::kt2ms());
  a.setMslp(AtcfLine::readValueCheckBlank<double>(split[9]));

  auto r1 =
      AtcfLine::readValueCheckBlank<double>(split[13]) * Physical::nmi2km();
  auto r2 =
      AtcfLine::readValueCheckBlank<double>(split[14]) * Physical::nmi2km();
  auto r3 =
      AtcfLine::readValueCheckBlank<double>(split[15]) * Physical::nmi2km();
  auto r4 =
      AtcfLine::readValueCheckBlank<double>(split[16]) * Physical::nmi2km();

  Isotach i(
      Isotach::codeFromString(split[12]),
      AtcfLine::readValueCheckBlank<double>(split[11]) * Physical::kt2ms(), r1,
      r2, r3, r4);
  a.addIsotach(i);

  a.setPouter(AtcfLine::readValueCheckBlank<double>(split[17]));
  a.setRadiusPouter(AtcfLine::readValueCheckBlank<double>(split[18]) *
                    Physical::nmi2km());
  a.setRadiusMaxWinds(AtcfLine::readValueCheckBlank<double>(split[19]) *
                      Physical::nmi2km());
  a.setGusts(AtcfLine::readValueCheckBlank<double>(split[20]) *
             Physical::kt2ms());
  a.setEyeDiameter(AtcfLine::readValueCheckBlank<double>(split[21]) *
                   Physical::nmi2km());
  a.setSubregion(split[22][0]);
  a.setMaxSeas(AtcfLine::readValueCheckBlank<double>(split[23]));
  a.setInitials(split[24]);
  a.setStormDirection(AtcfLine::readValueCheckBlank<double>(split[25]));
  a.setStormSpeed(AtcfLine::readValueCheckBlank<double>(split[26]) *
                  Physical::kt2ms());
  a.setStormName(split[27]);
  a.setIsNull(false);
  return a;
}

/**
 * Generic helper function to check for blank data in Atcf lines during read
 * @tparam T variable type
 * @param line string of data
 * @return data as number or zero in appropriate type
 */
template <typename T, typename>
T AtcfLine::readValueCheckBlank(const std::string &line) {
  if (boost::trim_copy(line).empty()) {
    return 0;
  } else {
    if (std::is_integral<T>::value) {
      return stoi(line);
    } else if (std::is_floating_point<T>::value) {
      return stod(line);
    }
  }
}

/**
 * Split a string by commas using Boost libraries. Note that token compression
 * is turned off
 * @param line string to split
 * @return vector of split strings
 */
std::vector<std::string> AtcfLine::splitString(const std::string &line) {
  auto line2 = line;
  std::vector<std::string> result;
  boost::trim_if(line2, boost::is_any_of(" "));
  boost::algorithm::split(result, line2, boost::is_any_of(","),
                          boost::token_compress_off);
  for (auto &s : result) {
    boost::trim_left(s);
  }
  return result;
}

/**
 * Check if the current object is nulled
 * @return
 */
bool AtcfLine::isNull() const { return m_null; }

/**
 * Set the current object null status
 * @param isNull null status of object
 */
void AtcfLine::setIsNull(bool isNull) { m_null = isNull; }

/**
 * Get the current gust speed
 * @return
 */
double AtcfLine::gusts() const { return m_gusts; }

void AtcfLine::setGusts(double gusts) { m_gusts = gusts; }

std::string AtcfLine::basin() const { return m_basin; }

void AtcfLine::setBasin(const std::string &basin) { m_basin = basin; }

int AtcfLine::cycloneNumber() const { return m_cycloneNumber; }

void AtcfLine::setCycloneNumber(int cycloneNumber) {
  m_cycloneNumber = cycloneNumber;
}

Date AtcfLine::datetime() const { return m_datetime; }

void AtcfLine::setDatetime(const Date &datetime) { m_datetime = datetime; }

Date AtcfLine::referenceDatetime() const { return m_refDatetime; }

void AtcfLine::setReferenceDatetime(const Date &datetime) {
  m_refDatetime = datetime;
}

int AtcfLine::technum() const { return m_technum; }

void AtcfLine::setTechnum(int technum) { m_technum = technum; }

std::string AtcfLine::techstring() const { return m_techstring; }

void AtcfLine::setTechstring(const std::string &techstring) {
  m_techstring = techstring;
}

int AtcfLine::tau() const { return m_tau; }

void AtcfLine::setTau(int tau) { m_tau = tau; }

double AtcfLine::lat() const { return m_lat; }

void AtcfLine::setLat(double lat) { m_lat = lat; }

double AtcfLine::lon() const { return m_lon; }

void AtcfLine::setLon(double lon) { m_lon = lon; }

double AtcfLine::vmax() const { return m_vmax; }

void AtcfLine::setVmax(double vmax) { m_vmax = vmax; }

double AtcfLine::mslp() const { return m_mslp; }

void AtcfLine::setMslp(double mslp) { m_mslp = mslp; }

std::string AtcfLine::maxDevelopmentLevel() const {
  return m_maxDevelopmentLevel;
}

void AtcfLine::setMaxDevelopmentLevel(const std::string &maxDevelopmentLevel) {
  m_maxDevelopmentLevel = maxDevelopmentLevel;
}

double AtcfLine::pouter() const { return m_pouter; }

void AtcfLine::setPouter(double pouter) { m_pouter = pouter; }

double AtcfLine::radiusPouter() const { return m_radiusPouter; }

void AtcfLine::setRadiusPouter(double radiusPouter) {
  m_radiusPouter = radiusPouter;
}

double AtcfLine::radiusMaxWinds() const { return m_radiusMaxWinds; }

void AtcfLine::setRadiusMaxWinds(double radiusMaxWinds) {
  m_radiusMaxWinds = radiusMaxWinds;
}

double AtcfLine::eyeDiameter() const { return m_eyeDiameter; }

void AtcfLine::setEyeDiameter(double eyeDiameter) {
  m_eyeDiameter = eyeDiameter;
}

char AtcfLine::subregion() const { return m_subregion; }

void AtcfLine::setSubregion(char subregion) { m_subregion = subregion; }

double AtcfLine::maxSeas() const { return m_maxSeas; }

void AtcfLine::setMaxSeas(double maxSeas) { m_maxSeas = maxSeas; }

std::string AtcfLine::initials() const { return m_initials; }

void AtcfLine::setInitials(const std::string &initials) {
  m_initials = initials;
}

double AtcfLine::stormDirection() const { return m_stormDirection; }

void AtcfLine::setStormDirection(double stormDirection) {
  m_stormDirection = stormDirection;
}

double AtcfLine::stormSpeed() const { return m_stormSpeed; }

void AtcfLine::setStormSpeed(double stormSpeed) { m_stormSpeed = stormSpeed; }

std::string AtcfLine::systemDepth() const { return m_systemDepth; }

void AtcfLine::setSystemDepth(const std::string &systemDepth) {
  m_systemDepth = systemDepth;
}

std::string AtcfLine::stormName() const { return m_stormName; }

void AtcfLine::setStormName(const std::string &storm_name) {
  m_stormName = storm_name;
}

Isotach *AtcfLine::isotach(size_t index) {
  assert(index < m_isotach.size());
  return &m_isotach[index];
}

const Isotach *AtcfLine::cisotach(size_t index) const {
  assert(index < m_isotach.size());
  return &m_isotach[index];
}

void AtcfLine::addIsotach(const Isotach &iso) {
  this->m_isotach.push_back(iso);
}

void AtcfLine::removeIsotach(size_t pos) {
  this->m_isotach.erase(this->m_isotach.begin() + pos);
}

size_t AtcfLine::nIsotach() const { return m_isotach.size(); }

Isotach *AtcfLine::seas(size_t index) {
  assert(index < m_seas.size());
  return &m_seas[index];
}

const Isotach *AtcfLine::cseas(size_t index) const {
  assert(index < m_seas.size());
  return &m_seas[index];
}

/**
 * @brief operator<< Overload for streaming AtcfLine objects to a stream
 * @param os ostream object
 * @param atcf AtcfLine object to write to specified stream
 * @return stream object
 */
std::ostream &operator<<(std::ostream &os, const AtcfLine &atcf) {
  os << "|---------------------------------------------------------------------"
        "---|\n";
  os << "                         Basin: " << atcf.basin() << "\n";
  os << "                       Cyclone: " << atcf.cycloneNumber() << "\n";
  os << "                    Storm Name: " << atcf.stormName() << "\n";
  os << "                          Date: " << atcf.datetime().toString()
     << "\n";
  os << "                Reference Date: "
     << atcf.referenceDatetime().toString() << "\n";
  os << "                     Technique: " << atcf.techstring() << "("
     << atcf.technum() << ")\n";
  os << "                 Forecast Hour: " << atcf.tau() << " hours\n";
  os << "                     Longitude: " << atcf.lon() << " degrees east\n";
  os << "                      Latitude: " << atcf.lat() << " degrees north\n";
  os << "                      Max Wind: " << atcf.vmax() << " m/s\n";
  os << "                     Max Gusts: " << atcf.gusts() << " m/s\n";
  os << "                  Min Pressure: " << atcf.mslp() << " mb\n";
  os << "  Number of specified isotachs: " << atcf.m_isotach.size() << "\n";
  size_t n = 0;
  for (const auto iso : atcf.m_isotach) {
    os << "                        Isotach " << n << ": " << iso << " km";
    if (Isotach::isNull(iso)) {
      os << "; [NULL]";
    }
    os << "\n";
    ++n;
  }
  os << "            Last Closed Isobar: " << atcf.pouter() << " mb\n";
  os << "     Radius Last Closed Isobar: " << atcf.radiusPouter() << " km\n";
  os << "       Radius to Maximum Winds: " << atcf.radiusMaxWinds() << " km\n";
  os << "                  Eye Diameter: " << atcf.eyeDiameter() << " km\n";
  os << "               Storm Direction: " << atcf.stormDirection() << " deg\n";
  os << "                   Storm Speed: " << atcf.stormSpeed() << " m/s\n";
  os << "|---------------------------------------------------------------------"
        "---|\n";
  os << "\n";
  return os;
}

bool AtcfLine::isSameForecastPeriod(const AtcfLine &a1, const AtcfLine &a2) {
  if (a1.basin() == a2.basin() && a1.cycloneNumber() == a2.cycloneNumber() &&
      a1.datetime() == a2.datetime() &&
      a1.cisotach(0)->windSpeed() != a2.cisotach(0)->windSpeed()) {
    return true;
  } else {
    return false;
  }
}

bool AtcfLine::operator<(const AtcfLine &a) const {
  if (this->datetime() < a.datetime()) return true;
  if (this->datetime() > a.datetime()) return false;
  if (this->nIsotach() == 0 && a.nIsotach() == 0) return false;
  if (this->nIsotach() > 0 && a.nIsotach() == 0) return true;
  if (this->nIsotach() == 0 && a.nIsotach() > 0) return false;
  if (this->cisotach(0)->windSpeed() < a.cisotach(0)->windSpeed()) return true;
  return false;
}

void AtcfLine::setStormTranslationVelocities(const double u, const double v,
                                             const double uv) {
  m_u = u;
  m_v = v;
  m_uv = uv;
}

std::tuple<double, double, double> AtcfLine::stormTranslationVelocities()
    const {
  return std::make_tuple(m_u, m_v, m_uv);
}

double AtcfLine::uvTrans() const { return m_uv; }

void AtcfLine::setUvTrans(double uv) { m_uv = uv; }

double AtcfLine::uTrans() const { return m_u; }

void AtcfLine::setUTrans(double u) { m_u = u; }

double AtcfLine::vTrans() const { return m_v; }

void AtcfLine::setVTrans(double v) { m_v = v; }
