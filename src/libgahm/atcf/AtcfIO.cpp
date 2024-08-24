//
// Created by Zach Cobell on 7/30/24.
//

#include "AtcfIO.hpp"

#include <algorithm>
#include <array>
#include <cstddef>
#include <exception>
#include <fstream>
#include <iostream>
#include <iterator>
#include <numeric>
#include <optional>
#include <stdexcept>
#include <string>
#include <vector>

#include "AtcfPeriod.hpp"
#include "AtcfTrack.hpp"
#include "datatypes/Datetime.hpp"
#include "datatypes/QuadCode.hpp"
#include "physical/Units.hpp"
#include "storm/Isotach.hpp"
#include "storm/Quadrant.hpp"
#include "util/ioutils.hpp"

/**
 * Convert the ATCF ordering of the radial data (by isotach is major index)
 * to the internal representation that we need (by quadrant as major index)
 *
 * @param isotachs Vector of TempIsotach objects that will be transformed
 *
 * @return Array of quadrants
 */
auto Gahm::Atcf::AtcfIO::transpose_to_quadrants(
    double latitude, const std::vector<Gahm::Atcf::TempIsotach> &isotachs)
    -> std::array<Gahm::Storm::Quadrant, 4> {
  std::array<Gahm::Storm::Quadrant, 4> quadrants;

  // The Isotachs vector needs to be length 3. Fill out if not
  std::array<Gahm::Atcf::TempIsotach, 3> isotach_array = {};
  for (size_t i = 0; i < 3; i++) {
    if (i < isotachs.size()) {
      isotach_array.at(i) = isotachs.at(i);
    }
  }

  // Isotachs are always organized as 34, 50, and 64 kt winds
  constexpr auto kt2ms = Gahm::Physical::Units::convert(
      Gahm::Physical::Units::Knot, Gahm::Physical::Units::MetersPerSecond);

  // Do the transpose
  for (unsigned i = 0; i < 4; ++i) {
    const auto iso_0 =
        Storm::Isotach(34.0 * kt2ms, isotach_array.at(0).distance.at(i));
    const auto iso_1 =
        Storm::Isotach(50.0 * kt2ms, isotach_array.at(1).distance.at(i));
    const auto iso_2 =
        Storm::Isotach(64.0 * kt2ms, isotach_array.at(2).distance.at(i));
    quadrants.at(i) =
        Storm::Quadrant(Types::QuadCode::integer_to_quadrant_code(i), latitude,
                        {iso_0, iso_1, iso_2});
  }

  return quadrants;
}

auto Gahm::Atcf::AtcfIO::sanitize_temp_isotach(
    double radius_to_max_winds,
    const Gahm::Atcf::TempIsotach &temp_isotach) -> TempIsotach {
  const auto n_quad_populated =
      std::count_if(temp_isotach.distance.begin(), temp_isotach.distance.end(),
                    [](const auto &d) { return d > 0.0; });
  if (n_quad_populated == 0) {
    // Set all radii to r_max
    return TempIsotach(temp_isotach.wind_speed,
                       {radius_to_max_winds, radius_to_max_winds,
                        radius_to_max_winds, radius_to_max_winds});
  } else if (n_quad_populated == 1) {
    // Populate all with the one provided value
    const auto populated_value = *std::find_if(
        temp_isotach.distance.begin(), temp_isotach.distance.end(),
        [](const auto &d) { return d > 0.0; });
    const std::array<double, 4> filled_radii = {
        populated_value, populated_value, populated_value, populated_value};
    return {temp_isotach.wind_speed, filled_radii};
  } else if (n_quad_populated == 2) {
    // Fill in the missing value with the mean of the two provided
    const auto mean_radius = std::accumulate(temp_isotach.distance.begin(),
                                             temp_isotach.distance.end(), 0.0) /
                             2.0;
    auto filled_radii = temp_isotach.distance;
    std::replace_if(
        filled_radii.begin(), filled_radii.end(),
        [](const auto &d) { return d == 0.0; }, mean_radius);
    return {temp_isotach.wind_speed, filled_radii};
  } else if (n_quad_populated == 3) {
    // Fill in the missing value with the mean of the adjacent values
    const auto index_not_populated =
        std::distance(temp_isotach.distance.begin(),
                      std::find(temp_isotach.distance.begin(),
                                temp_isotach.distance.end(), 0.0));
    const auto index_1 =
        static_cast<std::size_t>((index_not_populated - 1) % 4);
    const auto index_2 =
        static_cast<std::size_t>((index_not_populated + 1) % 4);
    const auto mean_radius = (temp_isotach.distance.at(index_1) +
                              temp_isotach.distance.at(index_2)) /
                             2.0;
    auto filled_radii = temp_isotach.distance;
    filled_radii.at(static_cast<size_t>(index_not_populated)) = mean_radius;
    return {temp_isotach.wind_speed, filled_radii};
  } else {
    return {temp_isotach.wind_speed, temp_isotach.distance};
  }
}

/**
 * Read the atcf file format and put into an AtcfTack data structure
 *
 * The function returns an optional with an AtcfTrack or nullopt if the
 * read failed
 *
 * @return std::optional of type AtcfTrack
 */
auto Gahm::Atcf::AtcfIO::read() const -> AtcfTrack {
  std::ifstream file(m_filename);
  if (!file.is_open()) {
    throw std::runtime_error("Could not open file");
  }

  constexpr auto kt_to_ms = Gahm::Physical::Units::convert(
      Gahm::Physical::Units::Knot, Gahm::Physical::Units::MetersPerSecond);
  constexpr auto mb_to_pa = Gahm::Physical::Units::convert(
      Gahm::Physical::Units::Millibar, Gahm::Physical::Units::Pascal);
  constexpr auto nmi_to_m = Gahm::Physical::Units::convert(
      Gahm::Physical::Units::NauticalMile, Gahm::Physical::Units::Meter);

  try {
    Types::Datetime previous_datetime;
    std::vector<AtcfPeriod> periods;
    std::vector<Gahm::Atcf::TempIsotach> isotachs;
    std::string line;
    while (std::getline(file, line)) {
      const auto tokens = Util::IO::split_string(line);

      const auto datetime =
          Types::Datetime(tokens.at(2), std::stoi(tokens.at(5)));
      const auto lon = Util::IO::parse_position(tokens.at(7));
      const auto lat = Util::IO::parse_position(tokens.at(6));
      const auto v_max = std::stod(tokens.at(8)) * kt_to_ms;
      const auto c_pressure = std::stod(tokens.at(9)) * mb_to_pa;
      const auto bk_pressure = 1013.0 * mb_to_pa;

      const auto r_max = [&]() -> double {
        if (tokens.size() < 20) {
          return 0.0;
        } else {
          return std::stod(tokens.at(19)) * nmi_to_m;
        }
      }();

      const auto this_isotach = [&]() -> Gahm::Atcf::TempIsotach {
        const auto isotach_speed = std::stod(tokens.at(11)) * kt_to_ms;
        const auto isotach_distance_1 = std::stod(tokens.at(13)) * nmi_to_m;
        const auto isotach_distance_2 = std::stod(tokens.at(14)) * nmi_to_m;
        const auto isotach_distance_3 = std::stod(tokens.at(15)) * nmi_to_m;
        const auto isotach_distance_4 = std::stod(tokens.at(16)) * nmi_to_m;

        const TempIsotach iso = {isotach_speed,
                                 {isotach_distance_1, isotach_distance_2,
                                  isotach_distance_3, isotach_distance_4}};

        auto iso_sanitized = AtcfIO::sanitize_temp_isotach(r_max, iso);
        if (iso_sanitized.wind_speed == 0.0) {
          iso_sanitized.wind_speed = v_max;
        }

        return iso_sanitized;
      }();

      if (datetime == previous_datetime) {
        if (this_isotach.wind_speed > 0.0) {
          isotachs.push_back(this_isotach);
        }
      } else {
        if (!isotachs.empty()) {
          if (r_max > 0.0 || isotachs.size() > 1) {
            auto quadrants =
                Gahm::Atcf::AtcfIO::transpose_to_quadrants(lat, isotachs);
            periods.push_back(AtcfPeriod(datetime, c_pressure, bk_pressure,
                                         v_max, r_max, {lon, lat}, quadrants));
          }
          isotachs.clear();
        }
        isotachs.push_back(this_isotach);
        previous_datetime = datetime;
      }
    }

    return AtcfTrack(periods);

  } catch (const std::exception &e) {
    throw std::runtime_error("Error reading ATCF file: " +
                             std::string(e.what()));
  }
}