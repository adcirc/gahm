//
// Created by Zach Cobell on 7/30/24.
//

#include "AtcfPeriod.h"

#include <algorithm>
#include <array>
#include <iostream>
#include <optional>

#include "datatypes/Datetime.h"
#include "storm/Quadrant.h"
#include "storm/StormTranslation.h"

namespace Gahm::Atcf {

/**
 * Compute the GAHM parameters for each quadrant
 */
void AtcfPeriod::compute_gahm_parameters() {
  std::ranges::for_each(m_quadrants, [this](Storm::Quadrant &quad) {
    quad.compute_gahm_parameters(m_translation, m_eye_location,
                                 m_central_pressure, m_background_pressure,
                                 m_v_max);
  });
}

/**
 * This function checks that each quadrant has the same number of populated
 * isotachs. This allows us to interpolate between two periods cleanly.
 * @param period_1 The first period to compare
 * @param period_2 The second period to compare
 * @return True if the periods are interpolatable, false otherwise
 */
auto AtcfPeriod::is_interpolatable(const Gahm::Atcf::AtcfPeriod &period_1,
                                   const Gahm::Atcf::AtcfPeriod &period_2)
    -> bool {
  return std::ranges::all_of(
      period_1.quadrants(), [&period_2](const Storm::Quadrant &quad) {
        const auto quad_2 = period_2.quadrant(quad.quadrant_code());
        return Storm::Quadrant::is_interpolatable(quad, quad_2);
      });
}

/**
 * Interpolates between two periods to create a new period at the given datetime
 * @param period_1 First period
 * @param period_2 Second period
 * @param datetime Target datetime
 * @return The interpolated period
 */
auto AtcfPeriod::interpolate(
    const AtcfPeriod &period_1, const AtcfPeriod &period_2,
    const Types::Datetime &datetime) -> std::optional<AtcfPeriod> {
  if (!AtcfPeriod::is_interpolatable(period_1, period_2)) {
    return std::nullopt;
  }

  const auto time_delta =
      static_cast<double>(datetime.seconds_since_epoch() -
                          period_1.datetime().seconds_since_epoch());
  const double weight =
      time_delta /
      static_cast<double>(period_2.datetime().seconds_since_epoch() -
                          period_1.datetime().seconds_since_epoch());

  const auto eye_location_x = Util::Interpolation::linear(
      period_1.eye_location().x(), period_2.eye_location().x(), weight);
  const auto eye_location_y = Util::Interpolation::linear(
      period_1.eye_location().y(), period_2.eye_location().y(), weight);
  const auto this_eye_location = Types::Point(eye_location_x, eye_location_y);

  const auto this_central_pressure = Util::Interpolation::linear(
      period_1.central_pressure(), period_2.central_pressure(), weight);
  const auto this_background_pressure = Util::Interpolation::linear(
      period_1.background_pressure(), period_2.background_pressure(), weight);
  const auto this_v_max =
      Util::Interpolation::linear(period_1.v_max(), period_2.v_max(), weight);
  const auto this_r_max =
      Util::Interpolation::linear(period_1.r_max(), period_2.r_max(), weight);

  const auto this_translation = Storm::StormTranslation(
      this_eye_location, period_1.eye_location(), time_delta);

  std::array<Storm::Quadrant, 4> this_quadrants;
  std::ranges::transform(
      period_1.quadrants(), period_2.quadrants(), this_quadrants.begin(),
      [&](const Storm::Quadrant &quad_1, const Storm::Quadrant &quad_2) {
        return Storm::Quadrant::interpolate(quad_1, quad_2, weight,
                                            eye_location_y);
      });

  return AtcfPeriod(datetime, this_central_pressure, this_background_pressure,
                    this_v_max, this_r_max, this_eye_location, this_translation,
                    this_quadrants);
}

}  // namespace Gahm::Atcf

auto operator<<(std::ostream &stream,
                const Gahm::Atcf::AtcfPeriod &period) -> std::ostream & {
  stream << "Date: " << period.datetime()
         << ", Lat: " << period.eye_location().x()
         << ", Lon: " << period.eye_location().y()
         << ", Max Wind: " << period.v_max()
         << ", Min Pressure: " << period.central_pressure()
         << ", Max Radius: " << period.r_max() << "\n"
         << "  Quadrants: \n";

  for (const auto &quad : period.quadrants()) {
    stream
        << "    Quadrant "
        << Gahm::Types::QuadCode::quadrant_code_to_string(quad.quadrant_code())
        << " ("
        << Gahm::Types::QuadCode::quadrant_code_to_integer(quad.quadrant_code())
        << "): \n";
    stream << quad;
  }
  stream << period.translation();
  return stream;
}