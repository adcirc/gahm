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
#include "FastMath.h"

using namespace Gahm;

float FastMath::float_fastExp(float x) noexcept {
  constexpr auto v0 = double((1 << 20) / M_LN2);
  constexpr auto v1 = double((1 << 20) * 1023 - 0);
  union union_exp {
    double d_;
    int32_t i_[2];
    explicit union_exp(int32_t v) : i_{0, v} {}
  };
  union_exp uu(v0 * x + v1);
  return uu.d_;
}

double FastMath::double_fastExp(double x) noexcept {
  constexpr auto v0 = double((int64_t(1) << 52) / M_LN2);
  constexpr auto v1 = double((int64_t(1) << 52) * 1023 - 0);
  union union_exp {
    double d_;
    int64_t i_;
    explicit union_exp(int64_t v) : i_{v} {}
  };
  union_exp uu(v0 * x + v1);
  return static_cast<float>(uu.d_);
}

float FastMath::float_fastInverseSquareRoot(const float number) noexcept {
  // The Quake algorithm
  const float x2 = number * 0.5F;
  constexpr float three_half = 1.5F;

  union {
    float f;
    uint32_t i;
  } conv = {.f = number};
  conv.i = 0x5f3759df - (conv.i >> 1);
  conv.f *= three_half - (x2 * conv.f * conv.f);
  return conv.f;
}

double FastMath::double_fastInverseSquareRoot(double number) noexcept {
  // The Quake algorithm, but for doubles
  double x2 = number * 0.5;
  std::int64_t i = *(std::int64_t *)&number;
  // The magic number is for doubles is from
  // https://cs.uwaterloo.ca/~m32rober/rsqrt.pdf
  i = 0x5fe6eb50c7b537a9 - (i >> 1);
  number = *(double *)&i;
  number = number * (1.5 - (x2 * number * number));  // 1st iteration
  //      y  = y * ( 1.5 - ( x2 * y * y ) );   // 2nd iteration, this can be
  //      removed
  return number;
}
