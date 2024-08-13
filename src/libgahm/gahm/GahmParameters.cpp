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
#include "physical/Constants.h"
#include "storm/StormTranslation.h"

// Forward declarations
auto compute_gahm_parameters(
    const Gahm::Atcf::StormTranslation& translation,
    const Gahm::Types::Point& eye_location, const Gahm::Types::Vec& unit_vector,
    double central_pressure, double background_pressure, double v_max,
    double isotach_speed,
    double isotach_radius) -> std::tuple<double, double, double>;

/**
 * @brief Construct a new Gahm Parameters object and solve the GAHM equations
 *
 * @param translation Storm translation object
 * @param eye_location Eye location
 * @param unit_vector Unit vector for the quadrant
 * @param central_pressure Storm central pressure
 * @param background_pressure Storm background pressure
 * @param v_max Maximum wind speed
 * @param isotach_speed Isotach speed
 * @param isotach_radius Isotach radius
 */
Gahm::GahmParameters::GahmParameters(
    const Gahm::Atcf::StormTranslation& translation,
    const Gahm::Types::Point& eye_location, const Gahm::Types::Vec& unit_vector,
    double central_pressure, double background_pressure, double v_max,
    double isotach_speed, double isotach_radius)
    : m_rmax(0.0), m_gahm_b(0.0), m_gahm_phi(0.0) {
  const auto parameters = compute_gahm_parameters(
      translation, eye_location, unit_vector, central_pressure,
      background_pressure, v_max, isotach_speed, isotach_radius);
  m_rmax = std::get<0>(parameters);
  m_gahm_b = std::get<1>(parameters);
  m_gahm_phi = std::get<2>(parameters);
}

/**
 * @brief Simple quadratic solver
 *
 * Return type is an optional where if the discriminant is negative, the
 * optional is empty. Otherwise, the optional contains an array of two
 * solutions.
 *
 * @param a_coef A coefficient
 * @param b_coef B coefficient
 * @param c_coef C coefficient
 * @return Optional array of solutions
 */
auto quadratic(double a_coef, double b_coef,
               double c_coef) -> std::optional<std::array<double, 2>> {
  const auto disc = b_coef * b_coef - 4 * a_coef * c_coef;
  if (disc < 0) {
    return std::nullopt;
  }
  const auto disc_sqrt = std::sqrt(disc);
  const auto sln_a = (-b_coef + disc_sqrt) / (2 * a_coef);
  const auto sln_b = (-b_coef - disc_sqrt) / (2 * a_coef);
  return std::make_optional(std::array<double, 2>{sln_a, sln_b});
}

/**
 * @brief Limit the quadrant wind speed
 *
 * Limits the quadrant wind speed when it exceeds the maximum allowable wind
 * speed based on the background speed (i.e. the storm translation)
 *
 * @param v_max Maximum wind speed
 * @param quadrant_wind_speed Quadrant wind speed
 * @param quadrant_unit_vector Quadrant unit vector
 * @param translation Storm translation object
 * @return Limited wind speed
 */
auto limit_quadrant_vmax(double v_max, double quadrant_wind_speed,
                         const Gahm::Types::Vec& quadrant_unit_vector,
                         const Gahm::Atcf::StormTranslation& translation)
    -> double {
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

/**
 * @brief Check the quadrant wind speed
 *
 * Checks the quadrant wind speed to see if it exceeds the maximum allowable
 * wind speed based on the background speed (i.e. the storm translation)
 *
 * @param v_max Maximum wind speed
 * @param quadrant_wind_speed Quadrant wind speed
 * @param v_max_vector Vector of the maximum wind speed
 * @param quadrant_unit_vector Quadrant unit vector
 * @param translation Storm translation object
 * @return Tuple of a boolean indicating if the wind speed was limited and the
 * wind speed
 */
auto check_quadrant_vmax(double v_max, double quadrant_wind_speed,
                         const Gahm::Types::Vec& v_max_vector,
                         const Gahm::Types::Vec& quadrant_unit_vector,
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

/**
 * @brief Generate the quadrant unit vector
 *
 * @param eye_location Eye location
 * @param unit_vector Unit vector
 * @param is_limited Boolean indicating if the wind speed was limited
 * @return Quadrant unit vector
 */
auto generate_quadrant_unit_vector(const Gahm::Types::Point& eye_location,
                                   const Gahm::Types::Vec& unit_vector,
                                   bool is_limited) -> Gahm::Types::Vec {
  if (is_limited) {
    const Gahm::Types::RotationMatrix rotation_matrix_ccw10(
        -10.0 * Gahm::Physical::Constants::deg2rad(), eye_location.y());
    return Gahm::Types::Vec::matmul_22_21(rotation_matrix_ccw10.data(),
                                          unit_vector);
  } else {
    const Gahm::Types::RotationMatrix rotation_matrix_ccw25(
        -25.0 * Gahm::Physical::Constants::deg2rad(), eye_location.y());
    return Gahm::Types::Vec::matmul_22_21(rotation_matrix_ccw25.data(),
                                          unit_vector);
  }
}

/**
 * @brief Generate the translation object
 *
 * @param translation Current translation object
 * @param v_max_10_10 Maximum wind speed
 * @return New translation object
 */
auto generate_translation_obj(const Gahm::Atcf::StormTranslation& translation,
                              const double v_max_10_10)
    -> Gahm::Atcf::StormTranslation {
  if (translation.speed() > v_max_10_10 / 2.0) {
    return {v_max_10_10 / 2.0, translation.direction()};
  } else {
    return translation;
  }
}

/**
 * @brief Compute the GAHM parameters using the Newton-Raphson solver
 *
 * @param translation Storm translation object
 * @param eye_location Eye location
 * @param unit_vector Unit vector for the quadrant
 * @param central_pressure Storm central pressure
 * @param background_pressure Storm background pressure
 * @param v_max Maximum wind speed
 * @param isotach_speed Isotach speed
 * @param isotach_radius Isotach radius
 */
auto compute_gahm_parameters(
    const Gahm::Atcf::StormTranslation& translation,
    const Gahm::Types::Point& eye_location, const Gahm::Types::Vec& unit_vector,
    double central_pressure, double background_pressure, double v_max,
    double isotach_speed,
    double isotach_radius) -> std::tuple<double, double, double> {
  const auto v_max_10_10 =
      v_max * Gahm::Physical::Constants::oneMinuteToTenMinuteWind();

  const auto v_max_10_10_nominal = v_max_10_10 - translation.speed();

  //  const auto v_max_10_10_theta =
  //      std::atan2(translation.velocity().v(), translation.velocity().u()) -
  //      Gahm::Physical::Constants::halfPi();

  const auto v_isotach_10_10 =
      isotach_speed * Gahm::Physical::Constants::oneMinuteToTenMinuteWind();

  const auto this_translation =
      generate_translation_obj(translation, v_max_10_10);

  const auto [is_limited, v_max_out] = check_quadrant_vmax(
      v_max_10_10_nominal, v_isotach_10_10, unit_vector * v_max_10_10_nominal,
      unit_vector, this_translation);

  const double v_vortex_max_10_10 = v_max_out;

  const auto quadrant_unit_vector =
      generate_quadrant_unit_vector(eye_location, unit_vector, is_limited);

  const auto v_vortex_max_10_tbl =
      v_vortex_max_10_10 *
      Gahm::Physical::Constants::tenMeterToTopOfBoundaryLayer();
  const auto vortex_quad_10_10 =
      v_isotach_10_10 /
      (quadrant_unit_vector + translation.velocity() / v_vortex_max_10_10)
          .magnitude();
  const auto vortex_quad_10_tbl =
      vortex_quad_10_10 *
      Gahm::Physical::Constants::tenMeterToTopOfBoundaryLayer();

  // Solve the GAHM equations
  auto solver = Gahm::Solver::GahmSolver(
      isotach_radius, vortex_quad_10_tbl, v_vortex_max_10_tbl,
      (background_pressure - central_pressure), eye_location.y());
  solver.solve();

  return std::make_tuple(solver.rmax(), solver.gahm_b(), solver.phi());
}