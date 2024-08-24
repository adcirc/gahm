//
// Created by Zach Cobell on 7/30/24.
//

#ifndef GAHM_ATCFIO_HPP
#define GAHM_ATCFIO_HPP

#include <array>
#include <string>
#include <utility>
#include <vector>

#include "AtcfTrack.hpp"
#include "storm/Quadrant.hpp"

namespace Gahm::Atcf {

/**
   * Quick structure used to aid in performing the transpose to quadrant based
   * data
 */
struct TempIsotach {
  double wind_speed;
  std::array<double, 4> distance;

  constexpr TempIsotach(double in_wind_speed,
                        std::array<double, 4> in_distance)
      : wind_speed(in_wind_speed), distance(in_distance) {}

  constexpr TempIsotach() : wind_speed(0.0), distance({0.0, 0.0, 0.0, 0.0}) {}
};


class AtcfIO {
 public:
  explicit AtcfIO(std::string filename) : m_filename(std::move(filename)) {}

  [[nodiscard]] auto read() const -> Gahm::Atcf::AtcfTrack;

  static auto transpose_to_quadrants(
      double latitude,
      const std::vector<Gahm::Atcf::TempIsotach> &isotachs)
      -> std::array<Gahm::Storm::Quadrant, 4>;

  static auto sanitize_temp_isotach(double radius_to_max_winds,
                                    const Gahm::Atcf::TempIsotach &temp_isotach)
      -> Gahm::Atcf::TempIsotach;

 private:
  std::string m_filename;
};

}  // namespace Gahm::Atcf
#endif  // GAHM_ATCFIO_HPP
