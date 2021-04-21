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
#include "atcf.h"
#include <fstream>

Atcf::Atcf() : m_filename("none") {}

Atcf::Atcf(const std::string &filename) : m_filename(filename) {}

std::string Atcf::filename() const { return m_filename; }

void Atcf::setFilename(const std::string &filename) { m_filename = filename; }

/**
 * @brief Reads the specified atcf file into a vector of AtcfLine objects
 * @return
 */
int Atcf::read() {
  std::ifstream f(m_filename);
  if (f.bad()) {
    return 1;
  }

  std::vector<AtcfLine> data;

  while (!f.eof()) {
    std::string line;
    std::getline(f, line);
    AtcfLine a = AtcfLine::parseAtcfLine(line);
    if (!a.isNull()) {
      data.push_back(a);
    }
  }
  f.close();

  std::sort(data.begin(), data.end());
  for (size_t i = data.size() - 1; i >= 1; --i) {
    if (AtcfLine::isSameForecastPeriod(data[i], data[i - 1])) {
      for (size_t j = 0; j < data[i].nIsotach(); ++j) {
        data[i - 1].addIsotach(*(data[i].cisotach(j)));
      }
      data.erase(data.begin() + i);
    }
  }

  for (const auto &aa : data) {
    std::cout << aa;
  }

  return 0;
}
