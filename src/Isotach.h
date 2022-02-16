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
 private:
  template <typename S>
  static decltype(auto) quadFlag_internal(S *s) {
    return (s->m_quadFlag);
  }

  template <typename S>
  static decltype(auto) isotachRadius_internal(S *s) {
    return (s->m_isotachRadius);
  }

  template <typename S>
  static decltype(auto) isotachRadiusNullInInput_internal(S *s) {
    return (s->m_isotachRadiusNullInInput);
  }

  template <typename S>
  static decltype(auto) rmax_internal(S *s) {
    return (s->m_rmax);
  }

  template <typename S>
  static decltype(auto) vmaxBl_internal(S *s) {
    return (s->m_vmaxBL);
  }

  template <typename S>
  static decltype(auto) hollandB_internal(S *s) {
    return (s->m_hollandB);
  }

  template <typename S>
  static decltype(auto) phi_internal(S *s) {
    return (s->m_phi);
  }

  template <typename S>
  static decltype(auto) quadrantVr_internal(S *s) {
    return (s->m_quadrantVr);
  }

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

  decltype(auto) quadFlag() { return quadFlag_internal(this); }

  decltype(auto) quadFlag() const { return quadFlag_internal(this); }

  decltype(auto) isotachRadius() { return isotachRadius_internal(this); }

  decltype(auto) isotachRadius() const { return isotachRadius_internal(this); }

  decltype(auto) isotachRadiusNullInInput() {
    return isotachRadiusNullInInput_internal(this);
  }

  decltype(auto) isotachRadiusNullInInput() const {
    return isotachRadiusNullInInput_internal(this);
  }

  decltype(auto) rmax() { return rmax_internal(this); }

  decltype(auto) rmax() const { return rmax_internal(this); }

  decltype(auto) vmaxBl() { return vmaxBl_internal(this); }

  decltype(auto) vmaxBl() const { return vmaxBl_internal(this); }

  decltype(auto) hollandB() { return hollandB_internal(this); }

  decltype(auto) hollandB() const { return hollandB_internal(this); }

  decltype(auto) phi() { return phi_internal(this); }

  decltype(auto) phi() const { return phi_internal(this); }

  decltype(auto) quadrantVr() { return quadrantVr_internal(this); }

  decltype(auto) quadrantVr() const { return quadrantVr_internal(this); }

 private:
  double m_windSpeed;
  Gahm::CircularArray<double, 4> m_isotachRadius;
  Gahm::CircularArray<double, 4> m_rmax;
  Gahm::CircularArray<double, 4> m_hollandB;
  Gahm::CircularArray<double, 4> m_vmaxBL;
  Gahm::CircularArray<double, 4> m_phi;
  Gahm::CircularArray<double, 4> m_quadrantVr;
  Gahm::CircularArray<bool, 4> m_quadFlag;
  Gahm::CircularArray<bool, 4> m_isotachRadiusNullInInput;
  Gahm::Isotach::RadiusCode m_radiusCode;
};
}  // namespace Gahm
std::ostream &operator<<(std::ostream &os, const Gahm::Isotach &iso);

#endif  // ISOTACH_H
