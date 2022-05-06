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
#include "StormMotion.h"

#include "UnitConversion.h"
using namespace Gahm;

StormMotion::StormMotion(double uv, double direction, bool degrees)
    : m_u(0.0), m_v(0.0), m_uv(0.0), m_direction(0.0) {
  this->set(uv, direction, degrees);
}

StormMotion::StormMotion(double uv, double u, double v, double direction,
                         bool degrees)
    : m_uv(uv), m_u(u), m_v(v), m_direction(direction) {}

void StormMotion::setU(double u) { m_u = u; }
double StormMotion::u() const { return m_u; }

void StormMotion::setV(double v) { m_v = v; }
double StormMotion::v() const { return m_v; }

void StormMotion::setSpeed(double speed) { m_uv = speed; }
double StormMotion::speed() const { return m_uv; }

void StormMotion::setDirection(double direction) { m_direction = direction; }
double StormMotion::direction() const { return m_direction; }

void StormMotion::setExplicit(const double uv, const double u, const double v,
                              const double direction) {
  m_uv = uv;
  m_u = u;
  m_v = v;
  m_direction = direction;
}

void StormMotion::set(double speed, double direction, bool degrees) {
  if (degrees) direction *= Units::convert(Units::Degree, Units::Radian);
  m_direction = direction;
  m_uv = 1.5 *
         std::pow(speed * Units::convert(Units::MetersPerSecond, Units::Knot),
                  0.63) *
         Units::convert(Units::Knot, Units::MetersPerSecond);
  m_u = std::sin(m_direction) * m_uv;
  m_v = std::cos(m_direction) * m_uv;
}

void StormMotion::set(const StormMotion &s) {
  m_direction = s.direction();
  m_uv = s.speed();
  m_u = s.u();
  m_v = s.v();
}