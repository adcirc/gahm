//
// Created by Zach Cobell on 7/30/24.
//

#include "Isotach.h"

#include <cassert>
#include <ostream>

#include "StormTranslation.h"
#include "datatypes/Point.h"
#include "datatypes/Vec.h"
#include "gahm/GahmParameters.h"
#include "physical/Units.h"

namespace Gahm::Storm {

void Isotach::compute_gahm_parameters(
    const Gahm::Storm::StormTranslation &translation,
    const Types::Point &eye_location, const Types::Vec &unit_vector,
    double central_pressure, double background_pressure, double v_max) {
  m_gahm_params = Gahm::Solver::GahmParameters(
      translation, eye_location, unit_vector, central_pressure,
      background_pressure, v_max, m_wind_speed, m_distance_to_isotach);
}

}  // namespace Gahm::Storm

auto operator<<(std::ostream &stream,
                const Gahm::Storm::Isotach &isotach) -> std::ostream & {
  constexpr auto ms2kt = Gahm::Physical::Units::convert(
      Gahm::Physical::Units::MetersPerSecond, Gahm::Physical::Units::Knot);
  constexpr auto m2nmi = Gahm::Physical::Units::convert(
      Gahm::Physical::Units::Meter, Gahm::Physical::Units::NauticalMile);
  stream << "Wind Speed=" << isotach.wind_speed() << " ("
         << isotach.wind_speed() * ms2kt << " knots)"
         << ", Radius=" << isotach.radius() << " (" << isotach.radius() * m2nmi
         << " nmi)"
         << ", Radius to Max Winds="
         << isotach.gahm_parameters().radius_to_max_winds()
         << ", Gahm B=" << isotach.gahm_parameters().gahm_b()
         << ", Gahm Phi=" << isotach.gahm_parameters().gahm_phi()
         << ", Holland B=" << isotach.gahm_parameters().holland_b();

  return stream;
}