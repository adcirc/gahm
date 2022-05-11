//
// Created by Zach Cobell on 5/9/22.
//

#ifndef GAHM2_SRC_POSITIONWEIGHTS_H_
#define GAHM2_SRC_POSITIONWEIGHTS_H_

#include "UnitConversion.h"
#include <tuple>

namespace Gahm {
class PositionWeights {
public:
  PositionWeights(int isotach, double isotach_weight, int quadrant,
                  double delta_angle)
      : m_isotach(isotach), m_isotachWeighting(isotach_weight),
        m_quadrant(quadrant), m_delta_angle(delta_angle),
        m_delta_angle_weight1(1.0 / std::pow(m_delta_angle, 2.0)),
        m_delta_angle_weight2(1.0 / std::pow(s_angle_90 - m_delta_angle, 2.0)) {
  }

  [[nodiscard]] int isotach() const { return m_isotach; }

  [[nodiscard]] int quadrant() const { return m_quadrant; }

  [[nodiscard]] double isotach_weight() const { return m_isotachWeighting; }

  [[nodiscard]] double delta_angle() const { return m_delta_angle; }

  [[nodiscard]] std::tuple<double, double> delta_angle_weights() const {
    return {m_delta_angle_weight1, m_delta_angle_weight2};
  }

private:
  static constexpr double s_angle_90 =
      90.0 * Units::convert(Units::Degree, Units::Radian);

  int m_quadrant;
  int m_isotach;
  double m_isotachWeighting;
  double m_delta_angle;
  double m_delta_angle_weight1;
  double m_delta_angle_weight2;
};
} // namespace Gahm

#endif // GAHM2_SRC_POSITIONWEIGHTS_H_
