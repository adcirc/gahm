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
#include "AtcfFile.h"

#include <algorithm>
#include <cstddef>
#include <fstream>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <utility>

#include "AtcfSnap.h"

namespace Gahm::Atcf {

/*
 * Constructor
 * @param filename Name of the ATCF file
 */
AtcfFile::AtcfFile(std::string filename, bool quiet)
    : m_filename(std::move(filename)), m_quiet(quiet) {}

/*
 * Reads the ATCF file
 */
void AtcfFile::read() {
  auto file_obj = std::make_unique<std::ifstream>(m_filename);
  if (!file_obj->is_open()) {
    throw std::runtime_error("Unable to open file: " + m_filename);
  }

  std::string line;
  while (std::getline(*(file_obj.get()), line)) {
    if (!line.empty()) {
      auto snap = AtcfSnap::parseAtcfSnap(line);
      if (snap.has_value()) {
        if (snap->isValid()) {
          this->addAtcfSnap(*snap);
        }
      } else {
        if (!m_quiet) {
          std::cerr << "[WARNING]: Invalid ATCF snap: "
                    << snap->date().toString() << '\n';
        }
      }
    }
  }
}

/*
 * Adds a snap to the ATCF file
 * @param snap Snap to add
 */
void AtcfFile::addAtcfSnap(const AtcfSnap& snap) {
  const auto iter =
      std::find_if(m_atcfSnaps.begin(), m_atcfSnaps.end(),
                   [&](const auto& val) { return val.date() == snap.date(); });
  if (iter == m_atcfSnaps.end()) {
    m_atcfSnaps.push_back(snap);
  } else {
    for (const auto& iso : snap.isotachs()) {
      iter->addIsotach(iso);
    }
  }
}

/*
 * Returns the number of snaps in the ATCF file
 * @return Number of snaps
 */
auto AtcfFile::size() const -> size_t { return m_atcfSnaps.size(); }

/*
 * Writes the ATCF file to disk
 * @param filename Name of the file to write
 * @param format Format of the ATCF file
 */
void AtcfFile::write(const std::string& filename) {
  auto file_obj = std::make_unique<std::ofstream>(filename);
  if (!file_obj->is_open()) {
    throw std::runtime_error("Unable to open file: " + filename);
  }
  size_t cycle = 0;
  for (const auto& snap : m_atcfSnaps) {
    cycle += 1;
    for (const auto& iso : snap.isotachs()) {
      *(file_obj.get()) << snap.to_string(cycle, m_atcfSnaps[0].date(), iso)
                        << "\n";
    }
  }
  file_obj->close();
}

}  // namespace Gahm::Atcf