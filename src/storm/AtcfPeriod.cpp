//
// Created by Zach Cobell on 7/30/24.
//

#include "AtcfPeriod.h"

#include <iostream>

using namespace Gahm::Atcf;

auto operator<<(std::ostream &stream,
                const Gahm::Atcf::AtcfPeriod &period) -> std::ostream & {
  stream << " Date: " << period.datetime()
         << ", Lat: " << period.eye_location().x()
         << ", Lon: " << period.eye_location().y()
         << ", Max Wind: " << period.v_max()
         << ", Min Pressure: " << period.central_pressure()
         << ", Max Radius: " << period.r_max() << "\n"
         << "  Quadrants: \n";
  for (const auto &quad : period.quadrants()) {
    stream << "  ";
    stream << quad;
  }
  stream << period.translation();
  return stream;
}