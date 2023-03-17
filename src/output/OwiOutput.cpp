// GNU General Public License v3.0
//
// This file is part of the GAHM model (https://github.com/adcirc/gahm).
// Copyright (c) 2023 ADCIRC Development Group.
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, version 3.
//
// This program is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program. If not, see <http://www.gnu.org/licenses/>.
//
// Author: Zach Cobell
// Contact: zcobell@thewaterinstitute.org
//

#include "OwiOutput.h"

#include <cassert>

#include "fmt/compile.h"
#include "fmt/core.h"

namespace Gahm::Output {

OwiOutput::OwiOutput(const Gahm::Datatypes::Date &start_date,
                     const Gahm::Datatypes::Date &end_date,
                     const std::string &filename,
                     const Gahm::Datatypes::WindGrid &wind_grid)
    : OutputFile(start_date, end_date, filename, wind_grid) {}

OwiOutput::~OwiOutput() { this->close_files(); }

void OwiOutput::open() {
  auto pressure_filename = this->filename() + ".pre";
  auto wind_filename = this->filename() + ".wnd";
  m_pressure_file = std::make_unique<std::ofstream>(pressure_filename);
  m_wind_file = std::make_unique<std::ofstream>(wind_filename);

  this->writeHeader(m_pressure_file.get());
  this->writeHeader(m_wind_file.get());
}

void OwiOutput::close() { this->close_files(); }

void OwiOutput::close_files() {
  if (m_pressure_file) {
    if (m_pressure_file->is_open()) m_pressure_file->close();
  }
  if (m_wind_file) {
    if (m_wind_file->is_open()) m_wind_file->close();
  }
}

void OwiOutput::writeHeader(std::ofstream *file) const {
  auto header_line = fmt::format(
      FMT_COMPILE("Oceanweather WIN/PRE Format                         "
                  "   {:04d}{:02d}{:02d}{:02d}     {:04d}{:02d}{:02d}{:02d}\n"),
      start_date().year(), start_date().month(), start_date().day(),
      start_date().hour(), end_date().year(), end_date().month(),
      end_date().day(), end_date().hour());
  *file << header_line;
}

void OwiOutput::write(const Datatypes::Date &date,
                      Datatypes::VortexSolution &solution) {
  if (!m_pressure_file->is_open()) {
    throw std::runtime_error(
        "Please call owi->open() before attempting to write data to files.");
  }

  auto record_header = this->generateRecordHeader(date);
  *m_pressure_file << record_header;
  *m_wind_file << record_header;

  this->write_record(m_pressure_file.get(), solution.p());
  this->write_record(m_wind_file.get(), solution.u());
  this->write_record(m_wind_file.get(), solution.v());
}

std::string OwiOutput::generateRecordHeader(const Datatypes::Date &date) const {
  auto lon_string = OwiOutput::formatHeaderCoordinates(windGrid().xll());
  auto lat_string = OwiOutput::formatHeaderCoordinates(windGrid().yll());
  return fmt::format(
      FMT_COMPILE(
          "iLat={:4d}iLong={:4d}DX={:6.4f}DY={:6.4f}SWLat={:8s}SWLon={:8s}DT="
          "{:04d}{:02d}{:02d}{:02d}{:02d}\n"),
      windGrid().ny(), windGrid().nx(), windGrid().dy(), windGrid().dx(),
      lat_string, lon_string, date.year(), date.month(), date.day(),
      date.hour(), date.minute());
}

std::string OwiOutput::formatHeaderCoordinates(const double value) {
  if (value <= -100.0) {
    return fmt::format("{:8.3f}", value);
  } else if (value < 0.0 || value >= 100.0) {
    return fmt::format(FMT_COMPILE("{:8.4f}"), value);
  } else {
    return fmt::format(FMT_COMPILE("{:8.5f}"), value);
  }
}

void OwiOutput::write_record(std::ostream *stream,
                             const std::vector<double> &value) const {
  assert(value.size() == windGrid().nx() * windGrid().ny());
  constexpr size_t num_records_per_line = 8;
  size_t n = 0;
  for (size_t j = 0; j < this->windGrid().ny(); ++j) {
    for (size_t i = 0; i < this->windGrid().nx(); ++i) {
      *stream << fmt::format(FMT_COMPILE("{:10.4f}"),
                             value[i + j * this->windGrid().nx()]);
      n++;
      if (n == num_records_per_line) {
        *(stream) << "\n";
        n = 0;
      }
    }
  }
  if (n != 0) *(stream) << "\n";
}

}  // namespace Gahm::Output