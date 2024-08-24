//
// Created by Zach Cobell on 8/2/24.
//

#include "Point.hpp"
#include <ostream>

auto operator<<(std::ostream &stream, const Gahm::Types::Point &point) -> std::ostream & {
  stream << "  X: " << point.x() << ", Y: " << point.y() << "\n";
  return stream;
}