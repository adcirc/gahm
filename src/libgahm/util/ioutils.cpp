//
// Created by Zach Cobell on 7/30/24.
//

#include "ioutils.h"

#include "boost/algorithm/string.hpp"

auto Gahm::Util::IO::split_string(const std::string &str)
    -> std::vector<std::string> {
  std::vector<std::string> v;
  boost::split(v, str, boost::is_any_of(","), boost::token_compress_off);

  // For each token, remove leading and trailing whitespace
  for (auto &s : v) {
    boost::trim(s);
  }

  return v;
}

auto Gahm::Util::IO::parse_position(const std::string &position) -> double {
  // Parse the position. Use the last character to determine if it is
  //  either N/S or E/W, divide by 10, and multiply by -1 if S or W

  double pos = std::stod(position.substr(0, position.size() - 1));
  if (position.back() == 'S' || position.back() == 'W') {
    pos *= -1;
  }

  return pos / 10;
}