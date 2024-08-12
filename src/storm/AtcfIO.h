//
// Created by Zach Cobell on 7/30/24.
//

#ifndef GAHM_ATCFIO_H
#define GAHM_ATCFIO_H

#include <array>
#include <chrono>
#include <optional>
#include <string>
#include <utility>

#include "AtcfTrack.h"

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
  };

  static auto transpose_to_quadrants(
      double latitude,
      const std::vector<Gahm::Atcf::AtcfIO::TempIsotach> &isotachs)
      -> std::array<Gahm::Storm::Quadrant, 4>;

 private:
  std::string m_filename;
};

}  // namespace Gahm::Atcf
#endif  // GAHM_ATCFIO_H
