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
#ifndef GAHM_SRC_ATCF_ATCFQUADRANT_H_
#define GAHM_SRC_ATCF_ATCFQUADRANT_H_

#include "datatypes/CircularArray.h"
#include "physical/Constants.h"

namespace Gahm::Atcf {
class AtcfQuadrant {
 public:
  explicit AtcfQuadrant(int quadrant_index = 0, double isotach_radius = 0.0,
                        double radius_to_max_wind_speed = 0.0,
                        double gahm_holland_b = 0.0,
                        double isotach_speed_at_boundary_layer = 0.0)
      : m_quadrant_index(quadrant_index),
        m_isotach_radius(isotach_radius),
        m_radius_to_max_wind_speed(radius_to_max_wind_speed),
        m_gahm_holland_b(gahm_holland_b),
        m_isotach_speed_at_boundary_layer(isotach_speed_at_boundary_layer) {}

  [[nodiscard]] double getIsotachRadius() const { return m_isotach_radius; }
  [[nodiscard]] double getRadiusToMaxWindSpeed() const {
    return m_radius_to_max_wind_speed;
  }
  [[nodiscard]] double getGahmHollandB() const { return m_gahm_holland_b; }
  [[nodiscard]] double getIsotachSpeedAtBoundaryLayer() const {
    return m_isotach_speed_at_boundary_layer;
  }
  [[nodiscard]] int getQuadrantIndex() const { return m_quadrant_index; }

  void setIsotachRadius(double isotach_radius) {
    m_isotach_radius = isotach_radius;
  }
  void setRadiusToMaxWindSpeed(double radius_to_max_wind_speed) {
    m_radius_to_max_wind_speed = radius_to_max_wind_speed;
  }
  void setGahmHollandB(double gahm_holland_b) {
    m_gahm_holland_b = gahm_holland_b;
  }
  void setIsotachSpeedAtBoundaryLayer(double isotach_speed_at_boundary_layer) {
    m_isotach_speed_at_boundary_layer = isotach_speed_at_boundary_layer;
  }
  void setQuadrantIndex(int quadrant_index) {
    m_quadrant_index = quadrant_index;
  }

  static constexpr double quadrant_angle(int quadrant_index) {
    return s_quadrant_angles[quadrant_index];
  }

 private:
  int m_quadrant_index;
  double m_isotach_radius;
  double m_radius_to_max_wind_speed;
  double m_gahm_holland_b;
  double m_isotach_speed_at_boundary_layer;

  static constexpr auto s_quadrant_angles = Datatypes::CircularArray<double, 4>(
      {45.0 * Gahm::Physical::Constants::deg2rad(),
       135.0 * Gahm::Physical::Constants::deg2rad(),
       225.0 * Gahm::Physical::Constants::deg2rad(),
       315.0 * Gahm::Physical::Constants::deg2rad()});
};
}  // namespace Gahm::Atcf
#endif  // GAHM3_SRC_ATCF_ATCFQUADRANT_H_
