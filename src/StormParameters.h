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
#ifndef GAHM_SRC_STORMPARAMETERS_H_
#define GAHM_SRC_STORMPARAMETERS_H_

#include <iostream>

#include "Physical.h"
#include "StormMotion.h"

namespace Gahm {
class StormParameters {
 public:
  StormParameters()
      : m_cycle(0),
        m_wtratio(0.0),
        m_latitude(0.0),
        m_longitude(0.0),
        m_central_pressure(Physical::backgroundPressure()),
        m_background_pressure(Physical::backgroundPressure()),
        m_vmax(0.0),
        m_stormMotion(0.0, 0.0, false),
        m_corio(0.0){};

  int cycle() const { return m_cycle; }
  void setCycle(int cycle) { m_cycle = cycle; }

  double wtratio() const { return m_wtratio; }
  void setWtratio(double wtratio) { m_wtratio = wtratio; }

  double latitude() const { return m_latitude; }
  void setLatitude(double latitude) {
    m_latitude = latitude;
    m_corio = Physical::coriolis(m_latitude);
  }

  double longitude() const { return m_longitude; }
  void setLongitude(double longitude) { m_longitude = longitude; }

  double centralPressure() const { return m_central_pressure; }
  void setCentralPressure(double centralPressure) {
    m_central_pressure = centralPressure;
  }

  double backgroundPressure() const { return m_background_pressure; }
  void setBackgroundPressure(double backgroundPressure) {
    m_background_pressure = backgroundPressure;
  }

  double vmax() const { return m_vmax; }
  void setVmax(double vmax) { m_vmax = vmax; }

  double corio() const { return m_corio; }

  Gahm::StormMotion stormMotion() const { return m_stormMotion; }
  void setStormMotion(const Gahm::StormMotion &s) { m_stormMotion = s; }

  void printToScreen() const {
    std::cout << "Cycle: " << cycle() << std::endl;
    std::cout << "WeightRatio: " << wtratio() << std::endl;
    std::cout << "Latitude: " << latitude() << std::endl;
    std::cout << "Longitude: " << longitude() << std::endl;
    std::cout << "centralPressure: " << centralPressure() << std::endl;
    std::cout << "backgroundPressure: " << backgroundPressure() << std::endl;
    std::cout << "vmax: " << vmax() << std::endl;
    std::cout << "utrans: " << stormMotion().u() << std::endl;
    std::cout << "vtrans: " << stormMotion().v() << std::endl;
    std::cout << "corio: " << corio() << std::endl;
  }

 private:
  int m_cycle;
  double m_wtratio;
  double m_latitude;
  double m_longitude;
  double m_central_pressure;
  double m_background_pressure;
  double m_vmax;
  StormMotion m_stormMotion;
  double m_corio;
};
}  // namespace Gahm
#endif  // GAHM_SRC_STORMPARAMETERS_H_
