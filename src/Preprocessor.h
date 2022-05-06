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
#ifndef PREPROCESSOR_H
#define PREPROCESSOR_H

#include <iostream>
#include <memory>
#include <vector>

#include "Assumptions.h"
#include "Atcf.h"
#include "StormMotion.h"

namespace Gahm {
class Preprocessor {
 public:
  explicit Preprocessor(Atcf *atcfData);

  int run();

 private:
  static StormMotion computeTranslationSpeed(const AtcfLine &d1,
                                             const AtcfLine &d2);

  int calculateOverlandTranslationVelocity();

  void setAllRadiiToRmax(CircularArray<double, 4> &radii,
                         CircularArray<bool, 4> &quadFlag, double rmax,
                         size_t record, size_t isotach);

  void setMissingRadiiToHalfNonzeroRadii(CircularArray<double, 4> &radii,
                                         double radiisum, size_t record,
                                         size_t isotach);

  void setMissingRadiiToHalfOfAverageSpecifiedRadii(
      CircularArray<double, 4> &radii, double radiisum, size_t record,
      size_t isotach);

  void setMissingRadiiToAverageOfAdjacentRadii(CircularArray<double, 4> &radii,
                                               size_t record, size_t isotach);

  int calculateRadii();

  int computeParameters();

  int generateMissingPressureData(const HurricanePressure::PressureMethod
                                      &method = HurricanePressure::KNAFFZEHR);

  static double computeIsotachDeTranslatedWindSpeed(const AtcfLine &atcfData,
                                                    const Isotach &iso,
                                                    int quadrant);

  static double computeGamma(const StormMotion &storm_motion, double u_vr,
                             double v_vr, double vmax_boundary_layer,
                             const Isotach &iso, size_t quadrant);

  static double computeVmaxAtBoundaryLayer(double vmax,
                                           double stormForwardSpeed);

  static unsigned countNonzeroIsotachs(
      const std::vector<AtcfLine>::iterator &ait, size_t i);

  Atcf *m_data;
};
}  // namespace Gahm
#endif  // PREPROCESSOR_H
