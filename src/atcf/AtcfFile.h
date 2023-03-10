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

  explicit AtcfFile(std::string filename);

  void read();

  [[nodiscard]] size_t size() const;

  //...Indexing
  AtcfSnap& operator[](size_t index) { return m_atcfSnaps[index]; }
  const AtcfSnap& operator[](size_t index) const { return m_atcfSnaps[index]; }

  //...Range-based for
  [[nodiscard]] auto begin() const { return m_atcfSnaps.begin(); }
  [[nodiscard]] auto end() const { return m_atcfSnaps.end(); }
  [[nodiscard]] auto rbegin() const { return m_atcfSnaps.rbegin(); }
  [[nodiscard]] auto rend() const { return m_atcfSnaps.rend(); }
  auto begin() { return m_atcfSnaps.begin(); }
  auto end() { return m_atcfSnaps.end(); }
  auto rbegin() { return m_atcfSnaps.rbegin(); }
  auto rend() { return m_atcfSnaps.rend(); }

  //...Lookup
  auto find(const Gahm::Datatypes::Date& date) {
    return std::find_if(m_atcfSnaps.begin(), m_atcfSnaps.end(),
                        [&](const auto& val) { return val.date() == date; });
  }

  [[nodiscard]] bool isRead() const { return m_isRead; }

  void write(const std::string& filename);

  [[nodiscard]] std::string filename() const { return m_filename; }

 private:
  void addAtcfSnap(const AtcfSnap& snap);


  std::string m_filename;
  bool m_isRead{false};
  std::vector<AtcfSnap> m_atcfSnaps;
};

}  // namespace Gahm::Atcf
#endif  // GAHM_SRC_ATCF_ATCFFILE_H_
