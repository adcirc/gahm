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
#ifndef GAHM_SRC_GAHMSOLUTIONPOINT_H_
#define GAHM_SRC_GAHMSOLUTIONPOINT_H_

#include "Constants.h"

class GahmSolutionPoint {
 public:
  GahmSolutionPoint()
      : m_u(0.0), m_v(0.0), m_p(Constants::backgroundPressure()) {}

  GahmSolutionPoint(double u, double v, double p) : m_u(u), m_v(v), m_p(p) {}

  double u() const { return m_u; }
  void setU(double u) { m_u = u; }

  double v() const { return m_v; }
  void setV(double v) { m_v = v; }

  double p() const { return m_p; }
  void setP(double p) { m_p = p; }

 private:
  double m_u;
  double m_v;
  double m_p;
};

#endif  // GAHM_SRC_GAHMSOLUTIONPOINT_H_
