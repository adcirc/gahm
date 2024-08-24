//
// Created by Zach Cobell on 7/30/24.
//

#ifndef GAHM_IOUTILS_HPP
#define GAHM_IOUTILS_HPP

#include <string>
#include <vector>

namespace Gahm::Util::IO {

auto split_string(const std::string &str) -> std::vector<std::string>;

auto parse_position(const std::string &position) -> double;

}  // namespace Gahm::util::io

#endif  // GAHM_IOUTILS_HPP
