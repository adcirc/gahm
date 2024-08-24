#ifndef GAHM_PHYSICALCONSTANTS_H
#define GAHM_PHYSICALCONSTANTS_H

#include <cmath>

namespace Gahm::Physical::Constants {

/*
 * Pi constant
 * @return Pi constant
 */
static constexpr auto pi() -> double { return M_PI; }

/*
 * 2*Pi constant
 * @return 2*Pi constant
 */
static constexpr auto twoPi() -> double { return 2.0 * pi(); }

/*
 * Pi/2 constant
 * @return Pi/2 constant
 */
static constexpr auto halfPi() -> double { return M_PI_2; }

/*
 * Pi/4 constant
 * @return Pi/4 constant
 */
static constexpr auto quarterPi() -> double { return M_PI_4; }

/*
 * Sqrt(2) constant
 */
static constexpr auto sqrt2() -> double { return M_SQRT2; }

/*
 * Pi/180 constant
 * @return Pi/180 constant
 */
static constexpr auto deg2rad() -> double { return M_PI / 180.0; }

/*
 * 180/Pi constant
 * @return 180/Pi constant
 */
static constexpr auto rad2deg() -> double { return 180.0 / M_PI; }

/*
 * Background pressure in millibars
 * @return Background pressure in millibars
 */
static constexpr auto backgroundPressure() -> double { return 1013.00; }

/*
 * Wind speed reduction factor for boundary layer winds to 10m
 * @return Wind speed reduction factor for 10m winds
 */
static constexpr auto topOfBoundaryLayerToTenMeter() -> double { return 0.9; }

/*
 * Wind speed increase factor for 10m winds to top of boundary layer
 * @return Wind speed increase factor for 10m winds to top of boundary layer
 */
static constexpr auto tenMeterToTopOfBoundaryLayer() -> double {
  return 1.0 / topOfBoundaryLayerToTenMeter();
}

/*
 * Rho of air in kg/m^3
 * @return Rho of air in kg/m^3
 */
static constexpr auto rhoAir() -> double {  // return 1.293;
  return 1.204;
}

/*
 * Gravitational acceleration in m/s^2
 * @return Gravitational acceleration in m/s^2
 */
static constexpr auto g() -> double { return 9.80665; }

/*
 * Rho of water in kg/m^3
 * @return Rho of water in kg/m^3
 */
static constexpr auto rhoWater() -> double { return 1000.0; }

/*
 * One to ten conversion factor
 * @return One to ten conversion factor
 */
static constexpr auto oneMinuteToTenMinuteWind() -> double {  // return 0.8928;
  return 0.89;
}

/*
 * Rotation rate of the Earth in rad/s
 * @return Rotation rate of the Earth in rad/s
 */
static constexpr auto omega() -> double { return 0.00007272; }

}  // namespace Gahm::Physical::Constants
#endif  // GAHM_PHYSICALCONSTANTS_H
