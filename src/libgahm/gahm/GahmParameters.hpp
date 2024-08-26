//
// Created by Zach Cobell on 7/30/24.
//

#ifndef GAHM_GAHMPARAMETERS_HPP
#define GAHM_GAHMPARAMETERS_HPP

#include "datatypes/Point.hpp"
#include "datatypes/Vec.hpp"
#include "storm/StormTranslation.hpp"
#include "util/Interpolation.hpp"

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
        m_vortex_max_10_10(0) {}

  static constexpr auto interpolate(const GahmParamPack& gahm_params_1,
                                    const GahmParamPack& gahm_params_2,
                                    const double& weight) -> GahmParamPack {
    const auto interp_radius_to_max_winds = Util::Interpolation::linear(
        gahm_params_1.radius_to_max_winds(),
        gahm_params_2.radius_to_max_winds(), weight);
    const auto interp_gahm_b = Util::Interpolation::linear(
        gahm_params_1.gahm_b(), gahm_params_2.gahm_b(), weight);
    const auto interp_gahm_phi = Util::Interpolation::linear(
        gahm_params_1.gahm_phi(), gahm_params_2.gahm_phi(), weight);
    const auto interp_holland_b = Util::Interpolation::linear(
        gahm_params_1.holland_b(), gahm_params_2.holland_b(), weight);
    const auto interp_vortex_quad_10_tbl =
        Util::Interpolation::linear(gahm_params_1.vortex_quad_10_tbl(),
                                    gahm_params_2.vortex_quad_10_tbl(), weight);
    const auto interp_vortex_max_10_tbl =
        Util::Interpolation::linear(gahm_params_1.vortex_max_10_tbl(),
                                    gahm_params_2.vortex_max_10_tbl(), weight);
    const auto interp_vortex_quad_10_10 =
        Util::Interpolation::linear(gahm_params_1.vortex_quad_10_10(),
                                    gahm_params_2.vortex_quad_10_10(), weight);
    const auto interp_vortex_max_10_10 =
        Util::Interpolation::linear(gahm_params_1.vortex_max_10_10(),
                                    gahm_params_2.vortex_max_10_10(), weight);
    const auto interp_unit_vector_tbl =
        Util::Interpolation::linear(gahm_params_1.unit_vector_tbl(),
                                    gahm_params_2.unit_vector_tbl(), weight);

    return {interp_radius_to_max_winds,
            interp_gahm_b,
            interp_gahm_phi,
            interp_holland_b,
            interp_vortex_quad_10_tbl,
            interp_vortex_max_10_tbl,
            interp_vortex_quad_10_10,
            interp_vortex_max_10_10,
            interp_unit_vector_tbl};
  }

  constexpr GahmParamPack(double in_radius_to_max_winds, double in_gahm_b,
                          double in_gahm_phi, double in_holland_b,
                          double in_vortex_quad_10_tbl, double in_vortex_max_10_tbl,
                          double in_vortex_quad_10_10, double in_vortex_max_10_10,
                          const Types::Vec& in_unit_vector_tbl)
      : m_radius_to_max_winds(in_radius_to_max_winds),
        m_gahm_b(in_gahm_b),
        m_gahm_phi(in_gahm_phi),
        m_holland_b(in_holland_b),
        m_vortex_quad_10_tbl(in_vortex_quad_10_tbl),
        m_vortex_max_10_tbl(in_vortex_max_10_tbl),
        m_vortex_quad_10_10(in_vortex_quad_10_10),
        m_vortex_max_10_10(in_vortex_max_10_10),
        m_unit_vector_tbl(in_unit_vector_tbl) {}

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
};

auto GahmParameters(const Gahm::Storm::StormTranslation& translation,
                    const Gahm::Types::Point& eye_location,
                    const Gahm::Types::Vec& unit_vector,
                    double central_pressure, double background_pressure,
                    double v_max, double isotach_speed,
                    double isotach_radius) -> GahmParamPack;

}  // namespace Gahm::Solver
#endif  // GAHM_GAHMPARAMETERS_HPP
