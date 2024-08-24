//
// Created by Zach Cobell on 8/12/24.
//

#ifndef GAHM_ATMOSPHERIC_HPP
#define GAHM_ATMOSPHERIC_HPP

#include <cassert>
#include <cmath>

#include "physical/Constants.hpp"

namespace Gahm::Physical::Atmospheric {

constexpr auto holland_b(const double &velocity, const double dp) -> double {
  assert(dp > 0.0);
  return (velocity * velocity * Gahm::Physical::Constants::rhoAir() * M_E) / dp;
}

/**
 * Computes the Rossby number for the storm
 * @param vmax max storm wind velocity
 * @param rmax radius to max winds
 * @param f_coriolis coriolis force
 * @return rossby number
 */
constexpr auto rossbyNumber(double vmax, double rmax,
                            double f_coriolis) -> double {
  assert(f_coriolis > 0.0);
  assert(rmax > 0.0);
  assert(vmax > 0.0);
  return vmax / (f_coriolis * rmax);
}

/**
 * Compute the turning angle at the specified distance for the given r_max value
 * @param distance distance from the center of the storm
 * @param r_max radius to max winds for this quadrant/isotach
 * @return turning angle in radians
 */
constexpr auto turning_angle(double distance, double r_max) -> double {
  constexpr auto deg_10 = 10.0 * Constants::deg2rad();
  constexpr auto deg_75 = 75.0 * Constants::deg2rad();
  constexpr auto deg_25 = 25.0 * Constants::deg2rad();

  if (distance < r_max) {
    return deg_10 * distance / r_max;
  } else if (distance < 1.2 * r_max) {
    return deg_10 + deg_75 * (distance / r_max - 1);
  } else {
    return deg_25;
  }
}

}  // namespace Gahm::Physical::Atmospheric

#endif  // GAHM_ATMOSPHERIC_HPP
