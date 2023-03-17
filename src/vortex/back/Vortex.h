//
// Created by Zach Cobell on 3/14/23.
//

#ifndef GAHM_VORTEX_H
#define GAHM_VORTEX_H

#include <array>
#include <tuple>
#include <vector>

#include "atcf/AtcfFile.h"
#include "datatypes/PointCloud.h"
#include "datatypes/VortexSolution.h"

namespace Gahm::Vortex {
class Vortex {
 public:
  Vortex(const Gahm::Atcf::AtcfFile *atcf, const std::vector<double> &x,
         const std::vector<double> &y);
  Vortex(const Gahm::Atcf::AtcfFile *atcf, Datatypes::PointCloud points);

  [[nodiscard]] std::tuple<std::vector<Atcf::AtcfSnap>::const_iterator, double>
  selectTimeIndex(const Gahm::Datatypes::Date &time) const;

  struct t_distance_and_azimuth {
    t_distance_and_azimuth() = default;
    t_distance_and_azimuth(double distance, double azimuth)
        : distance(distance), azimuth(azimuth) {}
    double distance;
    double azimuth;
  };

  struct t_quadrant_and_isotach {
    t_quadrant_and_isotach() = default;
    t_quadrant_and_isotach(int i_quadrant, double i_quadrant_weight,
                           int i_isotach, double i_isotach_weight,
                           double i_radius_inner, double i_radius_outer)
        : quadrant(i_quadrant),
          isotach(i_isotach),
          quadrant_weight(i_quadrant_weight),
          isotach_weight(i_isotach_weight),
          radius_inner(i_radius_inner),
          radius_outer(i_radius_outer) {}
    int quadrant;
    int isotach;
    double quadrant_weight;
    double isotach_weight;
    double radius_inner;
    double radius_outer;
  };

  struct t_storm_parameters {
    double vmax_boundary_layer;
    double radius_to_max_wind;
    double holland_b;
    double radius_to_max_winds_true;
  };

  [[nodiscard]] t_distance_and_azimuth computeDistanceAndAzimuth(
      const std::vector<Atcf::AtcfSnap>::const_iterator &timeIndex,
      double weight, size_t index, long position = -1) const;

  [[nodiscard]] std::vector<t_distance_and_azimuth>
  interpolateDistanceAndAzimuth(
      const std::vector<Atcf::AtcfSnap>::const_iterator &timeIndex,
      double weight) const;

  [[nodiscard]] Datatypes::VortexSolution solve(
      const Datatypes::Date &time) const;

  [[nodiscard]] Datatypes::VortexSolution::t_uvp solveEquations(
      const std::vector<Atcf::AtcfSnap>::const_iterator &thisSnap,
      const std::vector<Atcf::AtcfSnap>::const_iterator &nextSnap,
      double weight, size_t pointIndex,
      const t_distance_and_azimuth &distance_azimuth) const;

  [[nodiscard]] static t_quadrant_and_isotach findQuadrantAndIsotach(
      const Atcf::AtcfSnap &snap, const t_distance_and_azimuth &da);

  [[nodiscard]] const Gahm::Atcf::AtcfFile *atcf() const { return m_atcf; }

  [[nodiscard]] const Datatypes::PointCloud &points() const { return m_points; }

  static std::tuple<int, double> findBaseQuadrant(double azimuth);
  static std::tuple<int, double, double, double> findBaseIsotach(
      const Atcf::AtcfSnap &snap, int quadrant, double distance);

  static t_distance_and_azimuth distanceAndAzimuth(
      const Datatypes::Point &p, const Datatypes::Point &storm);

 private:
  void computeAllDistanceAndAzimuth();

  [[nodiscard]] static t_storm_parameters interpolateAlongIsotach(
      const Atcf::AtcfSnap &snap, const t_quadrant_and_isotach &factors,
      int quadrant_position);

  [[nodiscard]] static t_storm_parameters interpolateAlongRadius(
      t_storm_parameters &p0, t_storm_parameters &p1,
      t_quadrant_and_isotach &factors);

  [[nodiscard]] static t_storm_parameters interpolateParametersInTime(
      const t_storm_parameters &p0, const t_storm_parameters &p1,
      double time_weight);

  [[nodiscard]] static t_storm_parameters getStormParameters(
      const Atcf::AtcfSnap &snap, const t_distance_and_azimuth &da);

  const Gahm::Atcf::AtcfFile *m_atcf;
  Datatypes::PointCloud m_points;
  std::vector<std::vector<t_distance_and_azimuth>> m_distance_and_azimuth;
};
}  // namespace Gahm::Vortex

std::ostream &operator<<(std::ostream &os,
                         const Gahm::Vortex::Vortex::t_storm_parameters &p);

#endif  // GAHM_VORTEX_H
