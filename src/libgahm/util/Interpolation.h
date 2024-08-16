//
// Created by Zach Cobell on 8/16/24.
//

#ifndef GAHM_INTERPOLATION_H
#define GAHM_INTERPOLATION_H

namespace Gahm::Util::Interpolation {

template <typename T>
constexpr auto linear(T value_1, T value_2, double weight) -> T {
  return value_1 + (value_2 - value_1) * weight;
}

}  // namespace Gahm::Util::Interpolation

#endif  // GAHM_INTERPOLATION_H
