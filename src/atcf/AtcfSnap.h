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

#ifdef SWIG
#define NODISCARD
#else
#define NODISCARD [[nodiscard]]
#endif

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

  NODISCARD double centralPressure() const;
  void setCentralPressure(double centralPressure);

  NODISCARD double backgroundPressure() const;
  void setBackgroundPressure(double backgroundPressure);

  NODISCARD double radiusToMaxWinds() const;
  void setRadiusToMaxWinds(double radiusToMaxWinds);

  NODISCARD double vmax() const;
  void setVmax(double vmax);

  NODISCARD double vmaxBoundaryLayer() const;
  void setVmaxBoundaryLayer(double vmaxBoundaryLayer);

  NODISCARD const Gahm::Datatypes::Date& date() const;
  void setDate(const Gahm::Datatypes::Date& date);

  NODISCARD int stormId() const;
  void setStormId(int stormId);

  NODISCARD Gahm::Atcf::AtcfSnap::BASIN basin() const;
  void setBasin(Gahm::Atcf::AtcfSnap::BASIN basin);

  NODISCARD const std::string& stormName() const;
  void setStormName(const std::string& stormName);

  NODISCARD const std::vector<Gahm::Atcf::AtcfIsotach>& getIsotachs() const;
  std::vector<Gahm::Atcf::AtcfIsotach>& getIsotachs();

  NODISCARD static Gahm::Atcf::AtcfSnap::BASIN basinFromString(
      const std::string& basin);

  NODISCARD static std::string basinToString(Gahm::Atcf::AtcfSnap::BASIN basin);

  NODISCARD size_t numberOfIsotachs() const;

  NODISCARD const Gahm::Atcf::StormPosition& position() const;
  void setPosition(const Gahm::Atcf::StormPosition& position);

  NODISCARD const Gahm::Atcf::StormTranslation& translation() const;
  void setTranslation(const Gahm::Atcf::StormTranslation& translation);

  NODISCARD double hollandB() const;
  void setHollandB(double hollandB);

  void addIsotach(const Gahm::Atcf::AtcfIsotach& isotach);

  bool operator<(const Gahm::Atcf::AtcfSnap& other) const;

  NODISCARD bool isValid() const;

  NODISCARD std::string to_string(size_t cycle,
                                  const Gahm::Datatypes::Date& start_date,
                                  size_t isotach_index) const;

  void processIsotachRadii();

  NODISCARD const Gahm::Datatypes::CircularArray<std::vector<double>, 4>&
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
