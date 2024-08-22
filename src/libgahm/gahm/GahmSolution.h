//
// Created by Zach Cobell on 8/16/24.
//

#ifndef GAHM_GAHMSOLUTION_H
#define GAHM_GAHMSOLUTION_H

#include "datatypes/Vec.h"
#include "storm/Quadrant.h"
#include "storm/StormTranslation.h"

namespace Gahm::Solver::Solution {

struct GahmSolutionPoint {
  Types::Vec wind_vector;
  double pressure;

  constexpr GahmSolutionPoint(const Types::Vec &in_wind_speed_vector,
                              double in_pressure)
      : wind_vector(in_wind_speed_vector), pressure(in_pressure) {}
};

struct GahmInputParamsBase {
  const Types::Point &eye_location;
  const Storm::StormTranslation &translation;
  const double central_pressure;
  const double background_pressure;
  const double coriolis;

  constexpr GahmInputParamsBase(const Types::Point &in_eye_location,
                                const Storm::StormTranslation &in_translation,
                                const double in_central_pressure,
                                const double in_background_pressure,
                                const double in_coriolis)
      : eye_location(in_eye_location),
        translation(in_translation),
        central_pressure(in_central_pressure),
        background_pressure(in_background_pressure),
        coriolis(in_coriolis) {}
};

struct GahmInputParamsQuadrant : public GahmInputParamsBase {
  const Storm::Quadrant &quadrant;
  const double distance;

  constexpr GahmInputParamsQuadrant(
      const Storm::Quadrant &in_quadrant, const double in_distance,
      const Types::Point &in_eye_location,
      const Storm::StormTranslation &in_translation,
      const double in_central_pressure, const double in_background_pressure,
      const double in_coriolis)
      : GahmInputParamsBase(in_eye_location, in_translation,
                            in_central_pressure, in_background_pressure,
                            in_coriolis),
        quadrant(in_quadrant),
        distance(in_distance) {}
};

struct GahmInputParamsPoint : public GahmInputParamsBase {
  const std::array<Storm::Quadrant, 4> &quadrants;
  const Types::Point &point;

  constexpr GahmInputParamsPoint(
      const std::array<Storm::Quadrant, 4> &in_quadrants,
      const Types::Point &in_point, const Types::Point &in_eye_location,
      const Storm::StormTranslation &in_translation,
      const double in_central_pressure, const double in_background_pressure,
      const double in_coriolis)
      : GahmInputParamsBase(in_eye_location, in_translation,
                            in_central_pressure, in_background_pressure,
                            in_coriolis),
        quadrants(in_quadrants),
        point(in_point) {}
};

[[nodiscard]] auto get(const GahmInputParamsQuadrant &input)
    -> GahmSolutionPoint;

[[nodiscard]] auto get(const GahmInputParamsPoint &input) -> GahmSolutionPoint;

}  // namespace Gahm::Solver::Solution
#endif  // GAHM_GAHMSOLUTION_H
