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

#ifndef GAHM_OWIOUTPUT_H
#define GAHM_OWIOUTPUT_H

#include <fstream>
#include <memory>

#include "output/OutputFile.h"

namespace Gahm::Output {

class OwiOutput : public OutputFile {
 public:
  OwiOutput(const Gahm::Datatypes::Date &start_date,
            const Gahm::Datatypes::Date &end_date, const std::string &filename,
            const Gahm::Datatypes::WindGrid &wind_grid);

  ~OwiOutput() override;

  void open() override;

  void close() override;

  void write(const Datatypes::Date &date,
             Gahm::Datatypes::VortexSolution &solution) override;

 private:
  void writeHeader(std::ofstream *file) const;

  [[nodiscard]] std::string generateRecordHeader(
      const Datatypes::Date &date) const;

  static std::string formatHeaderCoordinates(double value);

  void write_record(std::ostream *stream,
                    const std::vector<double> &value) const;

  void close_files();

  std::unique_ptr<std::ofstream> m_pressure_file;
  std::unique_ptr<std::ofstream> m_wind_file;
};
}  // namespace Gahm::Output

#endif  // GAHM_OWIOUTPUT_H
