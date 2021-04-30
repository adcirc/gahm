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
#include "Assumption.h"

Assumption::Assumption(const Assumption::Severity s, const std::string &message,
                       const std::string file, const size_t line)
    : m_severity(s), m_message(message), m_filename(file), m_line(line) {}

Assumption::Severity Assumption::severity() const { return m_severity; }

std::string Assumption::message() const { return m_message; }

std::string Assumption::filename() const { return m_filename; }

size_t Assumption::line() const { return m_line; }

std::string Assumption::toString() const {
  return "ASSUMPTION: " + Assumption::severityString(m_severity) + " [" +
         this->m_filename + ", " + std::to_string(m_line) + "]: " + m_message;
}

std::string Assumption::severityString(Assumption::Severity s) {
  switch (s) {
    case Assumption::Severity::INFO:
      return "INFO";
    case Assumption::Severity::MINOR:
      return "MINOR";
    case Assumption::Severity::MAJOR:
      return "MAJOR";
    case Assumption::Severity::CRITICAL:
      return "CRITICAL";
  }
}
