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
#include "Isotach.h"
#include <cassert>

static constexpr std::array<const char *, 6> radiusString = {
    "AAA", "NEQ", "SEQ", "SWQ", "NWQ", "NONE"};

Isotach::Isotach()
    : m_windSpeed(0.0),
      m_isotachRadius({0.0, 0.0, 0.0, 0.0}),
      m_rmax({0.0, 0.0, 0.0, 0.0}),
      m_hollandB({1.0, 1.0, 1.0, 1.0}),
      m_vmaxBL({0.0, 0.0, 0.0, 0.0}),
      m_quadFlag({false, false, false, false}),
      m_radiusCode(NONE) {}

Isotach::Isotach(Isotach::RadiusCode code, double windSpeed, double r1,
                 double r2, double r3, double r4)
    : m_windSpeed(windSpeed),
      m_isotachRadius({r1, r2, r3, r4}),
      m_radiusCode(code) {}

double Isotach::windSpeed() const { return m_windSpeed; }

void Isotach::setWindSpeed(double windSpeed) { m_windSpeed = windSpeed; }

Isotach::RadiusCode Isotach::code() const { return m_radiusCode; }
void Isotach::setCode(RadiusCode code) { m_radiusCode = code; }

void Isotach::generateQuadFlag() {
  for (size_t i = 0; i < m_quadFlag.size(); ++i) {
    m_quadFlag[i] = m_isotachRadius.at(i) > 0;
  }
}

bool Isotach::isNull(const Isotach &iso) {
  if (iso.windSpeed() == 0.0) return true;
  if (iso.cisotachRadius()->at(0) <= 0.0 &&
      iso.cisotachRadius()->at(1) <= 0.0 &&
      iso.cisotachRadius()->at(2) <= 0.0 && iso.cisotachRadius()->at(3) <= 0.0)
    return true;
  if (iso.code() == RadiusCode::NONE) return true;
  return false;
}

Isotach::RadiusCode Isotach::codeFromString(const std::string &code) {
  if (code == "AAA") return AAA;
  if (code == "NEQ") return NEQ;
  if (code == "SEQ") return SEQ;
  if (code == "SWQ") return SWQ;
  if (code == "NWQ") return NWQ;
  return NONE;
}

std::string Isotach::stringFromCode(Isotach::RadiusCode code) {
  return radiusString[code];
}

Isotach::RadiusCode Isotach::radiusCode() const { return m_radiusCode; }

void Isotach::setRadiusCode(const RadiusCode &radiusCode) {
  m_radiusCode = radiusCode;
}

CircularArray<bool, 4> *Isotach::quadFlag() { return &m_quadFlag; }

CircularArray<double, 4> *Isotach::isotachRadius() { return &m_isotachRadius; }

CircularArray<double, 4> *Isotach::rmax() { return &m_rmax; }

CircularArray<double, 4> *Isotach::vmaxBl() { return &m_vmaxBL; }

CircularArray<double, 4> *Isotach::hollandB() { return &m_hollandB; }

CircularArray<double, 4> *Isotach::phi() { return &m_phi; }

const CircularArray<bool, 4> *Isotach::cquadFlag() const { return &m_quadFlag; }

const CircularArray<double, 4> *Isotach::cisotachRadius() const {
  return &m_isotachRadius;
}

const CircularArray<double, 4> *Isotach::crmax() const { return &m_rmax; }

const CircularArray<double, 4> *Isotach::cvmaxBl() const { return &m_vmaxBL; }

const CircularArray<double, 4> *Isotach::chollandB() const {
  return &m_hollandB;
}

const CircularArray<double, 4> *Isotach::cphi() const { return &m_phi; }

std::ostream &operator<<(std::ostream &os, const Isotach &iso) {
  os << Isotach::stringFromCode(iso.code()) << ", " << iso.windSpeed()
     << " m/s, [" << iso.cisotachRadius()->at(0) << ", "
     << iso.cisotachRadius()->at(1) << ", " << iso.cisotachRadius()->at(2)
     << ", " << iso.cisotachRadius()->at(3) << "]";
  return os;
}
