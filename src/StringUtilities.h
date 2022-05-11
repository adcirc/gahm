//
// Created by Zach Cobell on 5/10/22.
//

#ifndef GAHM2_SRC_STRINGUTILITIES_H_
#define GAHM2_SRC_STRINGUTILITIES_H_

#include <string>
#include <vector>

#include "Logging.h"
#include "boost/algorithm/string/split.hpp"
#include "boost/algorithm/string/trim.hpp"

namespace Gahm::StringUtilities {

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

}  // namespace Gahm::StringUtilities
#endif  // GAHM2_SRC_STRINGUTILITIES_H_
