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
#ifndef ISOTACH_H
#define ISOTACH_H

#include <array>
#include <ostream>
#include <string>
#include <vector>

#include "CircularArray.h"
#include "ParameterPack.h"
namespace Gahm {
class Isotach {
 public:
  enum RadiusCode { AAA, NEQ, SEQ, SWQ, NWQ, NONE };

  Isotach();

  Isotach(RadiusCode code, double windSpeed, double r1, double r2, double r3,
          double r4);

  double windSpeed() const;
  void setWindSpeed(double windSpeed);

  Gahm::Isotach::RadiusCode code() const;
  void setCode(Gahm::Isotach::RadiusCode code);

  void generateQuadFlag();

  static bool isNull(const Isotach &iso);

  static Isotach::RadiusCode codeFromString(const std::string &code);
  static std::string stringFromCode(Gahm::Isotach::RadiusCode code);

  Gahm::Isotach::RadiusCode radiusCode() const;
  void setRadiusCode(const Gahm::Isotach::RadiusCode &radiusCode);

  Gahm::ParameterPack parameterPack(int quad) const;

  Gahm::CircularArray<bool, 4> &quadrant_flag() { return m_quadrant_flag; }
  const Gahm::CircularArray<bool, 4> &quadrant_flag() const {
    return m_quadrant_flag;
  }

  Gahm::CircularArray<double, 4> &isotach_radius() { return m_isotachRadius; }
  const Gahm::CircularArray<double, 4> &isotach_radius() const {
    return m_isotachRadius;
  }

  Gahm::CircularArray<bool, 4> &isotach_radius_null_input() {
    return m_isotachRadiusNullInInput;
  }
  const Gahm::CircularArray<bool, 4> &isotach_radius_null_input() const {
    return m_isotachRadiusNullInInput;
  }

  Gahm::CircularArray<double, 4> &quadrant_radius_to_max_winds() {
    return m_quadrant_radius_max_winds;
  }
  const Gahm::CircularArray<double, 4> &quadrant_radius_to_max_winds() const {
    return m_quadrant_radius_max_winds;
  }

  Gahm::CircularArray<double, 4> &quadrant_vmax_boundary_layer() {
    return m_quadrant_vmax_boundary_layer;
  }
  const Gahm::CircularArray<double, 4> &quadrant_vmax_boundary_layer() const {
    return m_quadrant_vmax_boundary_layer;
  }

  Gahm::CircularArray<double, 4> &quadrant_holland_b() {
    return m_quadrant_holland_b;
  }
  const Gahm::CircularArray<double, 4> &quadrant_holland_b() const {
    return m_quadrant_holland_b;
  }

  Gahm::CircularArray<double, 4> &quadrant_phi() { return m_quadrant_phi; }
  const Gahm::CircularArray<double, 4> &quadrant_phi() const {
    return m_quadrant_phi;
  }

  Gahm::CircularArray<double, 4> &quadrant_isotach_wind_speed() {
    return m_quadrant_velocity;
  }
  const Gahm::CircularArray<double, 4> &quadrant_isotach_wind_speed() const {
    return m_quadrant_velocity;
  }

 private:
  double m_windSpeed;
  Gahm::CircularArray<double, 4> m_isotachRadius;
  Gahm::CircularArray<double, 4> m_quadrant_radius_max_winds;
  Gahm::CircularArray<double, 4> m_quadrant_holland_b;
  Gahm::CircularArray<double, 4> m_quadrant_vmax_boundary_layer;
  Gahm::CircularArray<double, 4> m_quadrant_phi;
  Gahm::CircularArray<double, 4> m_quadrant_velocity;
  Gahm::CircularArray<bool, 4> m_quadrant_flag;
  Gahm::CircularArray<bool, 4> m_isotachRadiusNullInInput;
  Gahm::Isotach::RadiusCode m_radiusCode;
};
}  // namespace Gahm
std::ostream &operator<<(std::ostream &os, const Gahm::Isotach &iso);

#endif  // ISOTACH_H
