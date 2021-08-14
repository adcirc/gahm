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

namespace Gahm {
class Preprocessor {
 public:
  explicit Preprocessor(Atcf *atcfData);

  int run();

 private:
  static int uvTrans(const AtcfLine &d1, const AtcfLine &d2, double &uv,
                     double &vv, double &uuvv);
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

  struct StormMotion {
    double u;
    double v;
    double uv;
    StormMotion(double ui, double vi, double uvi) : u(ui), v(vi), uv(uvi) {}
    void write(const double unitconversion = 1.0) const {
      std::cout << "U-velocity: " << u * unitconversion
                << ", V-Velocity: " << v * unitconversion
                << ", Speed: " << uv * unitconversion << std::endl;
    }
  };

  static StormMotion computeStormMotion(double speed, double direction);

  static double computeGamma(double uvr, double vvr, double vr,
                             const StormMotion &stormMotion, double vmaxBL);

  static double computeEpsilonAngle(double velocity, double quadrantVectorAngle,
                                    const StormMotion &stormMotion);

  static double computeQuadrantVrValue(double vmaxBL,
                                       double quadrantVectorAngles,
                                       const StormMotion &stormMotion,
                                       double vr);

  static double computeQuadrantVrValue(double quadrantVectorAngle,
                                       const StormMotion &stormMotion,
                                       double vr);

  static double computeVMaxBL(double vmax, double stormMotion);

  static double computeQuadrantVectorAngle(
      size_t index, std::array<double, 4> quadRotateAngle);

  static void computeQuadrantVrLoop(
      size_t quadrotindex, const std::array<double, 4> &quadRotateAngle,
      const std::array<bool, 4> &vmwBLflag, double vmaxBL, double vr,
      const StormMotion &stormMotion, Isotach &isotach);

  static void recomputeQuadrantVrLoop(
      size_t quadrotindex, const std::array<double, 4> &quadRotateAngle,
      std::array<bool, 4> &vmwBLflag, double vmaxBL, double vr,
      double stormDirection, const StormMotion &stormMotion, Isotach &isotach);

  static unsigned countNonzeroIsotachs(
      const std::vector<AtcfLine>::iterator &ait, size_t i);

  static std::array<double, 4> computeQuadRotateAngle(const AtcfLine &a,
                                                      size_t i);

  void convergeInwardRotationAngle(size_t rec_counter, size_t i, AtcfLine &a,
                                   const StormMotion &stormMotion, double vr,
                                   const std::array<double, 4> &quadRotateAngle,
                                   std::array<bool, 4> &vmwBLflag) const;

  Atcf *m_data;
};
}  // namespace Gahm
#endif  // PREPROCESSOR_H
