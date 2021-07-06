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
#include "StormParameters.h"

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
  Atcf(std::string filename, Assumptions *a);

  std::string filename() const;
  void setFilename(const std::string &filename);

  int read();

  size_t nRecords() const;
  const AtcfLine *crecord(size_t index) const;
  AtcfLine *record(size_t index);

  std::vector<AtcfLine> *data();

  StormParameters getStormParameters(const Date &d) const;

  std::pair<int, double> getCycleNumber(const Date &d) const;

  enum AtcfFileTypes { FormatAtcf, FormatNWS20 };
  void write(const std::string &filename, AtcfFileTypes = FormatNWS20) const;

 private:
  /// Filename of the Atcf file to use
  std::string m_filename;

  /// Vector of AtcfLine data representing individual time points
  std::vector<AtcfLine> m_atcfData;

  Assumptions *m_assumptions;
};

#endif  // ATCF_H
