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
#ifndef GAHM_SRC_FASTMATH_H_
#define GAHM_SRC_FASTMATH_H_

#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <type_traits>
#include "Logging.h"

namespace Gahm {
class FastMath {
 public:
  template <typename T>
  static T fast_exp(const T x) noexcept {
    if (std::is_same<T, float>()) {
      return FastMath::float_fastExp(x);
    } else if (std::is_same<T, double>()) {
      return FastMath::double_fastExp(x);
    } else {
      gahm_throw_exception("Invalid type");
      return T(0);
    }
  }

  template <typename T>
  static T fast_sqrt(const T x) noexcept {
    if (std::is_same<T, float>()) {
      return 1.0f / FastMath::float_fastInverseSquareRoot(x);
    } else if (std::is_same<T, double>()) {
      return 1.0 / FastMath::double_fastInverseSquareRoot(x);
    } else {
      gahm_throw_exception("Invalid type");
      return T(0);
    }
  }

  template<typename T>
  static constexpr T cos(T x) noexcept {
    constexpr T tp = 1./(2.*M_PI);
    constexpr T A = 0.25;
    constexpr T B = 16.0;
    constexpr T C = 0.5; 
    constexpr T D = 0.225;
    constexpr T E = 1.0;
    x *= tp;
    x -= A + std::floor(x + A);
    x *= B * (std::abs(x) - C);
    return x + D * x * (std::abs(x) - E);
  }

  template<typename T>
  static constexpr T sin(T x) noexcept {
    constexpr T B = 4.0 / M_PI;
    constexpr T C = -4.0 / (M_PI*M_PI);
    constexpr T Q = 0.775;
    constexpr T P = 0.225;
    T y = B * x + C * x * std::abs(x);
    return P * (y * abs(y) - y) + y;
  }

 private:
  static float float_fastExp(float number) noexcept;
  static double double_fastExp(double number) noexcept;
  static float float_fastInverseSquareRoot(const float number) noexcept;
  static double double_fastInverseSquareRoot(const double number) noexcept;
};
}  // namespace Gahm


// ifdefs to enable the different fast trancendental functions
#ifdef GAHM_USE_FASTMATH_SQRT
#define gahm_sqrt Gahm::FastMath::fast_sqrt
#else
#define gahm_sqrt std::sqrt
#endif

#ifdef GAHM_USE_FASTMATH_EXP
#define gahm_exp Gahm::FastMath::fast_exp
#else
#define gahm_exp std::exp
#endif

#ifdef GAHM_USE_FASTMATH_SINCOS
#define gahm_sin Gahm::FastMath::sin
#define gahm_cos Gahm::FastMath::cos
#else
#define gahm_sin std::sin
#define gahm_cos std::cos
#endif

#endif  // GAHM_SRC_FASTMATH_H_
