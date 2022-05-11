//
// Created by Zach Cobell on 5/8/22.
//

#ifndef GAHM2_SRC_GAHMEQUATIONS_H_
#define GAHM2_SRC_GAHMEQUATIONS_H_

#include "Atmospheric.h"
#include <cassert>

namespace Gahm {
class GahmEquations {

public:
  static double GahmFunction(double radius_to_max_wind,
                             double vmax_at_boundary_layer,
                             double isotach_windspeed_at_boundary_layer,
                             double distance, double coriolis_force,
                             double gahm_holland_b, double phi);

  static double GahmFunction(double radius_to_max_wind,
                             double vmax_at_boundary_layer,
                             double isotach_windspeed_at_boundary_layer,
                             double distance, double coriolis_force,
                             double gahm_holland_b);

  static double GahmFunctionDerivative(
      double radius_to_max_wind, double vmax_at_boundary_layer,
      double isotach_windspeed_at_boundary_layer, double coriolis_force,
      double gahm_holland_b, double phi);

  static double
  GahmFunctionDerivative(double radius_to_max_wind,
                         double vmax_at_boundary_layer,
                         double isotach_windspeed_at_boundary_layer,
                         double coriolis_force, double gahm_holland_b);

  static double GahmPressure(double central_pressure,
                             double background_pressure, double distance,
                             double radius_to_max_winds, double gahm_holland_b,
                             double phi);

  /**
   * Compute the GAHM phi parameter
   * @param vmax maximum storm wind velocity
   * @param rmax radius to max winds
   * @param b GAHM holland b parameter
   * @param fc coriolis force
   * @return phi
   */
  static constexpr double phi(double vmax, double rmax, double bg, double fc) {
    assert(fc > 0.0);
    assert(vmax > 0.0);
    assert(rmax > 0.0);
    const auto rossby = Atmospheric::rossbyNumber(vmax, rmax, fc);
    return 1.0 + (1.0 / (rossby * bg * (1.0 + 1.0 / rossby)));
  }

  /**
   * Computes the GAHM modified Holland B
   * @param vmax maximum storm wind velocity
   * @param rmax radius to maximum winds
   * @param dp pressure deficit
   * @param fc coriolis force
   * @param bg current value for GAHM Holland B
   * @param phi GAHM Phi parameter
   * @return GAHM Holland B
   */
  static double bg(double vmax, double rmax, double p0, double pinf, double fc,
                   double phi) {
    const auto b = Atmospheric::calcHollandB(vmax, p0, pinf);
    const auto ro = Atmospheric::rossbyNumber(vmax, rmax, fc);
    auto bg = (b * ((1 + 1 / ro) * std::exp(phi - 1)) / phi);
    return bg;
  }
};
} // namespace Gahm
#endif // GAHM2_SRC_GAHMEQUATIONS_H_
