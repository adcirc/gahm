//
// Created by Zach Cobell on 6/15/21.
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
