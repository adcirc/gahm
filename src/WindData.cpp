//
// Created by Zach Cobell on 1/21/21.
//

#include "WindData.h"

#include <cassert>

#include "Constants.h"

WindData::WindData() : m_n(0) {}

WindData::WindData(size_t n)
    : m_n(n),
      m_u(n, 0.0),
      m_v(n, 0.0),
      m_p(n, Constants::backgroundPressure()) {}

WindData::WindData(size_t n, double u, double v, double p)
    : m_n(n), m_u(n, u), m_v(n, v), m_p(n, p) {}

const std::vector<double>& WindData::u() const { return m_u; }

const std::vector<double>& WindData::v() const { return m_v; }

const std::vector<double>& WindData::p() const { return m_p; }

void WindData::set(size_t index, double u, double v, double p) {
  assert(index < m_n);
  m_u[index] = u;
  m_v[index] = v;
  m_p[index] = p;
}

void WindData::setU(size_t index, double value) {
  assert(index < m_u.size());
  m_u[index] = value;
}

void WindData::setV(size_t index, double value) {
  assert(index < m_v.size());
  m_v[index] = value;
}

void WindData::setP(size_t index, double value) {
  assert(index < m_p.size());
  m_p[index] = value;
}

size_t WindData::size() const { return m_n; }

void WindData::setU(const std::vector<double>& u) {
  assert(u.size() == m_n);
  m_u = u;
}

void WindData::setV(const std::vector<double>& v) {
  assert(v.size() == m_n);
  m_v = v;
}

void WindData::setP(const std::vector<double>& p) {
  assert(p.size() == m_n);
  m_p = p;
}

void WindData::setStormParameters(const StormParameters& sp) {
  m_stormParameters = sp;
}

const StormParameters* WindData::stormParameters() const {
  return &m_stormParameters;
}

void WindData::setSize(size_t n){
    m_n = n;
    m_u.resize(m_n);
    m_v.resize(m_n);
    m_p.resize(m_n);
}
