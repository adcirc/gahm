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
#include <utility>
#include <vector>

#include "atcf/AtcfFile.h"
#include "datatypes/Date.h"
#include "datatypes/PointCloud.h"
#include "datatypes/VortexSolution.h"

namespace Gahm {

class Vortex {
 public:
  Vortex(const Atcf::AtcfFile *atcfFile, Datatypes::PointCloud points);

  Datatypes::VortexSolution solve(const Gahm::Datatypes::Date &date);

  [[nodiscard]] std::tuple<std::vector<Atcf::AtcfSnap>::const_iterator, double>
  selectTime(const Datatypes::Date &date) const;

  struct t_point_position {
    int isotach;
    int quadrant;
    double isotach_weight;
    double quadrant_weight;
    int isotach_adjacent;
    double isotach_adjacent_weight;
  };
  static t_point_position getPointPosition(const Atcf::AtcfSnap &snap,
                                           double distance, double azimuth);

  static std::pair<int, double> getBaseQuadrant(double angle);
  static std::pair<int, double> getBaseIsotach(double distance, int quadrant,
                                               const Atcf::AtcfSnap &snap);

  [[nodiscard]] static double friction_angle(double radius,
                                             double radius_to_max_winds);
  [[nodiscard]] static std::tuple<double, double> rotate_winds(double u,
                                                               double v,
                                                               double angle,
                                                               double latitude);

 private:
  struct t_parameter_pack {
    double radius_to_max_wind;
    double radius_to_max_wind_true;
    double vmax_at_boundary_layer;
    double isotach_speed_at_boundary_layer;
    double holland_b;
  };
  static t_parameter_pack getParameterPack(
      const t_point_position &point_position, const Atcf::AtcfSnap &snap);

  static t_parameter_pack isotachToParameterPack(
      const Atcf::AtcfIsotach &isotach, int quadrant);

  static t_parameter_pack interpolateParameterPackIsotach(
      const t_point_position &point_position, const Atcf::AtcfSnap &snap,
      int quadrant_index);

  static t_parameter_pack interpolateParameterPackQuadrant(
      const t_point_position &point_position, const Atcf::AtcfSnap &snap);

  static t_parameter_pack interpolateParameterPack(const t_parameter_pack &p0,
                                                   const t_parameter_pack &p1,
                                                   double weight);
  static t_parameter_pack interpolateParameterPackRadial(
      const Vortex::t_parameter_pack &p0, const Vortex::t_parameter_pack &p1,
      double azimuth);

  const Atcf::AtcfFile *m_atcfFile;
  Datatypes::PointCloud m_points;
};
}  // namespace Gahm
#endif  // GAHM_VORTEX_H
