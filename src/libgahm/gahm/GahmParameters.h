//
// Created by Zach Cobell on 7/30/24.
//

#ifndef GAHM_GAHMPARAMETERS_H
#define GAHM_GAHMPARAMETERS_H

#include <tuple>

#include "datatypes/Point.h"
#include "datatypes/Vec.h"
#include "storm/StormTranslation.h"

namespace Gahm::Solver {

/**
 * @brief A struct to hold the parameters for the Gahm model.
 */
class GahmParamPack {
 public:
  constexpr GahmParamPack()
      : m_radius_to_max_winds(0),
        m_gahm_b(0),
        m_gahm_phi(0),
        m_holland_b(0),
        m_vortex_quad_10_tbl(0),
        m_vortex_max_10_tbl(0),
        m_vortex_quad_10_10(0),
        m_vortex_max_10_10(0),
        m_limited_quadrant_vmax(false) {}

  constexpr GahmParamPack(double radius_to_max_winds, double gahm_b,
                          double gahm_phi, double holland_b,
                          double vortex_quad_10_tbl, double vortex_max_10_tbl,
                          double vortex_quad_10_10, double vortex_max_10_10,
                          const Types::Vec& unit_vector_tbl,
                          bool limited_quadrant_vmax)
      : m_radius_to_max_winds(radius_to_max_winds),
        m_gahm_b(gahm_b),
        m_gahm_phi(gahm_phi),
        m_holland_b(holland_b),
        m_vortex_quad_10_tbl(vortex_quad_10_tbl),
        m_vortex_max_10_tbl(vortex_max_10_tbl),
        m_vortex_quad_10_10(vortex_quad_10_10),
        m_vortex_max_10_10(vortex_max_10_10),
        m_unit_vector_tbl(unit_vector_tbl),
        m_limited_quadrant_vmax(limited_quadrant_vmax) {}

  [[nodiscard]] constexpr auto radius_to_max_winds() const -> double {
    return m_radius_to_max_winds;
  }

  [[nodiscard]] constexpr auto gahm_b() const -> double { return m_gahm_b; }

  [[nodiscard]] constexpr auto gahm_phi() const -> double { return m_gahm_phi; }

  [[nodiscard]] constexpr auto holland_b() const -> double {
    return m_holland_b;
  }

  [[nodiscard]] constexpr auto vortex_quad_10_tbl() const -> double {
    return m_vortex_quad_10_tbl;
  }

  [[nodiscard]] constexpr auto vortex_max_10_tbl() const -> double {
    return m_vortex_max_10_tbl;
  }

  [[nodiscard]] constexpr auto vortex_quad_10_10() const -> double {
    return m_vortex_quad_10_10;
  }

  [[nodiscard]] constexpr auto vortex_max_10_10() const -> double {
    return m_vortex_max_10_10;
  }

  [[nodiscard]] constexpr auto unit_vector_tbl() const -> const Types::Vec& {
    return m_unit_vector_tbl;
  }

  [[nodiscard]] constexpr auto limited_quadrant_vmax() const -> bool {
    return m_limited_quadrant_vmax;
  }

 private:
  double m_radius_to_max_winds;
  double m_gahm_b;
  double m_gahm_phi;
  double m_holland_b;
  double m_vortex_quad_10_tbl;
  double m_vortex_max_10_tbl;
  double m_vortex_quad_10_10;
  double m_vortex_max_10_10;
  Types::Vec m_unit_vector_tbl;
  bool m_limited_quadrant_vmax;
};

auto GahmParameters(const Gahm::Storm::StormTranslation& translation,
                    const Gahm::Types::Point& eye_location,
                    const Gahm::Types::Vec& unit_vector,
                    double central_pressure, double background_pressure,
                    double v_max, double isotach_speed,
                    double isotach_radius) -> GahmParamPack;

}  // namespace Gahm::Solver
#endif  // GAHM_GAHMPARAMETERS_H
