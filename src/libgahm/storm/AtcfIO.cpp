//
// Created by Zach Cobell on 7/30/24.
//

#include "AtcfIO.h"

#include <array>
#include <cstddef>
#include <exception>
#include <fstream>
#include <iostream>
#include <optional>
#include <vector>

#include "AtcfPeriod.h"
#include "datatypes/Datetime.h"
#include "datatypes/QuadCode.h"
#include "physical/Units.h"
#include "storm/Isotach.h"
#include "storm/Quadrant.h"
#include "util/ioutils.h"

/**
 * Convert the ATCF ordering of the radial data (by isotach is major index)
 * to the internal representation that we need (by quadrant as major index)
 *
 * @param isotachs Vector of TempIsotach objects that will be transformed
 *
 * @return Array of quadrants
 */
auto Gahm::Atcf::AtcfIO::transpose_to_quadrants(
    double latitude,
    const std::vector<Gahm::Atcf::AtcfIO::TempIsotach> &isotachs)
    -> std::array<Gahm::Storm::Quadrant, 4> {
  std::array<Gahm::Storm::Quadrant, 4> quadrants;

  // The Isotachs vector needs to be length 3. Fill out if not
  std::array<Gahm::Atcf::AtcfIO::TempIsotach, 3> isotach_array = {};
  for (size_t i = 0; i < 3; i++) {
    if (i < isotachs.size()) {
      isotach_array.at(i) = isotachs.at(i);
    }
  }

  // Do the transpose
  for (unsigned i = 0; i < 4; ++i) {
    auto i0 = Storm::Isotach(isotach_array.at(0).wind_speed,
                             isotach_array.at(0).distance.at(i));
    auto i1 = Storm::Isotach(isotach_array.at(1).wind_speed,
                             isotach_array.at(1).distance.at(i));
    auto i2 = Storm::Isotach(isotach_array.at(2).wind_speed,
                             isotach_array.at(2).distance.at(i));
    quadrants.at(i) = Storm::Quadrant(
        Types::QuadCode::integer_to_quadrant_code(i), latitude, {i0, i1, i2});
  }

  return quadrants;
}

/**
 * Read the atcf file format and put into an AtcfTack data structure
 *
 * The function returns an optional with an AtcfTrack or nullopt if the
 * read failed
 *
 * @return std::optional of type AtcfTrack
 */
auto Gahm::Atcf::AtcfIO::read() const -> std::optional<AtcfTrack> {
  std::ifstream file(m_filename);
  if (!file.is_open()) {
    std::cerr << "Could not open file: " << m_filename << '\n';
    return std::nullopt;
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
    std::vector<Gahm::Atcf::AtcfIO::TempIsotach> isotachs;
    std::string line;
    while (std::getline(file, line)) {
      const auto tokens = io::split_string(line);

      const auto datetime =
          Types::Datetime(tokens.at(2), std::stoi(tokens.at(5)));
      const auto lon = io::parse_position(tokens.at(7));
      const auto lat = io::parse_position(tokens.at(6));
      const auto v_max = std::stod(tokens.at(8)) * kt_to_ms;
      const auto c_pressure = std::stod(tokens.at(9)) * mb_to_pa;
      const auto bk_pressure = 1013.0 * mb_to_pa;

      const auto this_isotach = [&]() -> Gahm::Atcf::AtcfIO::TempIsotach {
        const auto isotach_speed = std::stod(tokens.at(11)) * kt_to_ms;
        const auto isotach_distance_1 = std::stod(tokens.at(13)) * nmi_to_m;
        const auto isotach_distance_2 = std::stod(tokens.at(14)) * nmi_to_m;
        const auto isotach_distance_3 = std::stod(tokens.at(15)) * nmi_to_m;
        const auto isotach_distance_4 = std::stod(tokens.at(16)) * nmi_to_m;
        return {isotach_speed,
                {isotach_distance_1, isotach_distance_2, isotach_distance_3,
                 isotach_distance_4}};
      }();

      const auto r_max = [&]() -> double {
        if (tokens.size() < 20) {
          return 0.0;
        } else {
          return std::stod(tokens.at(19)) * nmi_to_m;
        }
      }();

      if (datetime == previous_datetime) {
        isotachs.push_back(this_isotach);
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

    std::cout << AtcfTrack(periods);

    return std::make_optional<AtcfTrack>(periods);
  } catch (const std::exception &e) {
    std::cerr << "Error reading file: " << e.what() << '\n';
    return std::nullopt;
  }
}