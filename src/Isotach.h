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

  Gahm::CircularArray<bool, 4> *quadFlag();
  Gahm::CircularArray<double, 4> *isotachRadius();
  Gahm::CircularArray<bool, 4> *isotachRadiusNullInInput();
  Gahm::CircularArray<double, 4> *rmax();
  Gahm::CircularArray<double, 4> *vmaxBl();
  Gahm::CircularArray<double, 4> *hollandB();
  Gahm::CircularArray<double, 4> *phi();
  Gahm::CircularArray<double, 4> *quadrantVr();

  const Gahm::CircularArray<bool, 4> *quadFlag() const;
  const Gahm::CircularArray<double, 4> *isotachRadius() const;
  const Gahm::CircularArray<bool, 4> *isotachRadiusNullInInput() const;
  const Gahm::CircularArray<double, 4> *rmax() const;
  const Gahm::CircularArray<double, 4> *vmaxBl() const;
  const Gahm::CircularArray<double, 4> *hollandB() const;
  const Gahm::CircularArray<double, 4> *phi() const;
  const Gahm::CircularArray<double, 4> *quadrantVr() const;

 private:
  double m_windSpeed;
  Gahm::Isotach::RadiusCode m_radiusCode;
  Gahm::CircularArray<double, 4> m_isotachRadius;
  Gahm::CircularArray<double, 4> m_rmax;
  Gahm::CircularArray<double, 4> m_hollandB;
  Gahm::CircularArray<double, 4> m_vmaxBL;
  Gahm::CircularArray<double, 4> m_quadrantVr;
  Gahm::CircularArray<double, 4> m_phi;
  Gahm::CircularArray<bool, 4> m_quadFlag;
  Gahm::CircularArray<bool, 4> m_isotachRadiusNullInInput;
};
}  // namespace Gahm
std::ostream &operator<<(std::ostream &os, const Gahm::Isotach &iso);

#endif  // ISOTACH_H
