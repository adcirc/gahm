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

#include <iostream>
#include <string>
#include <tuple>
#include <vector>
#include "Date.h"
#include "Isotach.h"
#include "boost/algorithm/string/trim.hpp"

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
  std::string m_basin;
  int m_cycloneNumber;
  Date m_refDatetime;
  Date m_datetime;
  int m_technum;
  std::string m_techstring;
  int m_tau;
  double m_lat;
  double m_lon;
  double m_vmax;
  double m_mslp;
  std::string m_maxDevelopmentLevel;
  std::vector<Isotach> m_isotach;
  double m_pouter;
  double m_radiusPouter;
  double m_radiusMaxWinds;
  double m_eyeDiameter;
  double m_gusts;
  char m_subregion;
  double m_maxSeas;
  std::string m_initials;
  double m_stormDirection;
  double m_stormSpeed;
  std::string m_stormName;
  std::string m_systemDepth;
  std::vector<Isotach> m_seas;
  bool m_null;

  double m_uv;
  double m_u;
  double m_v;

  static std::vector<std::string> splitString(const std::string &line);

  template <typename T, typename = typename std::enable_if<
                            std::is_arithmetic<T>::value, T>::type>
  static T readValueCheckBlank(const std::string &line) {
    if (boost::trim_copy(line) == "") {
      return 0;
    } else {
      if (std::is_integral<T>::value) {
        return stoi(line);
      } else if (std::is_floating_point<T>::value) {
        return stod(line);
      }
    }
  }
};

std::ostream &operator<<(std::ostream &os, const AtcfLine &atcf);

#endif  // ATCFLINE_H
