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
#include "StormMotion.h"
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

  //  int technum() const;
  //  void setTechnum(int technum);

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

  double vmaxAtBoundaryLayer() const;
  void setVmaxAtBoundaryLayer(double vmax);

  double centralPressure() const;
  void setCentralPressure(double centralPressure);

  std::string maxDevelopmentLevel() const;
  void setMaxDevelopmentLevel(const std::string &maxDevelopmentLevel);

  Gahm::Isotach *isotach(size_t index);
  const Gahm::Isotach *isotach(size_t index) const;

  void addIsotach(const Gahm::Isotach &iso);
  void removeIsotach(size_t pos);
  size_t nIsotach() const;

  std::vector<Gahm::Isotach> &isotachs();
  const std::vector<Gahm::Isotach> &isotachs() const;

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

  Gahm::StormMotion stormMotion() const;
  void setStormMotion(double speed, double direction);
  void setStormMotion(const Gahm::StormMotion &s);

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
    if (this->isotach(0)->windSpeed() < a.isotach(0)->windSpeed()) {
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

  double coriolis() const;
  void setCoriolis(double coriolis);

  double hollandB() const;
  void setHollandB(double b);

  const std::array<unsigned short, 4> &lastIsotach() const;
  void generateLastIsotach();

  const std::vector<double> *isotachRadii(int quad) const;

  void generateIsotachCache();

 private:
  static AtcfLine parseLine(const std::string &line, int formatid);

  /// Storm center latitude
  double m_lat;

  /// Storm center longitude
  double m_lon;

  /// Storm maximum wind velocity
  double m_vmax;

  /// Storm maximum wind velocity without translation velocity
  double m_vmax_at_boundary_layer;

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

  /// Translation speed
  Gahm::StormMotion m_stormTranslationSpeeds;

  /// Reference date/time
  Gahm::Date m_refDatetime;

  /// Current date/time
  Gahm::Date m_datetime;

  /// Last isotach position (guard against a zero isotach)
  std::array<unsigned short, 4> m_lastIsotach;

  /// Vector of storm isotachs
  std::vector<Isotach> m_isotach;

  /// Cache variable for isotachs
  CircularArray<std::vector<double>, 4> m_isotachRadiiCache;

  /// Cyclone number
  int m_cycloneNumber;

  /// Forecast hour
  int m_tau;

  // Technique Number
  // int m_technum;

  /// Basin
  std::string m_basin;

  /// Initials
  std::string m_initials;

  /// System Depth
  std::string m_systemDepth;

  /// Storm name
  std::string m_stormName;

  /// Technique String
  std::string m_techstring;

  /// Maximum development level that the storm has achieved
  std::string m_maxDevelopmentLevel;

  /// Subregion
  char m_subregion;

  /// Is this record null
  bool m_null;

  static std::vector<std::string> splitString(const std::string &line);

#ifndef SWIG
  template <typename T, typename = typename std::enable_if<
                            std::is_fundamental<T>::value, T>::type>
  static T readValueCheckBlank(const std::string &line);

#endif
};
}  // namespace Gahm

std::ostream &operator<<(std::ostream &os, const Gahm::AtcfLine &atcf);

#endif  // ATCFLINE_H
