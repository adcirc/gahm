//
// Created by Zach Cobell on 7/30/24.
//

#ifndef GAHM_GAHMPARAMETERS_H
#define GAHM_GAHMPARAMETERS_H

#include <tuple>

#include "datatypes/Point.h"
#include "datatypes/Vec.h"
#include "storm/StormTranslation.h"

namespace Gahm::Solver {

/**
 * @brief A struct to hold the parameters for the Gahm model.
 */
struct GahmParamPack {
  double radius_to_max_winds;
  double gahm_b;
  double gahm_phi;
  double holland_b;

  GahmParamPack(double in_radius_to_max_winds, double in_gahm_b,
                double in_gahm_phi, double in_holland_b)
      : radius_to_max_winds(in_radius_to_max_winds),
        gahm_b(in_gahm_b),
        gahm_phi(in_gahm_phi),
        holland_b(in_holland_b) {}
};

auto GahmParameters(const Gahm::Storm::StormTranslation& translation,
                    const Gahm::Types::Point& eye_location,
                    const Gahm::Types::Vec& unit_vector,
                    double central_pressure, double background_pressure,
                    double v_max, double isotach_speed,
                    double isotach_radius) -> GahmParamPack;

}  // namespace Gahm::Solver
#endif  // GAHM_GAHMPARAMETERS_H
