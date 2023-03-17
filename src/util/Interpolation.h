//
// Created by Zach Cobell on 3/14/23.
//

#ifndef GAHM_INTERPOLATION_H
#define GAHM_INTERPOLATION_H

#include <cmath>

namespace Gahm::Interpolation {

constexpr double linear(double v0, double v1, double weight) noexcept {
  //return v0 + weight * (v1 - v0);
  return (v0 * (1.0 - weight)) + (v1 * weight);
}

constexpr double angle(double v0, double v1, double weight,
                       bool wrap = false) noexcept {
  if (wrap && v1 < v0) v1 += Gahm::Physical::Constants::twoPi();
  auto angle = linear(v0, v1, weight);
  if (angle < 0.0) {
    angle += Gahm::Physical::Constants::twoPi();
  } else if (angle > Gahm::Physical::Constants::twoPi()) {
    angle -= Gahm::Physical::Constants::twoPi();
  }
  return angle;
}

// constexpr double radial(double v0, double v1, double w0, double w1) noexcept
// {
//   return ((v0 * w0) + (v1 * w1)) / (w0 + w1);
// }

constexpr double angle_idw(double v0, double v1, double delta_angle) noexcept {
  constexpr double angle_1 = Gahm::Physical::Constants::deg2rad();
  constexpr double angle_89 = 89.0 * angle_1;
  constexpr double angle_90 = 90.0 * angle_1;
  if (delta_angle < angle_1) {
    return v0;
  } else if (delta_angle > angle_89) {
    return v1;
  } else {
    double num = v0 / std::pow(delta_angle, 2.0) +
                 v1 / std::pow(angle_90 - delta_angle, 2.0);
    double den = 1.0 / std::pow(delta_angle, 2.0) +
                 1.0 / std::pow(angle_90 - delta_angle, 2.0);
    return num / den;
  }
}

}  // namespace Gahm::Interpolation

#endif  // GAHM_INTERPOLATION_H
