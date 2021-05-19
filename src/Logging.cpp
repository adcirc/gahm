// MIT License
//
// Copyright (c) 2020 ADCIRC Development Group
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
#include "Logging.h"

static const std::string c_errorHeading("[GAHM ERROR]: ");
static const std::string c_warningHeading("[GAHM WARNING]: ");
static const std::string c_logHeading("[GAHM INFO]: ");
static const std::string c_debugHeading("[GAHM DEBUG]: ");

/**
 * @brief Throws a runtime error
 * @param[in] s error description
 */
void Logging::throwError(const std::string &s) {
  throw std::runtime_error(c_errorHeading + s);
}

/**
 * @brief Throws an error with file name and line number
 * @param[in] s error description
 * @param[in] file file name where error occured
 * @param[in] line line number where error occured
 */
void Logging::throwError(const std::string &s, const char *file, int line) {
  throw std::runtime_error(c_errorHeading + s + " at " + file + ", line " +
                           std::to_string(line));
}

/**
 * @brief Logs an error message to the screen
 * @param s error message
 */
void Logging::logError(const std::string &s, const std::string &heading) {
  std::string header = c_errorHeading;
  if (heading != std::string()) {
    header = heading;
  }
  Logging::printErrorMessage(heading, s);
}

/**
 * @brief Logs a warning message
 * @param[in] s warning message
 */
void Logging::warning(const std::string &s, const std::string &heading) {
  std::string header = c_warningHeading;
  if (heading != std::string()) {
    header = heading;
  }
  Logging::printMessage(header, s);
}

/**
 * @brief Informational log message
 * @param s message string
 */
void Logging::log(const std::string &s, const std::string &heading) {
  std::string header = c_logHeading;
  if (heading != std::string()) {
    header = heading;
  }
  Logging::printMessage(header, s);
}

void Logging::debug(const std::string &s, const std::string &heading) {
#ifdef EBUG
  std::string header = c_debugHeading;
  if (heading != std::string()) {
    header = heading;
  }
  Logging::printMessage(header, s);
#endif
}

/**
 * @brief Prints a message to screen
 * @param header custom header
 * @param message log message
 */
void Logging::printMessage(const std::string &header,
                           const std::string &message) {
  std::cout << header << message << std::endl;
}

/**
 * @brief Prints a message to standard error
 * @param header custom header
 * @param message log message
 */
void Logging::printErrorMessage(const std::string &header,
                                const std::string &message) {
  std::cerr << header << message << std::endl;
}
