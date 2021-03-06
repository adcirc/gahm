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
#include "OwiAsciiDomain.h"

#include <cassert>
#include <utility>

#include "Logging.h"

#define FMT_HEADER_ONLY
#include "fmt/core.h"

using namespace Gahm;

OwiAsciiDomain::OwiAsciiDomain(const WindGrid *grid, const Date &startDate,
                               const Date &endDate,
                               const unsigned int time_step,
                               std::string pressureFile, std::string windFile)
    : m_isOpen(true),
      m_windGrid(grid),
      m_startDate(startDate),
      m_endDate(endDate),
      m_previousDate(startDate - time_step),
      m_timestep(time_step),
      m_pressureFile(std::move(pressureFile)),
      m_windFile(std::move(windFile)) {
  m_file_pressure = fopen(m_pressureFile.c_str(),"w");
  m_file_wind = fopen(m_windFile.c_str(),"w");
  this->write_header();
  assert(startDate < endDate);
}

OwiAsciiDomain::~OwiAsciiDomain() {
  if(m_file_pressure) fclose(m_file_pressure);
  if(m_file_wind) fclose(m_file_wind);
}

std::string OwiAsciiDomain::pressureFile() const { return m_pressureFile; }

std::string OwiAsciiDomain::windFile() const { return m_windFile; }

int OwiAsciiDomain::write(const Date &date, const std::vector<double> &pressure,
                          const std::vector<double> &wind_u,
                          const std::vector<double> &wind_v) {
  if (!m_isOpen) {
    gahm_throw_exception("OWI Domain not open");
  }
  if (date != m_previousDate + m_timestep) {
    gahm_throw_exception("Non-constant time spacing detected");
  }
  if (date > m_endDate) {
    gahm_throw_exception("Attempt to write past file end date");
  }

  auto recordHeader = generateRecordHeader(date, m_windGrid);
  fputs( recordHeader.c_str(), m_file_pressure);
  fputs( recordHeader.c_str(), m_file_wind);

  OwiAsciiDomain::write_record(m_file_pressure, pressure);
  OwiAsciiDomain::write_record(m_file_wind, wind_u);
  OwiAsciiDomain::write_record(m_file_wind, wind_v);

  m_previousDate = date;

  return 0;
}

void OwiAsciiDomain::write_header() {
  auto header = generateHeaderLine(m_startDate, m_endDate);
  fputs(header.c_str(), m_file_pressure);
  fputs(header.c_str(), m_file_wind);
}

std::string OwiAsciiDomain::generateHeaderLine(const Date &date1,
                                               const Date &date2) {
  return fmt::format(
      "Oceanweather WIN/PRE Format                         "
      "   {:4d}{:02d}{:02d}{:02d}     {:4d}{:02d}{:02d}{:02d}\n",
      date1.year(), date1.month(), date1.day(), date1.hour(), date2.year(),
      date2.month(), date2.day(), date2.hour());
}

std::string OwiAsciiDomain::generateRecordHeader(const Date &date,
                                                 const WindGrid *grid) {
  return fmt::format(
      "iLat={:4d}iLong={:4d}DX={:6.4f}DY={:6.4f}SWLat={:8.5f}SWLon={:8.4f}DT={:"
      "04d}{:02d}{:02d}{:02d}{:02d}\n",
      grid->nj(), grid->ni(), grid->dy(), grid->dx(), grid->bottom_left().y(),
      grid->bottom_left().x(), date.year(), date.month(), date.day(),
      date.hour(), date.minute());
}

void OwiAsciiDomain::write_record(FILE *stream,
                                  const std::vector<double> &value) {
  constexpr size_t num_records_per_line = 8;
  size_t n = 0;
  for (const auto &v : value) {
    fmt::print(stream, " {:9.4f}", v);
    n++;
    if (n == num_records_per_line) {
      fputs("\n", stream);
      n = 0;
    }
  }
  if(n!=0)fputs( "\n", stream);
}
