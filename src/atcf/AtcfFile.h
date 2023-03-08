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

 private:
  void addAtcfSnap(const AtcfSnap& snap);

  std::string m_filename;
  bool m_isRead{false};
  std::vector<AtcfSnap> m_atcfSnaps;
};

}  // namespace Gahm::Atcf
#endif  // GAHM_SRC_ATCF_ATCFFILE_H_
