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
