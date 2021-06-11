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

class Isotach {
 public:
  enum RadiusCode { AAA, NEQ, SEQ, SWQ, NWQ, NONE };

  Isotach();

  Isotach(RadiusCode code, double windSpeed, double r1, double r2, double r3,
          double r4);

  double windSpeed() const;
  void setWindSpeed(double windSpeed);

  RadiusCode code() const;
  void setCode(Isotach::RadiusCode code);

  void generateQuadFlag();

  static bool isNull(const Isotach &iso);

  static Isotach::RadiusCode codeFromString(const std::string &code);
  static std::string stringFromCode(Isotach::RadiusCode code);

  friend std::ostream &operator<<(std::ostream &os, const Isotach &iso);

  RadiusCode radiusCode() const;
  void setRadiusCode(const RadiusCode &radiusCode);

  CircularArray<bool, 4> *quadFlag();
  CircularArray<double, 4> *isotachRadius();
  CircularArray<bool, 4> *isotachRadiusNullInInput();
  CircularArray<double, 4> *rmax();
  CircularArray<double, 4> *vmaxBl();
  CircularArray<double, 4> *hollandB();
  CircularArray<double, 4> *phi();
  CircularArray<double, 4> *quadrantVr();

  const CircularArray<bool, 4> *cquadFlag() const;
  const CircularArray<double, 4> *cisotachRadius() const;
  const CircularArray<bool, 4> *cisotachRadiusNullInInput() const;
  const CircularArray<double, 4> *crmax() const;
  const CircularArray<double, 4> *cvmaxBl() const;
  const CircularArray<double, 4> *chollandB() const;
  const CircularArray<double, 4> *cphi() const;
  const CircularArray<double, 4> *cquadrantVr() const;

 private:
  double m_windSpeed;
  CircularArray<double, 4> m_isotachRadius;
  CircularArray<bool, 4> m_isotachRadiusNullInInput;
  CircularArray<double, 4> m_rmax;
  CircularArray<double, 4> m_hollandB;
  CircularArray<double, 4> m_vmaxBL;
  CircularArray<bool, 4> m_quadFlag;
  CircularArray<double, 4> m_quadrantVr;
  CircularArray<double, 4> m_phi;
  RadiusCode m_radiusCode;
};

std::ostream &operator<<(std::ostream &os, const Isotach &iso);

#endif  // ISOTACH_H
