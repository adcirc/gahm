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
#ifndef GAHM_SRC_ATCF_ATCFFILE_H_
#define GAHM_SRC_ATCF_ATCFFILE_H_

#include <algorithm>
#include <string>
#include <vector>

#include "atcf/AtcfSnap.h"

namespace Gahm::Atcf {

class AtcfFile {
 public:
  AtcfFile() = default;

  explicit AtcfFile(std::string filename, bool quiet = false);

  void read();

  [[nodiscard]] size_t size() const;

  //...Indexing
  Gahm::Atcf::AtcfSnap& operator[](size_t index) { return m_atcfSnaps[index]; }
  const Gahm::Atcf::AtcfSnap& operator[](size_t index) const {
    return m_atcfSnaps[index];
  }

  std::vector<Gahm::Atcf::AtcfSnap>& data() { return m_atcfSnaps; }

  [[nodiscard]] const std::vector<Gahm::Atcf::AtcfSnap>& data() const {
    return m_atcfSnaps;
  }

  //...Lookup
#ifndef SWIG
  auto find(const Gahm::Datatypes::Date& date) {
    return std::find_if(m_atcfSnaps.begin(), m_atcfSnaps.end(),
                        [&](const auto& val) { return val.date() == date; });
  }
#endif

  void write(const std::string& filename);

  [[nodiscard]] std::string filename() const { return m_filename; }

  void addAtcfSnap(const Gahm::Atcf::AtcfSnap& snap);

 private:
  std::string m_filename;
  bool m_quiet;
  std::vector<Gahm::Atcf::AtcfSnap> m_atcfSnaps;
};

}  // namespace Gahm::Atcf
#endif  // GAHM_SRC_ATCF_ATCFFILE_H_
