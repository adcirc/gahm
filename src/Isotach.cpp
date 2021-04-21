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
#include "isotach.h"
#include <cassert>

static constexpr std::array<const char *, 6> radiusString = {
    "AAA", "NEQ", "SEQ", "SWQ", "NWQ", "NONE"};

Isotach::Isotach()
    : m_windSpeed(0.0), m_radius{0.0, 0.0, 0.0, 0.0}, m_radiusCode(NONE) {}

Isotach::Isotach(Isotach::RadiusCode code, double windSpeed, double r1,
                 double r2, double r3, double r4)
    : m_windSpeed(windSpeed), m_radius{r1, r2, r3, r4}, m_radiusCode(code) {}

double Isotach::windSpeed() const { return m_windSpeed; }

void Isotach::setWindSpeed(double windSpeed) { m_windSpeed = windSpeed; }

double Isotach::radiusQ1() const { return m_radius[0]; }

void Isotach::setRadiusQ1(double radiusQ1) { m_radius[0] = radiusQ1; }

double Isotach::radiusQ2() const { return m_radius[1]; }

void Isotach::setRadiusQ2(double radiusQ2) { m_radius[1] = radiusQ2; }

double Isotach::radiusQ3() const { return m_radius[2]; }

void Isotach::setRadiusQ3(double radiusQ3) { m_radius[2] = radiusQ3; }

double Isotach::radiusQ4() const { return m_radius[3]; }

void Isotach::setRadiusQ4(double radiusQ4) { m_radius[3] = radiusQ4; }

std::array<double, 4> Isotach::radii() { return m_radius; }

void Isotach::setRadii(std::array<double, 4> radii) { m_radius = radii; }

void Isotach::setRadii(double r1, double r2, double r3, double r4) {
  m_radius = {r1, r2, r3, r4};
}

void Isotach::setRadii(std::vector<double> radii) {
  assert(radii.size() == 4);
  m_radius = {radii[0], radii[1], radii[2], radii[3]};
}

Isotach::RadiusCode Isotach::code() const { return m_radiusCode; }
void Isotach::setCode(RadiusCode code) { m_radiusCode = code; }

bool Isotach::isNull(const Isotach &iso) {
  if (iso.windSpeed() == 0.0) return true;
  if (iso.radiusQ1() <= 0.0 && iso.radiusQ2() <= 0.0 && iso.radiusQ3() <= 0.0 &&
      iso.radiusQ4() <= 0.0)
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

std::ostream &operator<<(std::ostream &os, const Isotach &iso) {
  os << Isotach::stringFromCode(iso.code()) << ", " << iso.windSpeed()
     << " m/s, [" << iso.radiusQ1() << ", " << iso.radiusQ2() << ", "
     << iso.radiusQ3() << ", " << iso.radiusQ4() << "]";
  return os;
}
