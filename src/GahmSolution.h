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
#ifndef GAHM_SRC_GAHMSOLUTION_H_
#define GAHM_SRC_GAHMSOLUTION_H_

#include <vector>

#include "GahmSolutionPoint.h"

class GahmSolution {
 public:
  GahmSolution() = default;

  explicit GahmSolution(size_t size) { m_solution.reserve(size); }

  GahmSolution(size_t size, const GahmSolutionPoint &p) : m_solution(size, p) {}

  size_t size() const { return m_solution.size(); }

  GahmSolutionPoint &operator[](size_t index) {
    assert(index < m_solution.size());
    return m_solution[index];
  }

  GahmSolutionPoint &at(size_t index) { return this->operator[](index); }

  void push_back(const GahmSolutionPoint &p) { m_solution.push_back(p); }
  void emplace_back(const GahmSolutionPoint &p) { m_solution.emplace_back(p); }

  void reserve(const size_t sz) { m_solution.reserve(sz); }

 private:
  std::vector<GahmSolutionPoint> m_solution;
};

#endif  // GAHM_SRC_GAHMSOLUTION_H_
