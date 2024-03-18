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
#ifndef GAHM_VORTEX_H
#define GAHM_VORTEX_H

#include <tuple>
#include <vector>

#include "atcf/AtcfFile.h"
#include "atcf/AtcfIsotach.h"
#include "atcf/AtcfSnap.h"
#include "atcf/StormPosition.h"
#include "atcf/StormTranslation.h"
#include "datatypes/Date.h"
#include "datatypes/PointCloud.h"
#include "datatypes/PointPosition.h"
#include "datatypes/VortexSolution.h"

#ifdef SWIG
#define NODISCARD
#else
#define NODISCARD [[nodiscard]]
#endif

namespace Gahm {

class Vortex {
 public:
  Vortex(const Atcf::AtcfFile *atcfFile, Datatypes::PointCloud points);

  auto solve(const Gahm::Datatypes::Date &date) -> Datatypes::VortexSolution;

  NODISCARD auto selectTime(const Datatypes::Date &date) const
      -> std::tuple<std::vector<Atcf::AtcfSnap>::const_iterator, double>;

  static auto getPointPosition(const Atcf::AtcfSnap &snap, double distance,
                               double azimuth)
      -> Gahm::Datatypes::PointPosition;

  static auto getBaseQuadrant(double angle) -> std::tuple<int, double>;

  static auto getBaseIsotach(double distance, int quadrant,
                             const Atcf::AtcfSnap &snap)
      -> std::tuple<int, double>;

  NODISCARD static auto friction_angle(double radius,
                                       double radius_to_max_winds) -> double;
  NODISCARD static auto rotate_winds(double u_vector, double v_vector,
                                     double angle, double latitude)
      -> std::tuple<double, double>;

  NODISCARD static auto computeTranslationVelocityComponents(
      double wind_speed, double vmax_at_boundary_layer,
      const Atcf::StormTranslation &translation) -> std::tuple<double, double>;

  NODISCARD static auto decomposeWindVector(double wind_speed, double azimuth,
                                            double latitude)
      -> std::tuple<double, double>;

 private:
  struct t_parameter_pack {
    double radius_to_max_wind;
    double radius_to_max_wind_true;
    double vmax_at_boundary_layer;
    double isotach_speed_at_boundary_layer;
    double holland_b;
  };

  struct t_vortex_state {
    std::vector<Atcf::AtcfSnap>::const_iterator time_it;
    std::vector<Atcf::AtcfSnap>::const_iterator time_it_next;
    double time_weight;
    Atcf::StormPosition current_storm_position;
    Atcf::StormTranslation current_storm_translation;
    double background_pressure;
    double central_pressure;
    double f_coriolis;
    Datatypes::Date date;
  };

  static auto solveVortexPoint(const Vortex::t_vortex_state &state,
                               const Datatypes::Point &point) -> Datatypes::Uvp;

  static auto computeVortexWindVector(const t_vortex_state &state,
                                      const t_parameter_pack &pack,
                                      const double distance,
                                      const double azimuth)
      -> std::tuple<double, double>;

  static auto getInterpolatedPack(const t_vortex_state &state,
                                  const double distance, const double azimuth)
      -> t_parameter_pack;

  static auto getParameterPack(
      const Gahm::Datatypes::PointPosition &point_position,
      const Atcf::AtcfSnap &snap) -> t_parameter_pack;

  static auto isotachToParameterPack(const Atcf::AtcfIsotach &isotach,
                                     int quadrant) -> t_parameter_pack;

  static auto interpolateParameterPackIsotach(
      const Gahm::Datatypes::PointPosition &point_position,
      const Atcf::AtcfSnap &snap, int quadrant_index) -> t_parameter_pack;

  static auto interpolateParameterPackQuadrant(
      const Gahm::Datatypes::PointPosition &point_position,
      const Atcf::AtcfSnap &snap) -> t_parameter_pack;

  static auto interpolateParameterPack(const t_parameter_pack &pack0,
                                       const t_parameter_pack &pack1,
                                       double weight) -> t_parameter_pack;
  static auto interpolateParameterPackRadial(
      const Vortex::t_parameter_pack &pack0,
      const Vortex::t_parameter_pack &pack1, double azimuth)
      -> t_parameter_pack;

  const Atcf::AtcfFile *m_atcfFile;
  Datatypes::PointCloud m_points;
};
}  // namespace Gahm
#endif  // GAHM_VORTEX_H
