//
// Created by Zach Cobell on 8/2/24.
//

#include "Vec.hpp"
#include <ostream>

auto operator<<(std::ostream &stream,
                const Gahm::Types::Vec &vec) -> std::ostream & {
  stream << "  U: " << vec.u() << ", V: " << vec.v() << "\n";
  return stream;
}