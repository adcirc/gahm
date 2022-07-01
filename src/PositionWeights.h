//
// Created by Zach Cobell on 5/9/22.
//

#ifndef GAHM2_SRC_POSITIONWEIGHTS_H_
#define GAHM2_SRC_POSITIONWEIGHTS_H_

#include <tuple>
#include <utility>

#include "StormIsotach.h"
#include "UnitConversion.h"

namespace Gahm {
class PositionWeights {
 public:
  PositionWeights(
      int isotach, double isotach_weight, int quadrant, double delta_angle,
      std::pair<StormQuadrant::quadrant_it<4>, StormQuadrant::quadrant_it<4>>
          this_quad_it,
      std::pair<StormQuadrant::quadrant_it<4>, StormQuadrant::quadrant_it<4>>
          next_quad_it)
      : m_isotach(isotach),
        m_isotachWeighting(isotach_weight),
        m_quadrant(quadrant),
        m_delta_angle(delta_angle),
        m_this_isotach_iterators(std::move(this_quad_it)),
        m_next_isotach_iterators(std::move(next_quad_it)),
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

  [[nodiscard]] std::pair<StormQuadrant::quadrant_it<4>,
                          StormQuadrant::quadrant_it<4>>
  this_isotach_iterators() const {
    return m_this_isotach_iterators;
  }

  [[nodiscard]] std::pair<StormQuadrant::quadrant_it<4>,
                          StormQuadrant::quadrant_it<4>>
  next_isotach_iterators() const {
    return m_next_isotach_iterators;
  }

 private:
  static constexpr double s_angle_90 =
      90.0 * Units::convert(Units::Degree, Units::Radian);

  int m_quadrant;
  int m_isotach;
  double m_isotachWeighting;
  double m_delta_angle;
  std::pair<StormQuadrant::quadrant_it<4>, StormQuadrant::quadrant_it<4>>
      m_this_isotach_iterators;
  std::pair<StormQuadrant::quadrant_it<4>, StormQuadrant::quadrant_it<4>>
      m_next_isotach_iterators;
  double m_delta_angle_weight1;
  double m_delta_angle_weight2;
};
}  // namespace Gahm

#endif  // GAHM2_SRC_POSITIONWEIGHTS_H_
