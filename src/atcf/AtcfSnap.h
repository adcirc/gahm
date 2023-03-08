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
#ifndef GAHM_SRC_ATCFSNAP_H_
#define GAHM_SRC_ATCFSNAP_H_

#include <optional>

#include "atcf/AtcfIsotach.h"
#include "datatypes/CircularArray.h"
#include "datatypes/Date.h"

namespace Gahm::Atcf {

/*
 * Class to hold a single ATCF snapshot in time and the
 * potentially multiple associated isotachs
 */
class AtcfSnap {
 public:
  enum BASIN { NONE, WP, IO, SH, CP, EP, AL, SL };

  AtcfSnap(BASIN basin, double central_pressure, double background_pressure,
           double radius_to_max_winds, double vmax, double vmax_boundary_layer,
           const Gahm::Datatypes::Date& date, int storm_id,
           std::string storm_name);

  static std::optional<AtcfSnap> parseAtcfSnap(const std::string& line);

  [[nodiscard]] double centralPressure() const;
  void setCentralPressure(double centralPressure);

  [[nodiscard]] double backgroundPressure() const;
  void setBackgroundPressure(double backgroundPressure);

  [[nodiscard]] double radiusToMaxWinds() const;
  void setRadiusToMaxWinds(double radiusToMaxWinds);

  [[nodiscard]] double vmax() const;
  void setVmax(double vmax);

  [[nodiscard]] double vmaxBoundaryLayer() const;
  void setVmaxBoundaryLayer(double vmaxBoundaryLayer);

  [[nodiscard]] const Gahm::Datatypes::Date& date() const;
  void setDate(const Gahm::Datatypes::Date& date);

  [[nodiscard]] int stormId() const;
  void setStormId(int stormId);

  [[nodiscard]] BASIN basin() const;
  void setBasin(BASIN basin);

  [[nodiscard]] const std::string& stormName() const;
  void setStormName(const std::string& stormName);

  [[nodiscard]] const std::vector<AtcfIsotach>& isotachs() const;
  std::vector<AtcfIsotach>& isotachs();

  [[nodiscard]] const Datatypes::CircularArray<double, 4>& radii() const;

  [[nodiscard]] static AtcfSnap::BASIN basinFromString(
      const std::string& basin);

  [[nodiscard]] size_t numberOfIsotachs() const;

  void addIsotach(const AtcfIsotach& isotach);

  bool operator<(const AtcfSnap& other) const;

 private:
  static Gahm::Datatypes::Date parseDate(const std::string& date_str,
                                         const std::string& tau_str);

  static Gahm::Atcf::AtcfIsotach parseIsotach(
      const std::vector<std::string>& line);

  double m_central_pressure;
  double m_background_pressure;
  double m_radius_to_max_winds;
  double m_vmax;
  double m_vmax_boundary_layer;
  Gahm::Datatypes::Date m_date;
  int m_storm_id;
  BASIN m_basin;
  std::string m_storm_name;
  std::vector<AtcfIsotach> m_isotachs;
  Datatypes::CircularArray<double, 4> m_radii;
};

}  // namespace Gahm::Atcf

#endif  // GAHM_SRC_ATCFSNAP_H_
