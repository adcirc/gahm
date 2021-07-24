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

#ifndef METGET_LIBRARY_WINDDATA_H_
#define METGET_LIBRARY_WINDDATA_H_

#include <array>
#include <cstdlib>
#include <vector>

#include "StormParameters.h"

namespace Gahm {
class WindData {
 public:
  WindData();

  explicit WindData(size_t n);

  WindData(size_t n, double u, double v, double p);

  const std::vector<double> &u() const;
  const std::vector<double> &v() const;
  const std::vector<double> &p() const;
  size_t size() const;

  void set(size_t index, double u, double v, double p);

  void setU(size_t index, double value);
  void setV(size_t index, double value);
  void setP(size_t index, double value);

  void setU(const std::vector<double> &u);
  void setV(const std::vector<double> &v);
  void setP(const std::vector<double> &p);

  void setStormParameters(const StormParameters &sp);
  const StormParameters *stormParameters() const;

  void setSize(size_t n);

 private:
  size_t m_n;
  std::vector<double> m_u;
  std::vector<double> m_v;
  std::vector<double> m_p;
  StormParameters m_stormParameters;
};
}  // namespace Gahm
#endif  // METGET_LIBRARY_WINDDATA_H_
