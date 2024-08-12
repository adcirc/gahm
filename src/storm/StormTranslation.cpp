//
// Created by Zach Cobell on 7/31/24.
//
#include "StormTranslation.h"

#include <iostream>

#include "physical/Constants.h"

auto operator<<(std::ostream &stream,
                const Gahm::Atcf::StormTranslation &translation)
    -> std::ostream & {
  constexpr auto rad2deg = Gahm::Physical::Constants::rad2deg();
  stream << "  Translation Speed: " << translation.speed() << " m/s\n";
  stream << "  Translation Direction: " << translation.direction() * rad2deg
         << " degrees\n";
  stream << "  Translation Vector: " << translation.velocity().u() << " m/s, "
         << translation.velocity().v() << " m/s\n";
  return stream;
}