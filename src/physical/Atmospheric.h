//
// Created by Zach Cobell on 8/12/24.
//

#ifndef GAHM_ATMOSPHERIC_H
#define GAHM_ATMOSPHERIC_H

#include <cassert>
#include <cmath>

#include "physical/Constants.h"

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
constexpr auto rossbyNumber(double vmax, double rmax, double f_coriolis)
    -> double {
  assert(f_coriolis > 0.0);
  assert(rmax > 0.0);
  assert(vmax > 0.0);
  return vmax / (f_coriolis * rmax);
}

}  // namespace Gahm::Physical::Atmospheric

#endif  // GAHM_ATMOSPHERIC_H
