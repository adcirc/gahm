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
#ifndef GAHM_SRC_GAHMSTATE_H_
#define GAHM_SRC_GAHMSTATE_H_

#include <vector>

#include "Atcf.h"
#include "Date.h"
#include "Interpolation.h"
#include "StormParameters.h"
namespace Gahm {
class GahmState {
 public:
  GahmState(Gahm::Atcf *atcf, std::vector<double> x_points,
            std::vector<double> y_points);

  void query(const Gahm::Date &d);

  double x(size_t index) const;
  double y(size_t index) const;

  size_t size();

  double distance(size_t index) const;
  double azimuth(size_t index) const;
  Gahm::StormParameters stormParameters() const;

  double stormDirection() const;
  double stormMotion() const;
  double stormMotionU() const;
  double stormMotionV() const;

 private:
  void computeDistanceToStormCenter(double stormCenterX, double stormCenterY);

  const std::vector<double> m_xpoints;
  const std::vector<double> m_ypoints;

  std::vector<double> m_distance;
  std::vector<double> m_azimuth;

  Gahm::Atcf *m_atcf;

  Gahm::StormParameters m_stormParametersQuery;

  double m_stormMotion;
  double m_direction;
  double m_stormMotionU;
  double m_stormMotionV;
};
}  // namespace Gahm
#endif  // GAHM_SRC_GAHMSTATE_H_
