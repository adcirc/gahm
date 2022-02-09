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

#include "WindData.h"

#include <cassert>

#include "Constants.h"

using namespace Gahm;

WindData::WindData() : m_n(0) {}

WindData::WindData(size_t n)
    : m_n(n), m_data(n, Gahm::Uvp(0, 0, Physical::backgroundPressure())) {}

WindData::WindData(size_t n, const Gahm::Uvp& uvp) : m_n(n), m_data(n, uvp) {}

std::vector<double> WindData::u() const {
  std::vector<double> u;
  u.reserve(m_n);
  for (const auto& d : m_data) {
    u.push_back(d.u());
  }
  return u;
}

std::vector<double> WindData::v() const {
  std::vector<double> v;
  v.reserve(m_n);
  for (const auto& d : m_data) {
    v.push_back(d.v());
  }
  return v;
}

std::vector<double> WindData::p() const {
  std::vector<double> p;
  p.reserve(m_n);
  for (const auto& d : m_data) {
    p.push_back(d.p());
  }
  return p;
}

void WindData::set(size_t index, const Gahm::Uvp& record) {
  assert(index < m_n);
  m_data[index] = record;
}

void WindData::setU(size_t index, double value) {
  assert(index < m_data.size());
  m_data[index].setU(value);
}

void WindData::setV(size_t index, double value) {
  assert(index < m_data.size());
  m_data[index].setV(value);
}

void WindData::setP(size_t index, double value) {
  assert(index < m_data.size());
  m_data[index].setP(value);
}

size_t WindData::size() const { return m_n; }

void WindData::setStormParameters(const StormParameters& sp) {
  m_stormParameters = sp;
}

const StormParameters* WindData::stormParameters() const {
  return &m_stormParameters;
}

void WindData::setSize(size_t n) {
  m_n = n;
  m_data.resize(m_n);
}
