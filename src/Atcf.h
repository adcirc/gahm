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
#ifndef ATCF_H
#define ATCF_H

#include <string>
#include <vector>

#include "Assumptions.h"
#include "AtcfLine.h"
#include "HurricanePressure.h"

class Atcf {
 public:
  Atcf();
  Atcf(const std::string &filename, Assumptions *assumptions = nullptr);

  std::string filename() const;
  void setFilename(const std::string &filename);

  int read();

  int calculateRmax();

  size_t nRecords() const;
  const AtcfLine *record(size_t index) const;
  AtcfLine record(size_t index);

  struct StormParameters {
    int cycle;
    double wtratio;
    double latitude;
    double longitude;
    double central_pressure;
    double background_pressure;
    double vmax;
    double utrans;
    double vtrans;
    double uvtrans;
  };

  StormParameters getStormParameters(const Date &d) const;

 private:
  std::string m_filename;

  std::vector<AtcfLine> m_atcfData;

  Assumptions *m_assumptions;

  int generateMissingPressureData(
      const HurricanePressure::PressureMethod &method =
          HurricanePressure::KNAFFZEHR);

  static int uvTrans(const AtcfLine &d1, const AtcfLine &d2, double &uv,
                     double &vv, double &uuvv);
  static inline double linearInterp(double weight, double v1, double v2);

  int calculateOverlandTranslationVelocity();
  std::pair<int, double> getCycleNumber(const Date &d) const;
};

#endif  // ATCF_H
