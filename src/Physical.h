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
#ifndef PHYSICAL_H
#define PHYSICAL_H

class Physical {
 public:
  static constexpr double rotation_earth() { return 3600.0 * 7.2921 * 10e-5; }

  static constexpr double km2nmi() { return 1.852; }
  static constexpr double nmi2km() { return 1.0 / km2nmi(); }

  static constexpr double ms2mph() { return 2.23694; }
  static constexpr double kt2mph() { return 1.15078; }
  static constexpr double mph2kt() { return 1.0 / kt2mph(); }
  static constexpr double mph2ms() { return 1.0 / ms2mph(); }
  static constexpr double ms2kt() { return 1.94394; }
  static constexpr double kt2ms() { return 1.0 / ms2kt(); }

  static constexpr double backgroundPressure() { return 1013.0; }
  static constexpr double windReduction() { return 0.9; }
  static constexpr double rhoAir() { return 1.15; }
};

#endif  // PHYSICAL_H
