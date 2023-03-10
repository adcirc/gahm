// GNU General Public License v3.0
//
// This file is part of the GAHM model (https://github.com/adcirc/gahm).
// Copyright (c) 2023 ADCIRC Development Group.
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, version 3.
//
// This program is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program. If not, see <http://www.gnu.org/licenses/>.
//
// Author: Zach Cobell
// Contact: zcobell@thewaterinstitute.org
//
#ifndef GAHM_SRC_UTIL_STRINGUTILITIES_H_
#define GAHM_SRC_UTIL_STRINGUTILITIES_H_

#include <string>
#include <vector>

#include "boost/algorithm/string/split.hpp"
#include "boost/algorithm/string/trim.hpp"

namespace Gahm::detail::Utilities {
std::vector<std::string> splitString(const std::string &s) {
  const auto s2 = boost::trim_copy_if(s, boost::is_any_of(" "));
  std::vector<std::string> results;
  boost::algorithm::split(results, s2, boost::is_any_of(","),
                          boost::token_compress_off);
  return results;
}

template <typename T,
          typename = typename std::enable_if<std::is_fundamental_v<T>>::type>
T readValueCheckBlank(const std::string &s) {
  const auto s_copy = boost::trim_copy(s);
  if (s_copy.empty()) {
    return 0;
  } else {
    if (std::is_integral_v<T>) {
      return stoi(s_copy);
    } else if (std::is_floating_point_v<T>) {
      return stod(s_copy);
    } else if (std::is_same_v<bool, T>) {
      auto i = stoi(s_copy);
      return i != 0;
    } else {
      // You should not be here
      return 0;
    }
  }
}
}  // namespace Gahm::detail::Utilities

#endif  // GAHM_SRC_UTIL_STRINGUTILITIES_H_
