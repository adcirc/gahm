// MIT License
//
// Copyright (c) 2023 ADCIRC Development Group
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
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
