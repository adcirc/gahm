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

#include <memory>
#include <string>
#include <vector>

#include "Date.h"
#include "WindGrid.h"

namespace Gahm {
class OwiAsciiDomain {
 public:
  OwiAsciiDomain(const Gahm::WindGrid *grid, const Gahm::Date &startDate,
                 const Gahm::Date &endDate, unsigned time_step,
                 std::string pressureFile, std::string windFile);

  ~OwiAsciiDomain();

  int write(const Gahm::Date &date, const std::vector<double> &pressure,
            const std::vector<double> &wind_u,
            const std::vector<double> &wind_v);

  std::string pressureFile() const;

  std::string windFile() const;

 private:
  void write_header();

  static std::string generateHeaderLine(const Gahm::Date &date1,
                                        const Gahm::Date &date2);
  static std::string generateRecordHeader(const Gahm::Date &date,
                                          const Gahm::WindGrid *grid);
  static void write_record(FILE *stream, const std::vector<double> &value);

  bool m_isOpen;
  const Date m_startDate;
  const Date m_endDate;
  Date m_previousDate;
  const unsigned m_timestep;
  const Gahm::WindGrid *m_windGrid;
  const std::string m_pressureFile;
  const std::string m_windFile;
  FILE *m_file_pressure;
  FILE *m_file_wind;
};
}  // namespace Gahm
#endif  // METGET_LIBRARY_OWIASCIIDOMAIN_H_
