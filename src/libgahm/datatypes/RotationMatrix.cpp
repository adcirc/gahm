//
// Created by Zach Cobell on 8/1/24.
//

#include "RotationMatrix.hpp"

#include <ostream>

auto operator<<(std::ostream &stream,
                const Gahm::Types::RotationMatrix &matrix) -> std::ostream & {
  stream << "Rotation Matrix:\n";
  stream << "  " << matrix.at(0, 0) << " " << matrix.at(0, 1) << "\n";
  stream << "  " << matrix.at(1, 0) << " " << matrix.at(1, 1) << "\n";
  return stream;
}
