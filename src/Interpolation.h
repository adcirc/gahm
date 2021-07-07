//
// Created by Zach Cobell on 6/17/21.
//

#ifndef GAHM_SRC_INTERPOLATION_H_
#define GAHM_SRC_INTERPOLATION_H_

namespace Interpolation {
/**
 * Computes a linear interpolation between two values with a specified
 * weighting factor
 * @param[in] weight weighting factor
 * @param[in] v1 value 1 for weighting
 * @param[in] v2 value 2 for weighting
 * @return interpolated value
 */
static double linearInterp(const double weight, const double v1,
                           const double v2) {
  return (1.0 - weight) * v1 + weight * v2;
}

static double angleInterp(const double weight, const double v1,
                          const double v2) {
  constexpr double min_angle =
      10.0 * Units::convert(Units::Degree, Units::Radian);

  if (v2 - v1 < min_angle) {
    return linearInterp(weight, v1, v2);
  } else {
    double v1x = std::cos(v1);
    double v1y = std::sin(v1);
    double v2x = std::cos(v2);
    double v2y = std::sin(v2);
    double v3x = v1x * (1.0 - weight) + v2x * weight;
    double v3y = v1y * (1.0 - weight) + v2y * weight;
    return std::atan2(v3y, v3x);
  }
}

static double powerInterp(double power1, double power2, double value1, double value2){
    return ((value1 * power1) + (value2 * power2)) / ( power1 + power2 );
}

}  // namespace Interpolation

#endif  // GAHM_SRC_INTERPOLATION_H_
