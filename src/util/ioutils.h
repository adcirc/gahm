//
// Created by Zach Cobell on 7/30/24.
//

#ifndef GAHM_IOUTILS_H
#define GAHM_IOUTILS_H

#include <chrono>
#include <string>

namespace Gahm::io {

auto split_string(const std::string &str) -> std::vector<std::string>;

//auto parse_datetime(const std::string &datetime, int hours = 0) -> std::chrono::time_point<std::chrono::steady_clock>;

auto parse_position(const std::string &position) -> double;

}  // namespace Gahm::io

#endif  // GAHM_IOUTILS_H
