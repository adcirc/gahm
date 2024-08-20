//
// Created by Zach Cobell on 7/30/24.
//

#include "GahmParameters.h"

#include <algorithm>
#include <array>
#include <cmath>
#include <iostream>
#include <optional>
#include <tuple>

#include "datatypes/Point.h"
#include "datatypes/RotationMatrix.h"
#include "datatypes/Vec.h"
#include "gahm/GahmSolver.h"
#include "physical/Atmospheric.h"
#include "physical/Constants.h"
#include "storm/StormTranslation.h"

namespace {
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
constexpr auto quadratic(double a_coef, double b_coef,
                         double c_coef) -> double {
  const auto disc = b_coef * b_coef - 4 * a_coef * c_coef;
  //  if (disc < 0) {
  //    return std::nullopt;
  //  }
  const auto disc_sqrt = std::sqrt(disc);
  const auto sln_a = (-b_coef + disc_sqrt) / (2 * a_coef);
  //  const auto sln_b = (-b_coef - disc_sqrt) / (2 * a_coef);
  return sln_a;
}

/**
 * @brief Updates the quadrant v_max so that it is always greater than the
 * isotach speed
 *
 * @param v_max Maximum wind speed
 * @param quadrant_wind_speed Quadrant wind speed
 * @param quadrant_unit_vector Quadrant unit vector
 * @param translation Storm translation object
 * @return Limited wind speed
 */
auto compute_new_quadrant_vmax(
    double quadrant_wind_speed, const Gahm::Types::Vec& quadrant_unit_vector,
    const Gahm::Storm::StormTranslation& translation) -> double {
  constexpr double acoef = 1.0;
  const double bcoef =
      2.0 * translation.speed() *
      (quadrant_unit_vector.u() * translation.unit_vector().u() +
       quadrant_unit_vector.v() * translation.unit_vector().v());
  const double ccoef = (translation.speed() * translation.speed()) -
                       (quadrant_wind_speed * quadrant_wind_speed);
  return quadratic(acoef, bcoef, ccoef);
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
                         const Gahm::Storm::StormTranslation& translation)
    -> std::tuple<bool, double> {
  const auto max_allowable_vmax =
      std::hypot(v_max_vector.u() + translation.velocity().u(),
                 v_max_vector.v() + translation.velocity().v());
  return quadrant_wind_speed > max_allowable_vmax
             ? std::make_tuple(true, compute_new_quadrant_vmax(
                                         quadrant_wind_speed,
                                         quadrant_unit_vector, translation))
             : std::make_tuple(false, v_max);
}

/**
 * @brief Generate the translation object
 *
 * @param translation Current translation object
 * @param v_max_10_10 Maximum wind speed
 * @return New translation object
 */
auto generate_translation_obj(const Gahm::Storm::StormTranslation& translation,
                              const double v_max_10_10)
    -> Gahm::Storm::StormTranslation {
  return translation.speed() > v_max_10_10 / 2.0
             ? Gahm::Storm::StormTranslation(v_max_10_10 / 2.0,
                                             translation.direction())
             : translation;
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
    const Gahm::Storm::StormTranslation& translation,
    const Gahm::Types::Point& eye_location, const Gahm::Types::Vec& unit_vector,
    double central_pressure, double background_pressure, double v_max,
    double isotach_speed,
    double isotach_radius) -> Gahm::Solver::GahmParamPack {
  const auto v_max_10_10 =
      v_max * Gahm::Physical::Constants::oneMinuteToTenMinuteWind();

  const auto v_max_10_10_nominal = v_max_10_10 - translation.speed();

  const auto v_isotach_10_10 =
      isotach_speed * Gahm::Physical::Constants::oneMinuteToTenMinuteWind();

  const auto this_translation =
      generate_translation_obj(translation, v_max_10_10);

  const Gahm::Types::RotationMatrix rotation_matrix_ccw10(
      -10.0 * Gahm::Physical::Constants::deg2rad(), eye_location.y());
  auto quadrant_vector_10 =
      Gahm::Types::Vec::matmul_22_21(rotation_matrix_ccw10.data(), unit_vector);

  const auto [is_limited, v_max_out] = check_quadrant_vmax(
      v_max_10_10_nominal, v_isotach_10_10, unit_vector * v_max_10_10_nominal,
      quadrant_vector_10, this_translation);
  const double v_vortex_max_10_10 = v_max_out;

  if (!is_limited) {
    const Gahm::Types::RotationMatrix rotation_matrix_ccw25(
        -25.0 * Gahm::Physical::Constants::deg2rad(), eye_location.y());
    quadrant_vector_10 = Gahm::Types::Vec::matmul_22_21(
        rotation_matrix_ccw25.data(), unit_vector);
  };

  const auto v_vortex_max_10_tbl =
      v_vortex_max_10_10 *
      Gahm::Physical::Constants::tenMeterToTopOfBoundaryLayer();
  const auto vortex_quad_10_10 =
      v_isotach_10_10 /
      (quadrant_vector_10 + translation.velocity() / v_vortex_max_10_10)
          .magnitude();

  const auto vortex_quad_10_tbl =
      vortex_quad_10_10 *
      Gahm::Physical::Constants::tenMeterToTopOfBoundaryLayer();

  // Solve the GAHM equations
  auto solver = Gahm::Solver::GahmSolver(
      isotach_radius, vortex_quad_10_tbl, v_vortex_max_10_tbl,
      (background_pressure - central_pressure), eye_location.y());
  solver.solve();

  const auto holland_b = Gahm::Physical::Atmospheric::holland_b(
      v_vortex_max_10_tbl, (background_pressure - central_pressure));

  return {solver.rmax(),     solver.gahm_b(),    solver.phi(),
          holland_b,         vortex_quad_10_tbl, v_vortex_max_10_tbl,
          vortex_quad_10_10, v_vortex_max_10_10, unit_vector,
          is_limited};
}

}  // namespace

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
auto Gahm::Solver::GahmParameters(
    const Gahm::Storm::StormTranslation& translation,
    const Gahm::Types::Point& eye_location, const Gahm::Types::Vec& unit_vector,
    double central_pressure, double background_pressure, double v_max,
    double isotach_speed,
    double isotach_radius) -> Gahm::Solver::GahmParamPack {
  return compute_gahm_parameters(translation, eye_location, unit_vector,
                                 central_pressure, background_pressure, v_max,
                                 isotach_speed, isotach_radius);
}