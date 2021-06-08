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
#include "Atcf.h"

#include <algorithm>
#include <fstream>
#include <numeric>
#include <utility>

#include "Logging.h"
#include "Physical.h"
#include "boost/format.hpp"

/**
 * Constructor
 * @param[in] filename filename of the Atcf file to read
 * @param[in] assumptions pointer to Assumptions object
 */
Atcf::Atcf(std::string filename, Assumptions *assumptions)
    : m_filename(std::move(filename)), m_assumptions(assumptions) {}

/**
 * Returns the filename of the Atcf file that is being used
 * @return filename
 */
std::string Atcf::filename() const { return m_filename; }

/**
 * @brief Sets the Atcf filename to use
 * @param filename
 */
void Atcf::setFilename(const std::string &filename) { m_filename = filename; }

/**
 * Reads the specified atcf file into a vector of AtcfLine objects
 * @return
 */
int Atcf::read() {
  std::ifstream f(m_filename);
  if (f.fail()) {
    gahm_throw_exception("Specified file does not exist");
    return 1;
  }

  while (!f.eof()) {
    std::string line;
    std::getline(f, line);
    AtcfLine a = AtcfLine::parseAtcfLine(line);
    if (!a.isNull()) {
      m_atcfData.emplace_back(a);
    }
  }
  f.close();

  std::sort(m_atcfData.begin(), m_atcfData.end());
  for (size_t i = m_atcfData.size() - 1; i >= 1; --i) {
    if (AtcfLine::isSameForecastPeriod(m_atcfData[i], m_atcfData[i - 1])) {
      for (size_t j = 0; j < m_atcfData[i].nIsotach(); ++j) {
        m_atcfData[i - 1].addIsotach(*(m_atcfData[i].cisotach(j)));
      }
      m_atcfData.erase(m_atcfData.begin() + i);
    }
  }
  return 0;
}

/**
 * Returns the total number of records currently in the Atcf
 * @return
 */
size_t Atcf::nRecords() const { return m_atcfData.size(); }

/**
 * Returns a reference to an immutable AtcfLine object at the specified index
 * @param[in] index position of the record to return
 * @return AtcfLine object
 */
const AtcfLine *Atcf::crecord(size_t index) const { return &m_atcfData[index]; }

/**
 * Returns the AtcfLine data at the specified location
 * @param[in] index position of the record to return
 * @return Reference to AtcfLine
 */
AtcfLine *Atcf::record(size_t index) { return &m_atcfData[index]; }

std::vector<AtcfLine> *Atcf::data() { return &m_atcfData; }

/**
 * Returns the cycle number and weighting factor for the specified time
 * @param[in] d date/time to select a cycle for
 * @return pair of cycle number prior to or equal to the specified time and the
 * appropriate weighting factor to use
 */
std::pair<int, double> Atcf::getCycleNumber(const Date &d) const {
  if (d < m_atcfData.front().datetime()) {
    m_assumptions->add(generate_assumption(
        Assumption::MINOR, "Requested date (" + d.toString() +
                               ") occurs before the first data record (" +
                               m_atcfData.front().datetime().toString() +
                               "). Assuming storm is in the start position."));
    return {-1, 1.0};
  } else if (d > m_atcfData.back().datetime()) {
    m_assumptions->add(generate_assumption(
        Assumption::MINOR, "Requested date (" + d.toString() +
                               ") occurs after the last data record (" +
                               m_atcfData.back().datetime().toString() +
                               "). Assuming storm is in the last position."));
    return {m_atcfData.size(), 1.0};
  } else {
    auto dt = d.toSeconds();
    for (auto it = m_atcfData.begin(); it < m_atcfData.end(); ++it) {
      auto t1 = it->datetime().toSeconds();
      auto t2 = (it + 1)->datetime().toSeconds();
      if (dt >= t1 && dt < t2) {
        return {it - m_atcfData.begin(),
                static_cast<double>(dt - t1) / static_cast<double>(t2 - t1)};
      }
    }
    gahm_throw_exception(
        "Could not find a suitable time record. Check record ordering.");
    return {0, 0.0};
  }
}

/**
 * Computes a linear interpolation between two values with a specified weighting
 * factor
 * @param[in] weight weighting factor
 * @param[in] v1 value 1 for weighting
 * @param[in] v2 value 2 for weighting
 * @return interpolated value
 */
inline double Atcf::linearInterp(const double weight, const double v1,
                                 const double v2) {
  return (1.0 - weight) * v1 + weight * v2;
}

/**
 * Returns a StormParameters object constructed via linear interpolation at the
 * specified date/time
 * @param[in] d date/time to compute parameters for
 * @return StormParameters object for specified date
 */
Atcf::StormParameters Atcf::getStormParameters(const Date &d) const {
  auto w = this->getCycleNumber(d);
  StormParameters s{};
  s.cycle = w.first;
  s.wtratio = w.second;
  if (s.cycle < 1) {
    s.central_pressure = m_atcfData.front().centralPressure();
    s.background_pressure = m_atcfData.front().lastClosedIsobar();
    s.vmax = m_atcfData.front().vmax();
    s.latitude = m_atcfData.front().lat();
    s.longitude = m_atcfData.front().lon();
    s.utrans = m_atcfData.front().uTrans();
    s.vtrans = m_atcfData.front().vTrans();
    s.uvtrans = m_atcfData.front().uvTrans();
  } else if (s.cycle >= m_atcfData.size()) {
    s.central_pressure = m_atcfData.back().centralPressure();
    s.background_pressure = m_atcfData.back().lastClosedIsobar();
    s.vmax = m_atcfData.back().vmax();
    s.latitude = m_atcfData.back().lat();
    s.longitude = m_atcfData.back().lon();
    s.utrans = m_atcfData.back().uTrans();
    s.vtrans = m_atcfData.back().vTrans();
    s.uvtrans = m_atcfData.back().uvTrans();
  } else {
    s.central_pressure =
        Atcf::linearInterp(s.wtratio, m_atcfData[s.cycle].centralPressure(),
                           m_atcfData[s.cycle + 1].centralPressure());
    s.background_pressure =
        Atcf::linearInterp(s.wtratio, m_atcfData[s.cycle].lastClosedIsobar(),
                           m_atcfData[s.cycle + 1].lastClosedIsobar());
    s.vmax = Atcf::linearInterp(s.wtratio, m_atcfData[s.cycle].vmax(),
                                m_atcfData[s.cycle + 1].vmax());
    s.latitude = Atcf::linearInterp(s.wtratio, m_atcfData[s.cycle].lat(),
                                    m_atcfData[s.cycle + 1].lat());
    s.longitude = Atcf::linearInterp(s.wtratio, m_atcfData[s.cycle].lon(),
                                     m_atcfData[s.cycle + 1].lon());
    s.utrans = Atcf::linearInterp(s.wtratio, m_atcfData[s.cycle].uTrans(),
                                  m_atcfData[s.cycle + 1].uTrans());
    s.vtrans = Atcf::linearInterp(s.wtratio, m_atcfData[s.cycle].vTrans(),
                                  m_atcfData[s.cycle + 1].vTrans());
    s.uvtrans = Atcf::linearInterp(s.wtratio, m_atcfData[s.cycle].uvTrans(),
                                   m_atcfData[s.cycle + 1].uvTrans());
  }

  return s;
}

void Atcf::write(const std::string &filename, Atcf::AtcfFileTypes) const {
  std::ofstream f(filename);
  size_t cycleNumber = 0;
  for (const auto &a : m_atcfData) {
    const std::string lat = boost::str(boost::format("%3i") %
                                       (std::floor(std::abs(a.lat() * 10.0)))) +
                            (a.lat() > 0 ? "N" : "S");
    const std::string lon = boost::str(boost::format("%3i") %
                                       (std::floor(std::abs(a.lon() * 10.0)))) +
                            (a.lon() > 0 ? "E" : "W");
    const std::string vmax =
        boost::str(boost::format("%3i") %
                   (std::round(a.vmax() * Units::convert(Units::MetersPerSecond,
                                                         Units::Knot))));
    const std::string mslp =
        boost::str(boost::format("%4i") % (std::round(a.centralPressure())));
    const std::string backgroundPressure =
        boost::str(boost::format("%4i") % std::round(a.lastClosedIsobar()));
    const std::string rmax = boost::str(
        boost::format("%4i") %
        (std::round(a.radiusMaxWinds() *
                    Units::convert(Units::Kilometer, Units::NauticalMile))));

    std::string heading =
        boost::str(boost::format("%3i") % (std::round(a.stormDirection())));
    if (heading == " -0") heading = "  0";  //...Check for signed zero

    const std::string forwardSpeed = boost::str(
        boost::format("%3i") %
        (std::round(a.stormSpeed() *
                    Units::convert(Units::MetersPerSecond, Units::Knot))));
    const std::string forecastHour =
        boost::str(boost::format("%4i") %
                   std::round((a.datetime().toSeconds() -
                               m_atcfData.begin()->datetime().toSeconds()) /
                              3600));
    cycleNumber++;
    for (size_t i = 0; i < a.nIsotach(); ++i) {
      const std::string isoWindSpeed = boost::str(
          boost::format("%4i") %
          (std::round(a.cisotach(i)->windSpeed() *
                      Units::convert(Units::MetersPerSecond, Units::Knot))));
      const std::string isospd = boost::str(
          boost::format("%7.1f") %
          (std::round(a.cisotach(i)->windSpeed() *
                      Units::convert(Units::MetersPerSecond, Units::Knot))));
      const std::string ir1 = boost::str(
          boost::format("%7.1f") %
          (std::round(a.cisotach(i)->cisotachRadius()->at(0) *
                      Units::convert(Units::Kilometer, Units::NauticalMile))));
      const std::string ir2 = boost::str(
          boost::format("%7.1f") %
          (std::round(a.cisotach(i)->cisotachRadius()->at(1) *
                      Units::convert(Units::Kilometer, Units::NauticalMile))));
      const std::string ir3 = boost::str(
          boost::format("%7.1f") %
          (std::round(a.cisotach(i)->cisotachRadius()->at(2) *
                      Units::convert(Units::Kilometer, Units::NauticalMile))));
      const std::string ir4 = boost::str(
          boost::format("%7.1f") %
          (std::round(a.cisotach(i)->cisotachRadius()->at(4) *
                      Units::convert(Units::Kilometer, Units::NauticalMile))));
      //      const std::string rmx1 =
      //          boost::str(boost::format("%9.1f") %
      //                     (a.cisotach(i)->crmax()->at(0) *
      //                      Units::convert(Units::Kilometer,
      //                      Units::NauticalMile)));
      //      const std::string rmx2 =
      //          boost::str(boost::format("%9.1f") %
      //                     (a.cisotach(i)->crmax()->at(1) *
      //                      Units::convert(Units::Kilometer,
      //                      Units::NauticalMile)));
      //      const std::string rmx3 =
      //          boost::str(boost::format("%9.1f") %
      //                     (a.cisotach(i)->crmax()->at(2) *
      //                      Units::convert(Units::Kilometer,
      //                      Units::NauticalMile)));
      //      const std::string rmx4 =
      //          boost::str(boost::format("%9.1f") %
      //                     (a.cisotach(i)->crmax()->at(3) *
      //                      Units::convert(Units::Kilometer,
      //                      Units::NauticalMile)));
      const std::string b1 = boost::str(boost::format("%9.1f") %
                                        a.cisotach(i)->chollandB()->at(0));
      const std::string b2 = boost::str(boost::format("%9.1f") %
                                        a.cisotach(i)->chollandB()->at(1));
      const std::string b3 = boost::str(boost::format("%9.1f") %
                                        a.cisotach(i)->chollandB()->at(2));
      const std::string b4 = boost::str(boost::format("%9.1f") %
                                        a.cisotach(i)->chollandB()->at(3));
      const std::string phi1 =
          boost::str(boost::format("%9.1f") % a.cisotach(i)->cphi()->at(0));
      const std::string phi2 =
          boost::str(boost::format("%9.1f") % a.cisotach(i)->cphi()->at(1));
      const std::string phi3 =
          boost::str(boost::format("%9.1f") % a.cisotach(i)->cphi()->at(2));
      const std::string phi4 =
          boost::str(boost::format("%9.1f") % a.cisotach(i)->cphi()->at(3));

      f << boost::str(
          boost::format("%3s, %02i, %04i%02i%02i%02i,   "
                        ",%5s,%4s,%5s,%5s,%4s,%5s,   "
                        ",%5s,%5s,%5s,%5s,%4s,%4s,%5s,     ,%4s,     ,    , "
                        "   ,    ,    "
                        ",%3s,%4s,%12s,%4i,%5i,%2i,%2i,%2i,%2i,%9s,%9s,%9s,%9s,"
                        "%9s,%9s,%9s,%9s,%9s,%9s,%9s,%9s\n") %
          a.basin() % a.cycloneNumber() % a.datetime().year() %
          a.datetime().month() % a.datetime().day() % a.datetime().hour() %
          a.techstring() % forecastHour % lat % lon % vmax % mslp %
          isoWindSpeed % Isotach::stringFromCode(Isotach::RadiusCode::NEQ) %
          ir1 % ir2 % ir3 % ir4 % backgroundPressure % rmax % heading %
          forwardSpeed % a.stormName() % cycleNumber % a.nIsotach() %
          a.cisotach(i)->cquadFlag()->at(0) %
          a.cisotach(i)->cquadFlag()->at(1) %
          a.cisotach(i)->cquadFlag()->at(2) %
          a.cisotach(i)->cquadFlag()->at(3) % ir1 % ir2 % ir3 % ir4 % b1 % b2 %
          b3 % b4 % phi1 % phi2 % phi3 % phi4);
    }
  }
  f.close();
}
