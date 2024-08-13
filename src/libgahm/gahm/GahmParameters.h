//
// Created by Zach Cobell on 7/30/24.
//

#ifndef GAHM_GAHMPARAMETERS_H
#define GAHM_GAHMPARAMETERS_H

#include "datatypes/Point.h"
#include "datatypes/Vec.h"
#include "storm/StormTranslation.h"

namespace Gahm {

class GahmParameters {
 public:
  explicit GahmParameters(const Gahm::Atcf::StormTranslation& translation,
                          const Gahm::Types::Point& eye_location,
                          const Gahm::Types::Vec& unit_vector,
                          double central_pressure, double background_pressure,
                          double v_max, double isotach_speed,
                          double isotach_radius);

  [[nodiscard]] constexpr auto radius_to_max_winds() const -> double {
    return m_rmax;
  }

  [[nodiscard]] constexpr auto gahm_b() const -> double { return m_gahm_b; }

  [[nodiscard]] constexpr auto gahm_phi() const -> double { return m_gahm_phi; }

 private:
  double m_rmax;
  double m_gahm_b;
  double m_gahm_phi;
};

}  // namespace Gahm
#endif  // GAHM_GAHMPARAMETERS_H
