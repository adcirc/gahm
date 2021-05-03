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

#include "Date.h"
#include "Isotach.h"

class AtcfLine {
 public:
  AtcfLine();

  static AtcfLine parseAtcfLine(const std::string &line);

  std::string basin() const;
  void setBasin(const std::string &basin);

  int cycloneNumber() const;
  void setCycloneNumber(int cycloneNumber);

  Date datetime() const;
  void setDatetime(const Date &datetime);

  Date referenceDatetime() const;
  void setReferenceDatetime(const Date &datetime);

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

  double mslp() const;
  void setMslp(double mslp);

  std::string maxDevelopmentLevel() const;
  void setMaxDevelopmentLevel(const std::string &maxDevelopmentLevel);

  Isotach *isotach(size_t index);
  const Isotach *cisotach(size_t index) const;
  void addIsotach(const Isotach &iso);
  void removeIsotach(size_t pos);
  size_t nIsotach() const;

  double pouter() const;
  void setPouter(double pouter);

  double radiusPouter() const;
  void setRadiusPouter(double radiusPouter);

  double radiusMaxWinds() const;
  void setRadiusMaxWinds(double radiusMaxWinds);

  double eyeDiameter() const;
  void setEyeDiameter(double eyeDiameter);

  char subregion() const;
  void setSubregion(char subregion);

  double maxSeas() const;
  void setMaxSeas(double maxSeas);

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

  Isotach *seas(size_t index);
  const Isotach *cseas(size_t index) const;

  friend std::ostream &operator<<(std::ostream &os, const AtcfLine &atcf);

  bool isNull() const;
  void setIsNull(bool isNull);

  double gusts() const;
  void setGusts(double gusts);

  static bool isSameForecastPeriod(const AtcfLine &a1, const AtcfLine &a2);

  bool operator<(const AtcfLine &a) const;

  void setStormTranslationVelocities(double u, double v, double uv);
  std::tuple<double, double, double> stormTranslationVelocities() const;

  double uvTrans() const;
  void setUvTrans(double uv);

  double uTrans() const;
  void setUTrans(double u);

  double vTrans() const;
  void setVTrans(double v);

 private:
  /// Basin where the cyclone occurs
  std::string m_basin;

  /// Cyclone number
  int m_cycloneNumber;

  /// Reference date/time
  Date m_refDatetime;

  /// Current date/time
  Date m_datetime;

  /// Technique number
  int m_technum;

  /// Technique name
  std::string m_techstring;

  /// Forecast hour
  int m_tau;

  /// Storm center latitude
  double m_lat;

  /// Storm center longitude
  double m_lon;

  /// Storm maximum wind velocity
  double m_vmax;

  /// Storm minimum sea level pressure
  double m_mslp;

  /// Maximum development level that the storm has achieved
  std::string m_maxDevelopmentLevel;

  /// Vector of storm isotachs
  std::vector<Isotach> m_isotach;

  /// Background pressure
  double m_pouter;

  /// Radius of last closed isobar
  double m_radiusPouter;

  /// Radius to maximum winds
  double m_radiusMaxWinds;

  /// Diameter of the storm eye
  double m_eyeDiameter;

  /// Maximum gust wind speed
  double m_gusts;

  /// Subregion where the storm occurs
  char m_subregion;

  /// Maximum wave heights
  double m_maxSeas;

  /// Forecaster initials
  std::string m_initials;

  /// Storm direction in degrees
  double m_stormDirection;

  /// Storm speed in m/s
  double m_stormSpeed;

  /// Storm name
  std::string m_stormName;

  /// System depth
  std::string m_systemDepth;

  /// "Isotachs" of wave heights
  std::vector<Isotach> m_seas;

  /// Is this record null
  bool m_null;

  /// Translation speed
  double m_uv;

  /// Translation velocity (u-direction)
  double m_u;

  /// Translation velocity (v-direction)
  double m_v;

  static std::vector<std::string> splitString(const std::string &line);

  template <typename T, typename = typename std::enable_if<
                            std::is_arithmetic<T>::value, T>::type>
  static T readValueCheckBlank(const std::string &line);

};

std::ostream &operator<<(std::ostream &os, const AtcfLine &atcf);

#endif  // ATCFLINE_H
