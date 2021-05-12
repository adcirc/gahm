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
#ifndef GAHM_H
#define GAHM_H

#include <array>
#include <memory>

#include "Assumptions.h"
#include "Atcf.h"
#include "Preprocessor.h"

class Gahm {
 public:
  explicit Gahm(std::string filename);

  std::string filename() const;

  int read();

  int get(const Date &d, const std::vector<double> &x,
          const std::vector<double> &y, std::vector<double> &u,
          std::vector<double> &v, std::vector<double> &p);

 private:
  struct uvp {
    double u;
    double v;
    double p;
    uvp(double u = 0.0, double v = 0.0, double p = 1013.0) : u(u), v(v), p(p) {}
  };

  template <bool geofactor>
  static uvp getUvpr(double distance, double angle, double rmax,
                     double rmax_true, double p_c, double p_background,
                     double b, double vmax, double pmin, double phi,
                     double utrans, double coriolis, double vtrans,
                     double clat);

  const std::string m_filename;
  Assumptions m_assumptions;
  std::unique_ptr<Atcf> m_atcf;
  std::unique_ptr<Preprocessor> m_preprocessor;
};

template <>
Gahm::uvp Gahm::getUvpr<true>(double distance, double angle, double rmax,
                              double rmax_true, double p_c, double p_background,
                              double b, double vmax, double pmin, double phi,
                              double utrans, double coriolis, double vtrans,
                              double clat);
template <>
Gahm::uvp Gahm::getUvpr<false>(double distance, double angle, double rmax,
                               double rmax_true, double p_c,
                               double p_background, double b, double vmax,
                               double pmin, double phi, double utrans,
                               double coriolis, double vtrans, double clat);

#endif  // GAHM_H
