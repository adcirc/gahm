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

/**
 * @class Atcf
 * @author Zachary Cobell
 * @brief Class that handles operations on Atcf format files
 * @copyright Copyright 2021 ADCIRC Development Group. All Rights Reserved. This
 * project is released under the terms of the MIT License
 *
 * This class handles the reading and manipulation of Atcf format data to
 * describe hurricane track data
 *
 */
class Atcf {
 public:
  Atcf(std::string filename, Assumptions *assumptions);

  std::string filename() const;
  void setFilename(const std::string &filename);

  int read();

  int calculateRadii();

  size_t nRecords() const;
  const AtcfLine *crecord(size_t index) const;
  AtcfLine *record(size_t index);

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

  enum AtcfFileTypes { FormatAtcf, FormatNWS20 };
  void write(const std::string &filename, AtcfFileTypes = FormatNWS20) const;

 private:
  /// Filename of the Atcf file to use
  std::string m_filename;

  /// Vector of AtcfLine data representing individual time points
  std::vector<AtcfLine> m_atcfData;

  /// Assumptions that are made within this code for diagnostic analysis later
  Assumptions *m_assumptions;

  int computeParameters();

  int generateMissingPressureData(const HurricanePressure::PressureMethod
                                      &method = HurricanePressure::KNAFFZEHR);

  void setAllRadiiToRmax(CircularArray<double, 4> *radii,
                         CircularArray<bool, 4> *quadFlag, double rmax,
                         size_t record, size_t isotach);

  void setMissingRadiiToHalfNonzeroRadii(CircularArray<double, 4> *radii,
                                         double radiisum, size_t record,
                                         size_t isotach);

  void setMissingRadiiToHalfOfAverageSpecifiedRadii(
      CircularArray<double, 4> *radii, double radiisum, size_t record,
      size_t isotach);

  void setMissingRadiiToAverageOfAdjacentRadii(CircularArray<double, 4> *radii,
                                               size_t record, size_t isotach);

  static int uvTrans(const AtcfLine &d1, const AtcfLine &d2, double &uv,
                     double &vv, double &uuvv);
  static inline double linearInterp(double weight, double v1, double v2);

  int calculateOverlandTranslationVelocity();
  std::pair<int, double> getCycleNumber(const Date &d) const;
};

#endif  // ATCF_H
