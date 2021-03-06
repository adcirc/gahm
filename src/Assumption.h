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
#ifndef ASSUMPTION_H
#define ASSUMPTION_H

#include <string>
namespace Gahm {
class Assumption {
 public:
  enum Severity { INFO, MINOR, MAJOR, CRITICAL };

  Assumption(Gahm::Assumption::Severity s, std::string message,
             std::string file, size_t line);

  Severity severity() const;

  std::string message() const;

  std::string filename() const;

  size_t line() const;

  std::string toString() const;
  static std::string severityString(Gahm::Assumption::Severity s);

 private:
  const Severity m_severity;
  const std::string m_message;
  const std::string m_filename;
  const size_t m_line;
};
}  // namespace Gahm

#endif  // ASSUMPTION_H
