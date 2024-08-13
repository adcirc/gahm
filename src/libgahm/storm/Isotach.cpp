//
// Created by Zach Cobell on 7/30/24.
//

#include "Isotach.h"

#include <ostream>

auto operator<<(std::ostream &stream,
                const Gahm::Storm::Isotach &isotach) -> std::ostream & {
  stream << "Isotach: " << isotach.wind_speed() << ", " << isotach.radius()
         << "m";
  return stream;
}