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
#ifndef GAHMVORTEX_H
#define GAHMVORTEX_H

#include <array>
#include <memory>
#include <vector>

#include "Atcf.h"
#include "GahmState.h"
#include "Preprocessor.h"
#include "StormParameters.h"
#include "Vortex.h"
#include "WindData.h"

class GahmVortex {
 public:
  GahmVortex(std::string filename, const std::vector<double> &x,
             const std::vector<double> &y);

  std::string filename() const;

  WindData get(const Date &d);

  Assumptions *assumptions();

  Atcf *atcf();

 private:
  struct uvp {
    double u;
    double v;
    double p;
  };

  const std::string m_filename;
  std::unique_ptr<Assumptions> m_assumptions;
  std::unique_ptr<Atcf> m_atcf;
  std::unique_ptr<Preprocessor> m_preprocessor;
  std::unique_ptr<GahmState> m_state;

  static uvp getUvpr(double distance, double angle,
                     const ParameterPack &pack, double utrans,
                     double vtrans, const StormParameters &s);

  ParameterPack generateStormParameterPackForLocation(
      const StormParameters &sp, const Vortex &v1, const Vortex &v2,
      int i) const;

  static constexpr double computePhi(const ParameterPack &p,
                                     double corio);
};

#endif  // GAHM_H
