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

#ifdef SWIG
#define NODISCARD
#else
#define NODISCARD [[nodiscard]]
#endif

namespace Gahm::Atcf {

class AtcfFile {
 public:
  AtcfFile() = default;

  explicit AtcfFile(std::string filename, bool quiet = false);

  void read();

  NODISCARD size_t size() const;

  std::vector<Gahm::Atcf::AtcfSnap>& data() { return m_atcfSnaps; }

  NODISCARD const std::vector<Gahm::Atcf::AtcfSnap>& data() const {
    return m_atcfSnaps;
  }

  NODISCARD bool empty() const { return m_atcfSnaps.empty(); }

  NODISCARD Atcf::AtcfSnap& at(size_t index) { return m_atcfSnaps.at(index); }
  NODISCARD const Atcf::AtcfSnap& at(size_t index) const {
    return m_atcfSnaps.at(index);
  }

#ifndef SWIG

  NODISCARD Atcf::AtcfSnap& front() { return m_atcfSnaps.front(); }
  NODISCARD const Atcf::AtcfSnap& front() const { return m_atcfSnaps.front(); }

  NODISCARD Atcf::AtcfSnap& back() { return m_atcfSnaps.back(); }
  NODISCARD const Atcf::AtcfSnap& back() const { return m_atcfSnaps.back(); }

  //...Iterators
  NODISCARD auto begin() { return m_atcfSnaps.begin(); }
  NODISCARD auto begin() const { return m_atcfSnaps.begin(); }
  NODISCARD auto cbegin() const { return m_atcfSnaps.cbegin(); }

  NODISCARD auto end() { return m_atcfSnaps.end(); }
  NODISCARD auto end() const { return m_atcfSnaps.end(); }
  NODISCARD auto cend() const { return m_atcfSnaps.cend(); }

  //...Find
  auto find(const Gahm::Datatypes::Date& date) {
    return std::find_if(m_atcfSnaps.begin(), m_atcfSnaps.end(),
                        [&](const auto& val) { return val.date() == date; });
  }

  //...Indexing
  Gahm::Atcf::AtcfSnap& operator[](size_t index) { return m_atcfSnaps[index]; }
  const Gahm::Atcf::AtcfSnap& operator[](size_t index) const {
    return m_atcfSnaps[index];
  }
#endif

  void write(const std::string& filename);

  NODISCARD std::string filename() const { return m_filename; }

  void addAtcfSnap(const Gahm::Atcf::AtcfSnap& snap);

 private:
  std::string m_filename;
  bool m_quiet;
  std::vector<Gahm::Atcf::AtcfSnap> m_atcfSnaps;
};

}  // namespace Gahm::Atcf
#endif  // GAHM_SRC_ATCF_ATCFFILE_H_
