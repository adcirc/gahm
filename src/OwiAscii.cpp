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
#include "OwiAscii.h"

#include <cassert>

using namespace Gahm;

OwiAscii::OwiAscii(const Date& startDate, const Date& endDate,
                   const unsigned time_step)
    : m_startdate(startDate), m_enddate(endDate), m_timestep(time_step) {}

int OwiAscii::addDomain(const WindGrid& w, const std::string& pressureFile,
                        const std::string& windFile) {
  m_domains.push_back(std::make_unique<OwiAsciiDomain>(
      &w, m_startdate, m_enddate, m_timestep, pressureFile, windFile));
  return 0;
}

int OwiAscii::write(const Date& date, const size_t domain_index,
                    const std::vector<double>& pressure,
                    const std::vector<double>& wind_u,
                    const std::vector<double>& wind_v) {
  assert(domain_index < m_domains.size());
  return m_domains[domain_index]->write(date, pressure, wind_u, wind_v);
}

int OwiAscii::write(const Date& date, const size_t domain_index,
                    const WindData& data) {
  return this->write(date, domain_index, data.p(), data.u(), data.v());
}
