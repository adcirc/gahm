// MIT License
//
// Copyright (c) 2023 ADCIRC Development Group
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
#include "AtcfFile.h"

#include <algorithm>
#include <fstream>
#include <memory>
#include <utility>

#include "physical/Earth.h"

namespace Gahm::Atcf {

/*
 * Constructor
 * @param filename Name of the ATCF file
 */
AtcfFile::AtcfFile(std::string filename) : m_filename(std::move(filename)) {}

/*
 * Reads the ATCF file
 */
void AtcfFile::read() {
  auto f = std::make_unique<std::ifstream>(m_filename);
  if (!f->is_open()) {
    throw std::runtime_error("Unable to open file: " + m_filename);
  }

  std::string line;
  while (std::getline(*(f.get()), line)) {
    if (line.empty()) {
      continue;
    }
    auto snap = AtcfSnap::parseAtcfSnap(line);
    if (snap.has_value()) {
      if (snap->isValid()) {
        this->addAtcfSnap(snap.value());
      }
    } else {
      std::cerr << "[WARNING]: Invalid ATCF snap: " << snap->date().toString()
                << std::endl;
    }
  }
  m_isRead = true;
}

/*
 * Adds a snap to the ATCF file
 * @param snap Snap to add
 */
void AtcfFile::addAtcfSnap(const AtcfSnap& snap) {
  const auto it =
      std::find_if(m_atcfSnaps.begin(), m_atcfSnaps.end(),
                   [&](const auto& val) { return val.date() == snap.date(); });
  if (it == m_atcfSnaps.end()) {
    m_atcfSnaps.push_back(snap);
  } else {
    for (const auto& iso : snap.getIsotachs()) {
      it->addIsotach(iso);
    }
  }
}

/*
 * Returns the number of snaps in the ATCF file
 * @return Number of snaps
 */
size_t AtcfFile::size() const { return m_atcfSnaps.size(); }

/*
 * Writes the ATCF file to disk
 * @param filename Name of the file to write
 * @param format Format of the ATCF file
 */
void AtcfFile::write(const std::string& filename) {
  auto f = std::make_unique<std::ofstream>(filename);
  if (!f->is_open()) {
    throw std::runtime_error("Unable to open file: " + filename);
  }
  size_t cycle = 0;
  for (const auto& snap : m_atcfSnaps) {
    cycle += 1;
    for (size_t i = 0; i < snap.numberOfIsotachs(); ++i) {
      *(f.get()) << snap.to_string(cycle, m_atcfSnaps[0].date(), i) << "\n";
    }
  }
  f->close();
}



}  // namespace Gahm::Atcf