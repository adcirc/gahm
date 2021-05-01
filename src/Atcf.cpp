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

#include "Logging.h"
#include "Physical.h"

Atcf::Atcf() : m_filename("none") {}

Atcf::Atcf(const std::string &filename, Assumptions *assumptions)
    : m_filename(filename), m_assumptions(assumptions) {}

std::string Atcf::filename() const { return m_filename; }

void Atcf::setFilename(const std::string &filename) { m_filename = filename; }

/**
 * @brief Reads the specified atcf file into a vector of AtcfLine objects
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

  this->calculateOverlandTranslationVelocity();
  this->generateMissingPressureData();
  this->calculateRmax();
  return 0;
}

int Atcf::uvTrans(const AtcfLine &d1, const AtcfLine &d2, double &uv,
                  double &vv, double &uuvv) {
  const auto dxdy =
      Physical::sphericalDx(d1.lon(), d1.lat(), d2.lon(), d2.lat());
  const double dt = static_cast<double>(d2.datetime().toSeconds() -
                                        d1.datetime().toSeconds());

  uv = std::abs(std::get<0>(dxdy) / dt);
  if (d2.lon() - d1.lon() <= 0.0) uv *= -1.0;

  vv = std::abs(std::get<1>(dxdy) / dt);
  if (d2.lat() - d1.lat() <= 0.0) vv *= -1.0;

  uuvv = std::get<2>(dxdy) / dt;

  return 0;
}

int Atcf::calculateOverlandTranslationVelocity() {
  for (auto it = m_atcfData.begin() + 1; it != m_atcfData.end(); ++it) {
    auto r1 = *(it - 1);
    auto r2 = *(it);
    double u, v, uv;
    int ierr = Atcf::uvTrans(r1, r2, u, v, uv);

    if (ierr != 0) {
      gahm_throw_exception("Error calculating UVTrans");
    }

    if (uv * Physical::ms2kt() < 1.0) {
      uv = 1.0 * Physical::kt2ms();
      it->setStormDirection((it - 1)->stormDirection());
    } else {
      double dir = std::atan2(u, v);
      if (dir < 0.0) dir += 360.0;
      it->setStormDirection(dir);
    }
    it->setStormTranslationVelocities(u, v, uv);
  }
  return 0;
}

int Atcf::calculateRmax() {
  for (auto ait = m_atcfData.begin(); ait != m_atcfData.end(); ++ait) {
    for (size_t i = 0; i < ait->nIsotach(); ++i) {
      const std::array<double, 6> lookup_radii = {
          ait->cisotach(i)->radiusQ4(), ait->cisotach(i)->radiusQ1(),
          ait->cisotach(i)->radiusQ2(), ait->cisotach(i)->radiusQ3(),
          ait->cisotach(i)->radiusQ4(), ait->cisotach(i)->radiusQ1()};
      const double radiisum = std::accumulate(lookup_radii.begin() + 1,
                                              lookup_radii.end() - 1, 0.0);
      std::array<int, 4> quadflag = {
          lookup_radii[1] > 0 ? 1 : 0, lookup_radii[2] > 0 ? 1 : 0,
          lookup_radii[3] > 0 ? 1 : 0, lookup_radii[4] > 0 ? 1 : 0};
      std::array<double, 4> radii = {lookup_radii[1], lookup_radii[2],
                                     lookup_radii[3], lookup_radii[4]};
      const int numNonzero =
          std::accumulate(quadflag.begin(), quadflag.end(), 0);
      switch (numNonzero) {
        case 0:
          std::fill(quadflag.begin(), quadflag.end(), 1);
          std::fill(radii.begin(), radii.end(), ait->radiusMaxWinds());
          this->m_assumptions->add(generate_assumption(
              Assumption::MAJOR,
              "No isotachs reported. Assuming a constant "
              "radius (RMAX). Record " +
                  std::to_string(ait - m_atcfData.cbegin()) +
                  ", isotach: " + std::to_string(i)));
          break;
        case 1:
          for (auto &r : radii) {
            if (r == 0.0) r = radiisum * 0.5;
          }
          this->m_assumptions->add(generate_assumption(
              Assumption::MAJOR,
              "One isotach reported. Missing radii are half "
              "the nonzero radius. Record " +
                  std::to_string(ait - m_atcfData.cbegin()) +
                  ", isotach: " + std::to_string(i)));
          break;
        case 2:
          for (auto &r : radii) {
            if (r == 0.0) r = radiisum * 0.25;
          }
          this->m_assumptions->add(generate_assumption(
              Assumption::MAJOR,
              "Two isotachs reported. Missing radii are half "
              "the average of the nonzero radii, Record " +
                  std::to_string(ait - m_atcfData.cbegin()) +
                  ", isotach: " + std::to_string(i)));
          break;
        case 3:
          for (size_t j = 0; j < radii.size(); ++j) {
            if (radii[j] == 0.0) {
              radii[j] = (lookup_radii[j - 1] + lookup_radii[j + 1]) * 0.5;
            }
          }
          this->m_assumptions->add(generate_assumption(
              Assumption::MAJOR,
              "Three isotachs reported. Missing radius is half "
              "the nonzero radius on either side. Record " +
                  std::to_string(ait - m_atcfData.cbegin()) +
                  ", isotach: " + std::to_string(i)));
          break;
        case 4:
          //...No missing radii
          break;
        default:
          gahm_throw_exception("Number of radii specified is not applicable");
          break;
      }
      ait->isotach(i)->setRadii(radii);
      ait->isotach(i)->setQuadflag(quadflag);
      ait->isotach(i)->setLookupRadii(lookup_radii);
    }
  }

  return 0;
}

size_t Atcf::nRecords() const { return m_atcfData.size(); }

const AtcfLine *Atcf::record(size_t index) const { return &m_atcfData[index]; }

AtcfLine Atcf::record(size_t index) { return m_atcfData[index]; }

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
    double dt = d.toSeconds();
    for (auto it = m_atcfData.begin(); it < m_atcfData.end(); ++it) {
      double t1 = it->datetime().toSeconds();
      double t2 = (it + 1)->datetime().toSeconds();
      if (dt >= t1 && dt < t2) {
        return {it - m_atcfData.begin(), (dt - t1) / (t2 - t1)};
      }
    }
    gahm_throw_exception(
        "Could not find a suitable time record. Check record ordering.");
    return {0, 0.0};
  }
}

inline double Atcf::linearInterp(const double weight, const double v1,
                                 const double v2) {
  return (1.0 - weight) * v1 + weight * v2;
}

Atcf::StormParameters Atcf::getStormParameters(const Date &d) const {
  auto w = this->getCycleNumber(d);
  StormParameters s{};
  s.cycle = w.first;
  s.wtratio = w.second;
  if (s.cycle < 1) {
    s.central_pressure = m_atcfData.front().mslp();
    s.background_pressure = m_atcfData.front().pouter();
    s.vmax = m_atcfData.front().vmax();
    s.latitude = m_atcfData.front().lat();
    s.longitude = m_atcfData.front().lon();
    s.utrans = m_atcfData.front().uTrans();
    s.vtrans = m_atcfData.front().vTrans();
    s.uvtrans = m_atcfData.front().uvTrans();
  } else if (s.cycle > m_atcfData.size()) {
    s.central_pressure = m_atcfData.back().mslp();
    s.background_pressure = m_atcfData.back().pouter();
    s.vmax = m_atcfData.back().vmax();
    s.latitude = m_atcfData.back().lat();
    s.longitude = m_atcfData.back().lon();
    s.utrans = m_atcfData.back().uTrans();
    s.vtrans = m_atcfData.back().vTrans();
    s.uvtrans = m_atcfData.back().uvTrans();
  } else {
    s.central_pressure = Atcf::linearInterp(
        s.wtratio, m_atcfData[s.cycle].mslp(), m_atcfData[s.cycle + 1].mslp());
    s.background_pressure =
        Atcf::linearInterp(s.wtratio, m_atcfData[s.cycle].pouter(),
                           m_atcfData[s.cycle + 1].pouter());
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
int Atcf::generateMissingPressureData(
    const HurricanePressure::PressureMethod &method) {
  HurricanePressure hp(method);
  double vmax_global = 0.0;
  for (auto it = m_atcfData.begin(); it != m_atcfData.end(); ++it) {
    vmax_global = std::max(vmax_global, it->vmax());
    if (it->mslp() <= 0.0) {
      if (it == m_atcfData.begin()) {
        it->setMslp(
            HurricanePressure::computeInitialPressureEstimate(it->vmax()));
        m_assumptions->add(
            generate_assumption(Assumption::MINOR,
                                "Pressure data was assumed using initial "
                                "pressure estimate method. Record " +
                                    std::to_string(it - m_atcfData.begin())));
      } else {
        it->setMslp(hp.computePressure(it->vmax(), vmax_global,
                                       (it - 1)->vmax(), (it - 1)->mslp(),
                                       it->lat(), it->uvTrans()));
        m_assumptions->add(generate_assumption(
            Assumption::MINOR,
            "Pressure was computed as " + std::to_string(it->mslp()) +
                "mb using vmax=" + std::to_string(it->vmax()) +
                "m/s, vmax_global=" + std::to_string(vmax_global) +
                "m/s, previous_pressure=" + std::to_string((it - 1)->mslp()) +
                "mb, lat=" + std::to_string(it->lat()) + ", speed=" +
                std::to_string(it->uvTrans()) + "m/s, with method=" +
                HurricanePressure::pressureMethodString(hp.pressureMethod()) +
                " for record " + std::to_string(it - m_atcfData.begin())));
      }
    }
  }
  return 0;
}
