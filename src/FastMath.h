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
      return T(0);
    }
  }

 private:
  static float float_fastExp(float number) noexcept;
  static double double_fastExp(double number) noexcept;
  static float float_fastInverseSquareRoot(const float number) noexcept;
  static double double_fastInverseSquareRoot(const double number) noexcept;
};
}  // namespace Gahm

#ifdef GAHM_USE_FASTMATH
#define gahm_sqrt Gahm::FastMath::fast_sqrt
#define gahm_exp Gahm::FastMath::fast_exp
#else
#define gahm_sqrt std::sqrt
#define gahm_exp std::exp
#endif

#endif  // GAHM_SRC_FASTMATH_H_
