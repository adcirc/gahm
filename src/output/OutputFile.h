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
#ifndef GAHM_OUTPUTFILE_H
#define GAHM_OUTPUTFILE_H

#include <string>
#include <utility>
#include <vector>

#include "datatypes/Date.h"
#include "datatypes/VortexSolution.h"
#include "datatypes/WindGrid.h"

#ifdef SWIG
#define NODISCARD
#else
#define NODISCARD [[nodiscard]]
#endif

namespace Gahm::Output {

class OutputFile {
 public:
  OutputFile(const Gahm::Datatypes::Date &start_date,
             const Gahm::Datatypes::Date &end_date, std::string filename,
             const Gahm::Datatypes::WindGrid &wind_grid)
      : m_start_date(start_date),
        m_end_date(end_date),
        m_filename(std::move(filename)),
        m_wind_grid(wind_grid) {}

  virtual ~OutputFile() = default;

  virtual void open() = 0;

  virtual void close() = 0;

  virtual void write(const Datatypes::Date &date,
                     Gahm::Datatypes::VortexSolution &solution) = 0;

  NODISCARD std::string filename() const { return m_filename; }

  NODISCARD const Gahm::Datatypes::WindGrid &windGrid() const {
    return m_wind_grid;
  }

  NODISCARD Gahm::Datatypes::Date start_date() const { return m_start_date; }
  NODISCARD Gahm::Datatypes::Date end_date() const { return m_end_date; }

 private:
  Datatypes::Date m_start_date;
  Datatypes::Date m_end_date;
  Gahm::Datatypes::WindGrid m_wind_grid;
  std::string m_filename;
};

}  // namespace Gahm::Output
#endif  // GAHM_OUTPUTFILE_H
