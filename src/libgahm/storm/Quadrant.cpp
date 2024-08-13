//
// Created by Zach Cobell on 8/4/24.
//

#include "Quadrant.h"

#include <ostream>

#include "storm/Isotach.h"

auto operator<<(std::ostream& stream,
                const Gahm::Storm::Quadrant& quadrant) -> std::ostream& {
  for (const auto& isotach : quadrant.isotachs()) {
    stream << "Quadrant: " << isotach << "\n";
  }
  return stream;
}
