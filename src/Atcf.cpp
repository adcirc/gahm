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
#include <cassert>
#include <fstream>
#include <utility>

#include "Constants.h"
#include "Interpolation.h"
#include "Logging.h"
#include "boost/format.hpp"

using namespace Gahm;

/**
 * Constructor
 * @param[in] filename filename of the Atcf file to read
 * @param[in] assumptions pointer to Assumptions object
 */
Atcf::Atcf(std::string filename, std::shared_ptr<Assumptions> a)
    : m_filename(std::move(filename)),
      m_format(Atcf::AtcfFormat::BEST_TRACK),
      m_assumptions(std::move(a)) {
  if (!m_assumptions) m_assumptions = std::make_shared<Assumptions>();
}

Atcf::Atcf(std::string filename, Atcf::AtcfFormat format,
           std::shared_ptr<Assumptions> a)
    : m_filename(std::move(filename)),
      m_format(format),
      m_assumptions(std::move(a)) {
  if (!m_assumptions) m_assumptions = std::make_shared<Assumptions>();
}

/**
 * Returns the filename of the Atcf file that is being used
 * @return filename
 */
std::string Atcf::filename() const { return m_filename; }

/**
 * @brief Sets the Atcf filename to use
 * @param filename
 */
void Atcf::setFilename(const std::string &filename, AtcfFormat format) {
  m_filename = filename;
  m_format = format;
}

Atcf::AtcfFormat Atcf::format() const { return m_format; }

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
    auto a = this->parseNextLine(f);
    if (!a.isNull()) m_atcfData.push_back(a);
  }
  f.close();

  if (m_format == ASWIP)
    this->computeAswipDatetime(m_atcfData[0].referenceDatetime());

  this->organizeAtcfData();

  return 0;
}

AtcfLine Atcf::parseNextLine(std::ifstream &f) {
  std::string line;
  std::getline(f, line);
  if (m_format == BEST_TRACK) {
    return AtcfLine::parseBestTrackLine(line);
  } else if (m_format == ASWIP) {
    return AtcfLine::parseAswipLine(line);
  } else {
    gahm_throw_exception("Invalid file format specified.");
    return {};
  }
}

void Atcf::organizeAtcfData() {
  std::sort(m_atcfData.begin(), m_atcfData.end());
  for (auto i = m_atcfData.size() - 1; i >= 1; --i) {
    if (AtcfLine::isSameForecastPeriod(m_atcfData[i], m_atcfData[i - 1])) {
      for (size_t j = 0; j < m_atcfData[i].nIsotach(); ++j) {
        m_atcfData[i - 1].addIsotach(m_atcfData[i].isotach(j));
      }
      m_atcfData.erase(m_atcfData.begin() + i);
    }
  }
}

void Atcf::computeAswipDatetime(const Date &referenceDate) {
  for (auto &a : m_atcfData) {
    a.setDatetime(referenceDate + a.tau() * 3600);
  }
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
  assert(std::is_sorted(m_atcfData.begin(), m_atcfData.end()));
  if (d < m_atcfData.front().datetime()) {
    m_assumptions->add(generate_assumption(
        Assumption::MINOR, "Requested date (" + d.toString() +
                               ") occurs before the first data record (" +
                               m_atcfData.front().datetime().toString() +
                               "). Assuming storm is in the start position."));
    return {0, 1.0};
  } else if (d > m_atcfData.back().datetime()) {
    m_assumptions->add(generate_assumption(
        Assumption::MINOR, "Requested date (" + d.toString() +
                               ") occurs after the last data record (" +
                               m_atcfData.back().datetime().toString() +
                               "). Assuming storm is in the last position."));
    return {m_atcfData.size() - 1, 1.0};
  } else if (d == m_atcfData.front().datetime()) {
    return {0, 1.0};
  } else if (d == m_atcfData.back().datetime()) {
    return {m_atcfData.size() - 1, 1.0};
  } else {
    const auto dt = d.toSeconds();
    auto it = std::lower_bound(m_atcfData.cbegin(), m_atcfData.cend(), dt,
                               AtcfLine::atcfLineLessThan()) -
              1;
    auto t1 = it->datetime().toSeconds();
    auto t2 = (it + 1)->datetime().toSeconds();
    return {it - m_atcfData.begin(),
            static_cast<double>(dt - t1) / static_cast<double>(t2 - t1)};
  }
  gahm_throw_exception(
      "Could not find a suitable time record. Check record ordering.");
  return {0, 0.0};
}

/**
 * Returns a StormParameters object constructed via linear interpolation at the
 * specified date/time
 * @param[in] d date/time to compute parameters for
 * @return StormParameters object for specified date
 */
StormParameters Atcf::getStormParameters(const Date &d) const {
  auto w = this->getCycleNumber(d);
  return this->getStormParameters(w.first, w.second);
}

/**
 * Returns a StormParameters object constructed via linear interpolation at the
 * specified date/time
 * @param[in] cycle cycle to compute parameters for
 * @param[in] weight of the specified cycle versus the next cycle
 * @return StormParameters object for specified date
 */
StormParameters Atcf::getStormParameters(const int cycle,
                                         const double weight) const {
  StormParameters s;
  s.setCycle(cycle);
  s.setWtratio(weight);
  if (cycle < 1) {
    s.setCentralPressure(m_atcfData.front().centralPressure());
    s.setBackgroundPressure(m_atcfData.front().lastClosedIsobar());
    s.setVmax(m_atcfData.front().vmax());
    s.setLatitude(m_atcfData.front().lat());
    s.setLongitude(m_atcfData.front().lon());
    s.setUtrans(m_atcfData.front().uTrans());
    s.setVtrans(m_atcfData.front().vTrans());
    s.setUvtrans(m_atcfData.front().uvTrans());
  } else if (cycle >= m_atcfData.size() - 1) {
    s.setCentralPressure(m_atcfData.back().centralPressure());
    s.setBackgroundPressure(m_atcfData.back().lastClosedIsobar());
    s.setVmax(m_atcfData.back().vmax());
    s.setLatitude(m_atcfData.back().lat());
    s.setLongitude(m_atcfData.back().lon());
    s.setUtrans(m_atcfData.back().uTrans());
    s.setVtrans(m_atcfData.back().vTrans());
    s.setUvtrans(m_atcfData.back().uvTrans());
  } else {
    s.setCentralPressure(Interpolation::linearInterp(
        s.wtratio(), m_atcfData[cycle].centralPressure(),
        m_atcfData[cycle + 1].centralPressure()));
    s.setBackgroundPressure(Interpolation::linearInterp(
        s.wtratio(), m_atcfData[cycle].lastClosedIsobar(),
        m_atcfData[cycle + 1].lastClosedIsobar()));
    s.setVmax(Interpolation::linearInterp(s.wtratio(), m_atcfData[cycle].vmax(),
                                          m_atcfData[cycle + 1].vmax()));
    s.setLatitude(Interpolation::linearInterp(
        s.wtratio(), m_atcfData[cycle].lat(), m_atcfData[cycle + 1].lat()));
    s.setLongitude(Interpolation::linearInterp(
        s.wtratio(), m_atcfData[cycle].lon(), m_atcfData[cycle + 1].lon()));
    s.setUtrans(Interpolation::linearInterp(s.wtratio(),
                                            m_atcfData[cycle].uTrans(),
                                            m_atcfData[cycle + 1].uTrans()));
    s.setVtrans(Interpolation::linearInterp(s.wtratio(),
                                            m_atcfData[cycle].vTrans(),
                                            m_atcfData[cycle + 1].vTrans()));
    s.setUvtrans(Interpolation::linearInterp(s.wtratio(),
                                             m_atcfData[cycle].uvTrans(),
                                             m_atcfData[cycle + 1].uvTrans()));
  }
  s.setCorio(Physical::coriolis(s.latitude()));

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
    const std::string backgroundPressure = boost::str(
        boost::format("%4i") % std::round(Physical::backgroundPressure()));
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
          (std::round(a.isotach(i).windSpeed() *
                      Units::convert(Units::MetersPerSecond, Units::Knot))));
      const std::string isospd = boost::str(
          boost::format("%7.1f") %
          (std::round(a.isotach(i).windSpeed() *
                      Units::convert(Units::MetersPerSecond, Units::Knot))));
      const std::string ir1 = boost::str(
          boost::format("%7.1f") %
          (std::round(a.isotach(i).isotachRadius().at(0) *
                      Units::convert(Units::Kilometer, Units::NauticalMile))));
      const std::string ir2 = boost::str(
          boost::format("%7.1f") %
          (std::round(a.isotach(i).isotachRadius().at(1) *
                      Units::convert(Units::Kilometer, Units::NauticalMile))));
      const std::string ir3 = boost::str(
          boost::format("%7.1f") %
          (std::round(a.isotach(i).isotachRadius().at(2) *
                      Units::convert(Units::Kilometer, Units::NauticalMile))));
      const std::string ir4 = boost::str(
          boost::format("%7.1f") %
          (std::round(a.isotach(i).isotachRadius().at(4) *
                      Units::convert(Units::Kilometer, Units::NauticalMile))));
      const std::string bbase =
          boost::str(boost::format("%9.4f") % a.hollandB());
      const std::string b1 =
          boost::str(boost::format("%9.4f") % a.isotach(i).hollandB().at(0));
      const std::string b2 =
          boost::str(boost::format("%9.4f") % a.isotach(i).hollandB().at(1));
      const std::string b3 =
          boost::str(boost::format("%9.4f") % a.isotach(i).hollandB().at(2));
      const std::string b4 =
          boost::str(boost::format("%9.4f") % a.isotach(i).hollandB().at(3));

      const std::string vmax1 =
          boost::str(boost::format("%9.4f") %
                     (a.isotach(i).vmaxBl().at(0) *
                      Units::convert(Units::MetersPerSecond, Units::Knot)));
      const std::string vmax2 =
          boost::str(boost::format("%9.4f") %
                     (a.isotach(i).vmaxBl().at(1) *
                      Units::convert(Units::MetersPerSecond, Units::Knot)));
      const std::string vmax3 =
          boost::str(boost::format("%9.4f") %
                     (a.isotach(i).vmaxBl().at(2) *
                      Units::convert(Units::MetersPerSecond, Units::Knot)));
      const std::string vmax4 =
          boost::str(boost::format("%9.4f") %
                     (a.isotach(i).vmaxBl().at(3) *
                      Units::convert(Units::MetersPerSecond, Units::Knot)));

      f << boost::str(
          boost::format(
              "%3s, %02i, %04i%02i%02i%02i,   "
              ",%5s,%4s,%5s,%5s,%4s,%5s,   "
              ",%5s,%5s,%5s,%5s,%4s,%4s,%5s,     ,%4s,     ,    , "
              "   ,    ,    "
              ",%3s,%4s,%12s,%4i,%5i,%2i,%2i,%2i,%2i,%9s,%9s,%9s,%9s,%9s,"
              "%9s,%9s,%9s,%9s,%9s,%9s,%9s,%9s\n") %
          a.basin() % a.cycloneNumber() % a.datetime().year() %
          a.datetime().month() % a.datetime().day() % a.datetime().hour() %
          a.techstring() % forecastHour % lat % lon % vmax % mslp %
          isoWindSpeed % Isotach::stringFromCode(Isotach::RadiusCode::NEQ) %
          ir1 % ir2 % ir3 % ir4 % backgroundPressure % rmax % heading %
          forwardSpeed % a.stormName() % cycleNumber % a.nIsotach() %
          a.isotach(i).quadFlag().at(0) % a.isotach(i).quadFlag().at(1) %
          a.isotach(i).quadFlag().at(2) % a.isotach(i).quadFlag().at(3) % ir1 %
          ir2 % ir3 % ir4 % bbase % b1 % b2 % b3 % b4 % vmax1 % vmax2 % vmax3 %
          vmax4);
    }
  }
  f.close();
}
Date Atcf::begin_time() const { return m_atcfData.front().datetime(); }

Date Atcf::end_time() const { return m_atcfData.back().datetime(); }

Assumptions *Atcf::assumptions() { return m_assumptions.get(); }

std::shared_ptr<Assumptions> Atcf::assumptions_sharedptr() {
  return m_assumptions;
}
