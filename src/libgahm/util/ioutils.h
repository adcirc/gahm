//
// Created by Zach Cobell on 7/30/24.
//

#ifndef GAHM_IOUTILS_H
#define GAHM_IOUTILS_H

#include <string>
#include <vector>

namespace Gahm::Util::IO {

auto split_string(const std::string &str) -> std::vector<std::string>;

auto parse_position(const std::string &position) -> double;

}  // namespace Gahm::util::io

#endif  // GAHM_IOUTILS_H
