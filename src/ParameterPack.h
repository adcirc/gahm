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
#ifndef PARAMETERPACK_H
#define PARAMETERPACK_H

#include <array>
#include <iostream>

namespace Gahm {
class ParameterPack {
 public:
  constexpr ParameterPack(double vmaxbl, double rmax, double rmaxtrue, double b)
      : m_vmaxbl(vmaxbl), m_rmax(rmax), m_rmaxtrue(rmaxtrue), m_b(b) {}

  constexpr double vmaxBoundaryLayer() const { return m_vmaxbl; }
  constexpr double radiusToMaxWinds() const { return m_rmax; }
  constexpr double radiusToMaxWindsTrue() const { return m_rmaxtrue; }
  constexpr double hollandB() const { return m_b; }

  void printToScreen() const {
    std::cout << "   vmaxBoundaryLayer: " << vmaxBoundaryLayer() << std::endl;
    std::cout << "    radiusToMaxWinds: " << radiusToMaxWinds() << std::endl;
    std::cout << "radiusToMaxWindsTrue: " << radiusToMaxWindsTrue()
              << std::endl;
    std::cout << "            hollandB: " << hollandB() << std::endl;
  }

 private:
  const double m_vmaxbl;
  const double m_rmax;
  const double m_rmaxtrue;
  const double m_b;
};
}  // namespace Gahm
#endif  // PARAMETERPACK_H
