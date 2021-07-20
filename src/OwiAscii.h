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

#ifndef METGET_LIBRARY_OWIASCII_H_
#define METGET_LIBRARY_OWIASCII_H_

#include <string>

#include "OwiAsciiDomain.h"
#include "WindData.h"
#include "WindGrid.h"

class OwiAscii {
 public:
  OwiAscii(const Date &date_start, const Date &date_end, unsigned time_step);

  int addDomain(const WindGrid &w, const std::string &pressureFile,
                const std::string &windFile);

  int write(const Date &date, const size_t domain_index, const WindData &data);

  int write(const Date &date, const size_t domain_index,
            const std::vector<double> &pressure,
            const std::vector<double> &wind_u,
            const std::vector<double> &wind_v);

 private:
  const Date m_startdate;
  const Date m_enddate;
  const unsigned m_timestep;
  std::vector<std::unique_ptr<OwiAsciiDomain>> m_domains;
};

#endif  // METGET_LIBRARY_OWIASCII_H_
