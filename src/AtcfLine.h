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
#ifndef ATCFLINE_H
#define ATCFLINE_H

#include <array>
#include <iostream>
#include <string>
#include <tuple>
#include <vector>

#include "Constants.h"
#include "Date.h"
#include "Isotach.h"
namespace Gahm {
class AtcfLine {
 public:
  AtcfLine();

  static AtcfLine parseBestTrackLine(const std::string &line);

  static AtcfLine parseAswipLine(const std::string &line);

  std::string basin() const;
  void setBasin(const std::string &basin);

  int cycloneNumber() const;
  void setCycloneNumber(int cycloneNumber);

  Gahm::Date datetime() const;
  void setDatetime(const Gahm::Date &datetime);

  Gahm::Date referenceDatetime() const;
  void setReferenceDatetime(const Gahm::Date &datetime);

  int technum() const;
  void setTechnum(int technum);

  std::string techstring() const;
  void setTechstring(const std::string &techstring);

  int tau() const;
  void setTau(int tau);

  double lat() const;
  void setLat(double lat);

  double lon() const;
  void setLon(double lon);

  double vmax() const;
  void setVmax(double vmax);

  double centralPressure() const;
  void setCentralPressure(double centralPressure);

  std::string maxDevelopmentLevel() const;
  void setMaxDevelopmentLevel(const std::string &maxDevelopmentLevel);

  Gahm::Isotach &isotach(size_t index);
  const Gahm::Isotach &isotach(size_t index) const;

  void addIsotach(const Gahm::Isotach &iso);
  void removeIsotach(size_t pos);
  size_t nIsotach() const;

  const std::vector<Gahm::Isotach> *isotachs() const;

  double lastClosedIsobar() const;
  void setLastClosedIsobar(double lastClosedIsobar);

  double radiusLastClosedIsobar() const;
  void setRadiusLastClosedIsobar(double radiusLastClosedIsobar);

  double radiusMaxWinds() const;
  void setRadiusMaxWinds(double radiusMaxWinds);

  double eyeDiameter() const;
  void setEyeDiameter(double eyeDiameter);

  char subregion() const;
  void setSubregion(char subregion);

  std::string initials() const;
  void setInitials(const std::string &initials);

  double stormDirection() const;
  void setStormDirection(double stormDirection);

  double stormSpeed() const;
  void setStormSpeed(double stormSpeed);

  std::string stormName() const;
  void setStormName(const std::string &storm_name);

  std::string systemDepth() const;
  void setSystemDepth(const std::string &systemDepth);

  bool isNull() const;
  void setIsNull(bool isNull);

  double gusts() const;
  void setGusts(double gusts);

  static bool isSameForecastPeriod(const Gahm::AtcfLine &a1,
                                   const Gahm::AtcfLine &a2);

  bool operator<(const Gahm::AtcfLine &a) const {
    if (this->datetime() < a.datetime()) return true;
    if (this->datetime() > a.datetime()) return false;
    if (this->nIsotach() == 0 && a.nIsotach() == 0) return false;
    if (this->nIsotach() > 0 && a.nIsotach() == 0) return true;
    if (this->nIsotach() == 0 && a.nIsotach() > 0) return false;
    if (this->isotach(0).windSpeed() < a.isotach(0).windSpeed()) {
      return true;
    }
    return false;
  }

#ifndef SWIG
  struct atcfLineLessThan {
    bool operator()(const Gahm::AtcfLine &left, const long right) {
      return left.datetime().toSeconds() < right;
    }
    bool operator()(const long left, const Gahm::AtcfLine &right) {
      return left < right.datetime().toSeconds();
    }
  };
#endif

  void setStormTranslationVelocities(double u, double v, double uv);
  std::tuple<double, double, double> stormTranslationVelocities() const;

  double uvTrans() const;
  void setUvTrans(double uv);

  double uTrans() const;
  void setUTrans(double u);

  double vTrans() const;
  void setVTrans(double v);

  double coriolis() const;
  void setCoriolis(double coriolis);

  double hollandB() const;
  void setHollandB(double b);

  double vmaxBl() const;
  void setVmaxBl(double v);

  const std::array<size_t, 4> &lastIsotach() const;
  void generateLastIsotach();

  std::vector<double> isotachRadii(int quad) const;

 private:
  static AtcfLine parseLine(const std::string &line, int formatid);

  /// Is this record null
  bool m_null;

  /// Cyclone number
  int m_cycloneNumber;

  /// Technique number
  int m_technum;

  /// Forecast hour
  int m_tau;

  /// Storm center latitude
  double m_lat;

  /// Storm center longitude
  double m_lon;

  /// Storm maximum wind velocity
  double m_vmax;

  /// Storm vmax at boundary layer
  double m_vmaxbl;

  /// Holland B
  double m_hollandB;

  /// Storm minimum sea level pressure
  double m_centralPressure;

  /// Coriolis Force
  double m_coriolis;

  /// Background pressure
  double m_lastClosedIsobar;

  /// Radius of last closed isobar
  double m_radiusLastClosedIsobar;

  /// Radius to maximum winds
  double m_radiusMaxWinds;

  /// Diameter of the storm eye
  double m_eyeDiameter;

  /// Maximum gust wind speed
  double m_gusts;

  /// Storm direction in degrees
  double m_stormDirection;

  /// Storm speed in m/s
  double m_stormSpeed;

  /// Translation speed
  double m_uv;

  /// Translation velocity (u-direction)
  double m_u;

  /// Translation velocity (v-direction)
  double m_v;

  /// Reference date/time
  Gahm::Date m_refDatetime;

  /// Current date/time
  Gahm::Date m_datetime;

  /// Vector of storm isotachs
  std::vector<Isotach> m_isotach;

  /// Last isotach position (guard against a zero isotach)
  std::array<size_t, 4> m_lastIsotach;

  /// Basin where the cyclone occurs
  std::string m_basin;

  /// Technique name
  std::string m_techstring;

  /// System depth
  std::string m_systemDepth;

  /// Storm name
  std::string m_stormName;

  /// Maximum development level that the storm has achieved
  std::string m_maxDevelopmentLevel;

  /// Forecaster initials
  std::string m_initials;

  /// Subregion where the storm occurs
  char m_subregion;

  static std::vector<std::string> splitString(const std::string &line);

#ifndef SWIG
  template <typename T, typename = typename std::enable_if<
                            std::is_arithmetic<T>::value, T>::type>
  static T readValueCheckBlank(const std::string &line);
#endif
};
}  // namespace Gahm

std::ostream &operator<<(std::ostream &os, const Gahm::AtcfLine &atcf);

#endif  // ATCFLINE_H
