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
#ifndef GAHM_SRC_PREPROCESSOR_PREPROCESSOR_H_
#define GAHM_SRC_PREPROCESSOR_PREPROCESSOR_H_

#include "atcf/AtcfFile.h"
#include "atcf/AtcfQuadrant.h"
#include "atcf/StormTranslation.h"

namespace Gahm {
class Preprocessor {
 public:
  explicit Preprocessor(Gahm::Atcf::AtcfFile *atcf);

  void prepareAtcfData();
  void solve();

 private:
  void fillMissingAtcfData();
  void computeStormTranslationVelocities();
  void computeBoundaryLayerWindspeed();
  static Gahm::Atcf::StormTranslation getTranslation(const Gahm::Atcf::AtcfSnap &now,
                                              const Gahm::Atcf::AtcfSnap &next);

  static double computeSimpleRelativeIsotachWindspeed(
      double wind_speed, Atcf::StormTranslation transit, int quadrant);

  Gahm::Atcf::AtcfFile *m_atcf{nullptr};
};
}  // namespace Gahm
#endif  // GAHM_SRC_PREPROCESSOR_PREPROCESSOR_H_
