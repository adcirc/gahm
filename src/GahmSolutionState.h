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
#ifndef GAHM_SRC_GAHMSOLUTIONSTATE_H_
#define GAHM_SRC_GAHMSOLUTIONSTATE_H_

#include <tuple>
#include <vector>

#include "Atcf.h"
#include "Date.h"
#include "Interpolation.h"
#include "StormParameters.h"

class GahmSolutionState {
 public:
  GahmSolutionState(Atcf *atcf, std::vector<double> x_points,
                    std::vector<double> y_points);

  void query(const Date &d);

  double x(size_t index);
  double y(size_t index);

  size_t size();

  double distance(size_t index);
  double azimuth(size_t index);
  StormParameters stormParameters();

 private:
  void generateUpdatedParameters(const Date &d);

  std::tuple<std::vector<double>, std::vector<double>>
  computeDistanceToStormCenter(double stormCenterX, double stormCenterY);

  const std::vector<double> m_xpoints;
  const std::vector<double> m_ypoints;

  std::vector<double> m_distance1;
  std::vector<double> m_distance2;
  std::vector<double> m_azimuth1;
  std::vector<double> m_azimuth2;
  std::vector<double> m_distanceQuery;
  std::vector<double> m_azimuthQuery;

  Atcf *m_atcf;

  Date m_date1;
  Date m_date2;

  StormParameters m_stormParametersQuery;

  bool m_initialized;
};

#endif  // GAHM_SRC_GAHMSOLUTIONSTATE_H_
