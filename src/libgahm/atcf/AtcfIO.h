//
// Created by Zach Cobell on 7/30/24.
//

#ifndef GAHM_ATCFIO_H
#define GAHM_ATCFIO_H

#include <array>
#include <optional>
#include <string>
#include <utility>
#include <vector>

#include "AtcfTrack.h"
#include "storm/Quadrant.h"

namespace Gahm::Atcf {

class AtcfIO {
 public:
  explicit AtcfIO(std::string filename) : m_filename(std::move(filename)) {}

  [[nodiscard]] auto read() const -> std::optional<AtcfTrack>;

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

  static auto transpose_to_quadrants(
      double latitude,
      const std::vector<Gahm::Atcf::AtcfIO::TempIsotach> &isotachs)
      -> std::array<Gahm::Storm::Quadrant, 4>;

  static auto sanitize_temp_isotach(double radius_to_max_winds,
                                    const TempIsotach &temp_isotach)
      -> TempIsotach;

 private:
  std::string m_filename;
};

}  // namespace Gahm::Atcf
#endif  // GAHM_ATCFIO_H
