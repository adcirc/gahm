//
// Created by Zach Cobell on 3/16/23.
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
    double distance;
    double azimuth;
    int isotach_adjacent;
    double isotach_adjacent_weight;
  };
  static t_point_position getPointPosition(
      const Datatypes::Point &point, const Atcf::StormPosition &storm_position,
      const Atcf::AtcfSnap &snap);

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
