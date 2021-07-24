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
#ifndef LOGGING_H
#define LOGGING_H

#include <iostream>
#include <string>
namespace Gahm {
class Logging {
 public:
  static void throwError(const std::string &s);
  static void throwError(const std::string &s, const char *file, int line);

  static void logError(const std::string &s,
                       const std::string &heading = std::string());
  static void warning(const std::string &s,
                      const std::string &heading = std::string());
  static void log(const std::string &s,
                  const std::string &heading = std::string());
  static void debug(const std::string &s,
                    const std::string &heading = std::string());

 private:
  Logging() = default;
  static void printMessage(const std::string &header,
                           const std::string &message);
  static void printErrorMessage(const std::string &header,
                                const std::string &message);
};
}  // namespace Gahm
#define gahm_throw_exception(arg) \
  Gahm::Logging::throwError(arg, __FILE__, __LINE__)

#endif  // LOGGING_H
