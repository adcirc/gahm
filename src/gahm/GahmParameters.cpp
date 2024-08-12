//
// Created by Zach Cobell on 7/30/24.
//

#include "GahmParameters.h"

#include <array>
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <optional>
#include <tuple>

#include "datatypes/Point.h"
#include "datatypes/RotationMatrix.h"
#include "datatypes/Vec.h"
#include "gahm/GahmSolver.h"
#include "physical/Atmospheric.h"
#include "physical/Constants.h"
#include "physical/Earth.h"
#include "storm/StormTranslation.h"

using Vec = Gahm::Types::Vec;

auto quadratic(double a, double b,
               double c) -> std::optional<std::array<double, 2>> {
  const auto disc = b * b - 4 * a * c;
  if (disc < 0) {
    return std::nullopt;
  }
  const auto disc_sqrt = std::sqrt(disc);
  const auto sln_a = (-b + disc_sqrt) / (2 * a);
  const auto sln_b = (-b - disc_sqrt) / (2 * a);
  return std::make_optional<std::array<double, 2>>({sln_a, sln_b});
}

auto limit_quadrant_vmax(
    double v_max, double quadrant_wind_speed, const Vec& quadrant_unit_vector,
    const Gahm::Atcf::StormTranslation& translation) -> double {
  constexpr double acoef = 1.0;
  const double bcoef =
      2.0 * (quadrant_unit_vector.u() * translation.velocity().u() +
             quadrant_unit_vector.v() * translation.velocity().v());
  const double ccoef = translation.speed() * translation.speed() -
                       quadrant_wind_speed * quadrant_wind_speed;

  const auto sln = quadratic(acoef, bcoef, ccoef);
  if (sln.has_value()) {
    const auto a_sln = sln.value()[0];
    const auto b_sln = sln.value()[1];
    const auto a_diff = std::abs(a_sln - v_max);
    const auto b_diff = std::abs(b_sln - v_max);
    return a_diff < b_diff ? a_sln : b_sln;
  } else {
    std::cerr << "Failed to solve quadratic for isotach wind speed\n";
    return v_max;
  }
}

auto check_quadrant_vmax(double v_max, double quadrant_wind_speed,
                         const Vec& v_max_vector,
                         const Vec& quadrant_unit_vector,
                         const Gahm::Atcf::StormTranslation& translation)
    -> std::tuple<bool, double> {
  const auto max_allowable_vmax =
      std::hypot(v_max_vector.u() + translation.velocity().u(),
                 v_max_vector.v() + translation.velocity().v());
  return quadrant_wind_speed > max_allowable_vmax
             ? std::make_tuple(
                   true, limit_quadrant_vmax(v_max, quadrant_wind_speed,
                                             quadrant_unit_vector, translation))
             : std::make_tuple(false, v_max);
}

Gahm::GahmParameters::GahmParameters(
    const Gahm::Atcf::StormTranslation& translation,
    const Gahm::Types::Point& eye_location, const Gahm::Types::Vec& unit_vector,
    double central_pressure, double background_pressure, double v_max,
    double isotach_speed, double isotach_radius)
    : m_translation(translation),
      m_eye_location(eye_location),
      m_unit_vector(unit_vector),
      m_central_pressure(central_pressure),
      m_background_pressure(background_pressure),
      m_isotach_speed_1_10(isotach_speed),
      m_isotach_radius(isotach_radius),
      m_fc(Gahm::Physical::Earth::coriolis(m_eye_location.y())),
      m_dp(m_background_pressure - m_central_pressure),
      m_v_max_10_10(v_max *
                    Gahm::Physical::Constants::oneMinuteToTenMinuteWind()),
      m_v_max_10_10_nominal(m_v_max_10_10 - m_translation.speed()),
      m_v_max_10_10_theta(std::atan2(m_translation.velocity().v(),
                                     m_translation.velocity().u()) -
                          Gahm::Physical::Constants::halfPi()),
      m_v_isotach_10_10(m_isotach_speed_1_10 *
                        Gahm::Physical::Constants::oneMinuteToTenMinuteWind()) {

  // Set the parameters in each quadrant isotach
  this->compute_gahm_parameters();
}

void Gahm::GahmParameters::compute_gahm_parameters() {
  const Types::RotationMatrix rotation_matrix_ccw25(
      -25.0 * Physical::Constants::deg2rad(), m_eye_location.y());
  const Types::RotationMatrix rotation_matrix_ccw10(
      -10.0 * Physical::Constants::deg2rad(), m_eye_location.y());
  const Types::RotationMatrix rotation_matrix_cw10(
      10.0 * Physical::Constants::deg2rad(), m_eye_location.y());

  // Impose a limit on the maximum storm translation speed
  if (m_translation.speed() > m_v_max_10_10 / 2.0) {
    m_translation =
        Atcf::StormTranslation(m_v_max_10_10 / 2.0, m_translation.direction());
  }

  const auto [is_limited, v_max_out] = check_quadrant_vmax(
      m_v_max_10_10_nominal, m_v_isotach_10_10,
      m_unit_vector * m_v_max_10_10_nominal, m_unit_vector, m_translation);
  m_v_vortex_max_10_10 = v_max_out;

  m_quadrant_unit_vector = [&]() {
    if (is_limited) {
      return Types::Vec::matmul_22_21(rotation_matrix_ccw10.data(),
                                      m_unit_vector);
    } else {
      return Types::Vec::matmul_22_21(rotation_matrix_ccw25.data(),
                                      m_unit_vector);
    }
  }();

  const auto v_vortex_max_10_tbl =
      m_v_vortex_max_10_10 *
      Physical::Constants::tenMeterToTopOfBoundaryLayer();
  const auto vortex_quad_10_10 =
      m_v_isotach_10_10 /
      (m_quadrant_unit_vector + m_translation.velocity() / m_v_vortex_max_10_10)
          .magnitude();
  const auto vortex_quad_10_tbl =
      vortex_quad_10_10 * Physical::Constants::tenMeterToTopOfBoundaryLayer();

  auto solver =
      Gahm::Solver::GahmSolver(m_isotach_radius, vortex_quad_10_tbl,
                               v_vortex_max_10_tbl, m_dp, m_eye_location.y());
  solver.solve();

  m_gahm_b = solver.gahm_b();
  m_rmax = solver.rmax();
  m_gahm_phi = solver.phi();

}
