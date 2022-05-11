//
// Created by Zach Cobell on 5/8/22.
//

#ifndef GAHM2__ISOTACH_H_
#define GAHM2__ISOTACH_H_

#include <cassert>
#include <cstdlib>

#include "CircularArray.h"
#include "StormQuadrant.h"
namespace Gahm {
class StormIsotach {
 public:
  StormIsotach(double wind_speed, const std::array<double, 4> &radii)
      : m_wind_speed(wind_speed),
        m_quadrant({StormQuadrant(0, radii[0], 0, 0, 0),
                    StormQuadrant(1, radii[1], 0, 0, 0),
                    StormQuadrant(2, radii[2], 0, 0, 0),
                    StormQuadrant(3, radii[3], 0, 0, 0)}) {}

  [[nodiscard]] double windSpeed() const { return m_wind_speed; }

  [[nodiscard]] const CircularArray<Gahm::StormQuadrant, 4> &quadrants() const {
    return m_quadrant;
  }
  [[nodiscard]] CircularArray<Gahm::StormQuadrant, 4> &quadrants() {
    return m_quadrant;
  }

  [[nodiscard]] Gahm::StormQuadrant &quadrant(int index) {
    return m_quadrant[index];
  }

  [[nodiscard]] const Gahm::StormQuadrant &quadrant(int index) const {
    return m_quadrant[index];
  }

  void fillMissingData(double radius_to_max_winds);

  bool operator<(const StormIsotach &a) const;

  bool operator>(const StormIsotach &a) const;

 private:
  void setMissingToAverageOfAdjacentRadii();
  void setMissingRadiiToSpecifiedRadii();
  void setAllRadiiToRmax(double radius_to_max_winds);
  void setMissingToHalfSpecifiedRadii();

  double m_wind_speed;
  CircularArray<StormQuadrant, 4> m_quadrant;
};
}  // namespace Gahm
#endif  // GAHM2__ISOTACH_H_
