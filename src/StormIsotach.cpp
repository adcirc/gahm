//
// Created by Zach Cobell on 5/8/22.
//

#include "StormIsotach.h"

#include <algorithm>

using namespace Gahm;
void StormIsotach::fillMissingData(double radius_to_max_winds) {
  //...Count the quadrants with no data
  const int n_zero = [&]() {
    int count = 0;
    for (const auto &q : m_quadrant) {
      if (q.isotach_radius() == 0.0) count++;
    }
    return count;
  }();

  //...Select filling technique based on number of zeros
  switch (n_zero) {
    case 0:
      return;
    case 1:
      return this->setMissingToAverageOfAdjacentRadii();
    case 2:
      return this->setMissingToHalfSpecifiedRadii();
    case 3:
      return this->setMissingRadiiToSpecifiedRadii();
    case 4:
      return this->setAllRadiiToRmax(radius_to_max_winds);
    default:
      return;
  }
}

void StormIsotach::setAllRadiiToRmax(double radius_to_max_winds) {
  for (auto &q : m_quadrant) {
    q.set_isotach_radius(radius_to_max_winds);
  }
}

void StormIsotach::setMissingRadiiToSpecifiedRadii() {
  double specified_radii = 0.0;

  for (auto &q : m_quadrant) {
    if (q.isotach_radius() > 0.0) {
      specified_radii = q.isotach_radius();
      break;
    }
  }

  for (auto &q : m_quadrant) {
    if (q.isotach_radius() == 0.0) q.set_isotach_radius(specified_radii);
  }
}

void StormIsotach::setMissingToAverageOfAdjacentRadii() {
  for (int i = 0; i < 4; ++i) {
    if (m_quadrant[i].isotach_radius() == 0.0) {
      m_quadrant[i].set_isotach_radius((m_quadrant[i - 1].isotach_radius() +
                                        m_quadrant[i + 1].isotach_radius()) /
                                       2.0);
    }
  }
}

void StormIsotach::setMissingToHalfSpecifiedRadii() {
  double sum = 0.0;
  for (const auto &q : m_quadrant) {
    if (q.isotach_radius() != 0.0) sum += q.isotach_radius();
  }
  const double radius = sum / 2.0;
  for (auto &q : m_quadrant) {
    if (q.isotach_radius() == 0.0) q.set_isotach_radius(radius);
  }
}

bool StormIsotach::operator<(const StormIsotach &a) const {
  return m_wind_speed < a.windSpeed();
}

bool StormIsotach::operator>(const StormIsotach &a) const {
  return m_wind_speed > a.windSpeed();
}
