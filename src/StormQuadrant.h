//
// Created by Zach Cobell on 5/8/22.
//

#ifndef GAHM2_SRC_STORMQUADRANT_H_
#define GAHM2_SRC_STORMQUADRANT_H_

#include <tuple>

namespace Gahm {
class StormQuadrant {
 public:
  StormQuadrant()
      : m_isotach_radius(0.0),
        m_radius_to_max_winds(0.0),
        m_gahm_holland_b(0.0),
        m_vmax_at_boundary_layer(0.0),
        m_isotach_speed_at_boundary_layer(0.0),
        m_index(0) {}

  StormQuadrant(int quad_index, double isotach_radius,
                double radius_to_max_winds, double holland_b,
                double vmax_at_boundary_layer)
      : m_index(quad_index),
        m_isotach_radius(isotach_radius),
        m_radius_to_max_winds(radius_to_max_winds),
        m_gahm_holland_b(holland_b),
        m_vmax_at_boundary_layer(vmax_at_boundary_layer),
        m_isotach_speed_at_boundary_layer(0.0) {}

  void set_vmax_at_boundary_layer(const double vmax) {
    m_vmax_at_boundary_layer = vmax;
  }
  [[nodiscard]] double vmax_at_boundary_layer() const {
    return m_vmax_at_boundary_layer;
  }

  void set_isotach_radius(const double radius) { m_isotach_radius = radius; }

  [[nodiscard]] double isotach_radius() const { return m_isotach_radius; }

  void set_gahm_holland_b(const double b) { m_gahm_holland_b = b; }

  [[nodiscard]] double gahm_holland_b() const { return m_gahm_holland_b; }

  void set_radius_to_max_winds(const double rmax) {
    m_radius_to_max_winds = rmax;
  }

  [[nodiscard]] double radius_to_max_winds() const {
    return m_radius_to_max_winds;
  }

  [[nodiscard]] double isotach_speed_at_boundary_layer() const {
    return m_isotach_speed_at_boundary_layer;
  }

  void set_isotach_speed_at_boundary_layer(double speed) {
    m_isotach_speed_at_boundary_layer = speed;
  }

  static std::tuple<int, double> getBaseQuadrant(double angle);

  [[nodiscard]] int index() const { return m_index; }

 private:
  double m_isotach_radius;
  double m_radius_to_max_winds;
  double m_gahm_holland_b;
  double m_vmax_at_boundary_layer;
  double m_isotach_speed_at_boundary_layer;
  int m_index;
};
}  // namespace Gahm
#endif  // GAHM2_SRC_STORMQUADRANT_H_
