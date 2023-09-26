// GNU General Public License v3.0
//
// This file is part of the GAHM model (https://github.com/adcirc/gahm).
// Copyright (c) 2023 ADCIRC Development Group.
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, version 3.
//
// This program is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program. If not, see <http://www.gnu.org/licenses/>.
//
// Author: Zach Cobell
// Contact: zcobell@thewaterinstitute.org
//
#ifndef GAHM_SRC_ATCFSNAP_H_
#define GAHM_SRC_ATCFSNAP_H_

#include <optional>

#include "atcf/AtcfIsotach.h"
#include "atcf/StormPosition.h"
#include "atcf/StormTranslation.h"
#include "datatypes/CircularArray.h"
#include "datatypes/Date.h"

namespace Gahm::Atcf {

/*
 * Class to hold a single ATCF snapshot in time and the
 * potentially multiple associated isotachs
 */
class AtcfSnap {
 public:
  enum BASIN { NONE, WP, IO, SH, CP, EP, AL, SL, kMaxValue = SL };

  AtcfSnap(Gahm::Atcf::AtcfSnap::BASIN basin, double central_pressure,
           double background_pressure, double radius_to_max_winds, double vmax,
           double vmax_boundary_layer, const Gahm::Datatypes::Date& date,
           int storm_id, std::string storm_name);

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

  [[nodiscard]] Gahm::Atcf::AtcfSnap::BASIN basin() const;
  void setBasin(Gahm::Atcf::AtcfSnap::BASIN basin);

  [[nodiscard]] const std::string& stormName() const;
  void setStormName(const std::string& stormName);

  [[nodiscard]] const std::vector<Gahm::Atcf::AtcfIsotach>& getIsotachs() const;
  std::vector<Gahm::Atcf::AtcfIsotach>& getIsotachs();

  [[nodiscard]] static Gahm::Atcf::AtcfSnap::BASIN basinFromString(
      const std::string& basin);

  [[nodiscard]] static std::string basinToString(
      Gahm::Atcf::AtcfSnap::BASIN basin);

  [[nodiscard]] size_t numberOfIsotachs() const;

  [[nodiscard]] const Gahm::Atcf::StormPosition& position() const;
  void setPosition(const Gahm::Atcf::StormPosition& position);

  [[nodiscard]] const Gahm::Atcf::StormTranslation& translation() const;
  void setTranslation(const Gahm::Atcf::StormTranslation& translation);

  [[nodiscard]] double hollandB() const;
  void setHollandB(double hollandB);

  void addIsotach(const Gahm::Atcf::AtcfIsotach& isotach);

  bool operator<(const Gahm::Atcf::AtcfSnap& other) const;

  [[nodiscard]] bool isValid() const;

  [[nodiscard]] std::string to_string(size_t cycle,
                                      const Gahm::Datatypes::Date& start_date,
                                      size_t isotach_index) const;

  void processIsotachRadii();

  [[nodiscard]] const Gahm::Datatypes::CircularArray<std::vector<double>, 4>&
  radii() const;

  void orderIsotachs();

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
  double m_holland_b;
  Gahm::Datatypes::Date m_date;
  int m_storm_id;
  BASIN m_basin;
  StormPosition m_position;
  StormTranslation m_translation;
  std::string m_storm_name;
  std::vector<Gahm::Atcf::AtcfIsotach> m_isotachs;
  Gahm::Datatypes::CircularArray<std::vector<double>, 4> m_radii;
};

}  // namespace Gahm::Atcf

#endif  // GAHM_SRC_ATCFSNAP_H_
