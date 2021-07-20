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

#ifndef METGET_LIBRARY_OWIASCIIDOMAIN_H_
#define METGET_LIBRARY_OWIASCIIDOMAIN_H_

#include <fstream>
#include <memory>
#include <string>
#include <vector>

#include "Date.h"
#include "WindGrid.h"

class OwiAsciiDomain {
 public:
  OwiAsciiDomain(const WindGrid *grid, const Date &startDate,
                 const Date &endDate, unsigned time_step,
                 std::string pressureFile, std::string windFile);

  ~OwiAsciiDomain();

  int write(const Date &date, const std::vector<double> &pressure,
            const std::vector<double> &wind_u,
            const std::vector<double> &wind_v);

  void open();

  void close();

 private:
  void write_header();

  static std::string generateHeaderLine(const Date &date1, const Date &date2);
  static std::string generateRecordHeader(const Date &date,
                                          const WindGrid *grid);
  static void write_record(std::ofstream *stream,
                           const std::vector<double> &value);

  bool m_isOpen;
  const Date m_startDate;
  const Date m_endDate;
  Date m_previousDate;
  const unsigned m_timestep;
  std::unique_ptr<std::ofstream> m_ofstream_pressure;
  std::unique_ptr<std::ofstream> m_ofstream_wind;
  const WindGrid *m_windGrid;
  const std::string m_pressureFile;
  const std::string m_windFile;
};

#endif  // METGET_LIBRARY_OWIASCIIDOMAIN_H_
