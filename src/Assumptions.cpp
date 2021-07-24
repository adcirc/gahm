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
#include "Assumptions.h"

#include <cassert>
#include <iostream>

using namespace Gahm;

void Assumptions::add(const Assumption &a) { m_assumptions.push_back(a); }

Assumption Assumptions::get(size_t index) const {
  assert(index < m_assumptions.size());
  return m_assumptions[index];
}

size_t Assumptions::count() const { return m_assumptions.size(); }

void Assumptions::log(const Assumption::Severity s) {
  for (const auto &a : m_assumptions) {
    if (a.severity() >= s) {
      const std::string logmessage = a.toString();
      std::cout << a.toString() << "\n";
    }
  }
  std::cout.flush();
}
