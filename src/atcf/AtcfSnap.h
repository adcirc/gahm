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

#include <cstddef>
#include <optional>
#include <string>
#include <vector>

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

  AtcfSnap() = default;

  AtcfSnap(Gahm::Atcf::AtcfSnap::BASIN basin, double central_pressure,
           double background_pressure, double radius_to_max_winds, double vmax,
           const Gahm::Datatypes::Date& date, int storm_id,
           std::string storm_name);

  static auto parseAtcfSnap(const std::string& line) -> std::optional<AtcfSnap>;

  NODISCARD auto centralPressure() const -> double;
  void setCentralPressure(double centralPressure);

  NODISCARD auto backgroundPressure() const -> double;
  void setBackgroundPressure(double backgroundPressure);

  NODISCARD auto radiusToMaxWinds() const -> double;
  void setRadiusToMaxWinds(double radiusToMaxWinds);

  NODISCARD auto vmax() const -> double;
  void setVmax(double vmax);

  NODISCARD auto vmaxBoundaryLayer() const -> double;
  void setVmaxBoundaryLayer(double vmaxBoundaryLayer);

  NODISCARD const Gahm::Datatypes::Date& date() const;
  void setDate(const Gahm::Datatypes::Date& date);

  NODISCARD auto stormId() const -> int;
  void setStormId(int stormId);

  NODISCARD auto basin() const -> Gahm::Atcf::AtcfSnap::BASIN;
  void setBasin(Gahm::Atcf::AtcfSnap::BASIN basin);

  NODISCARD const std::string& stormName() const;
  void setStormName(const std::string& stormName);

#ifndef SWIG
  NODISCARD auto getIsotachs() const
      -> const std::vector<Gahm::Atcf::AtcfIsotach>&;
#endif
  auto getIsotachs() -> std::vector<Gahm::Atcf::AtcfIsotach>&;

  NODISCARD static auto basinFromString(const std::string& basin)
      -> Gahm::Atcf::AtcfSnap::BASIN;

  NODISCARD static auto basinToString(Gahm::Atcf::AtcfSnap::BASIN basin)
      -> std::string;

  NODISCARD auto numberOfIsotachs() const -> size_t;

  NODISCARD const Gahm::Atcf::StormPosition& position() const;
  void setPosition(const Gahm::Atcf::StormPosition& position);

  NODISCARD const Gahm::Atcf::StormTranslation& translation() const;
  void setTranslation(const Gahm::Atcf::StormTranslation& translation);

  NODISCARD auto hollandB() const -> double;
  void setHollandB(double hollandB);

  void addIsotach(const Gahm::Atcf::AtcfIsotach& isotach);

  auto operator<(const Gahm::Atcf::AtcfSnap& other) const -> bool;

  NODISCARD auto isValid() const -> bool;

  NODISCARD auto to_string(size_t cycle,
                           const Gahm::Datatypes::Date& start_date,
                           size_t isotach_index) const -> std::string;

  void processIsotachRadii();

  NODISCARD const Gahm::Datatypes::CircularArray<std::vector<double>, 4>&
  radii() const;

  void orderIsotachs();

 private:
  static auto parseDate(const std::string& date_str, const std::string& tau_str)
      -> Gahm::Datatypes::Date;

  static auto parseIsotach(const std::vector<std::string>& line)
      -> Gahm::Atcf::AtcfIsotach;

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
