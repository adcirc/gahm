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
#include "HurricanePressure.h"

#include <cmath>

#include "Logging.h"

HurricanePressure::HurricanePressure(HurricanePressure::PressureMethod p)
    : m_pressureMethod(p) {}

double HurricanePressure::computePressure(const double wind_speed) {
  return this->computePressure(wind_speed, 0.0, 0.0, 0.0, 0.0, 0.0);
}

double HurricanePressure::computePressure(const double wind_speed,
                                          const double vmax_global,
                                          const double last_vmax,
                                          const double last_pressure) {
  return this->computePressure(wind_speed, vmax_global, last_vmax,
                               last_pressure, 0.0, 0.0);
}

double HurricanePressure::computePressure(const double wind_speed,
                                          const double latitude,
                                          const double forward_speed) {
  return this->computePressure(wind_speed, 0.0, 0.0, 0.0, latitude,
                               forward_speed);
}

double HurricanePressure::computePressure(const double wind_speed,
                                          const double vmax_global,
                                          const double last_vmax,
                                          const double last_pressure,
                                          const double latitude,
                                          const double forward_speed) {
  switch (m_pressureMethod) {
    case KNAFFZEHR:
      return knaffzehr(wind_speed);
    case DVORAK:
      return dvorak(wind_speed);
    case AH77:
      return ah77(wind_speed);
    case ASGS2012:
      return asgs2012(wind_speed, vmax_global, last_vmax, last_pressure);
    case TWOSLOPE:
      return twoslope(wind_speed, last_vmax, last_pressure);
    case COURTNEYKNAFF:
      return courtneyknaff(wind_speed, forward_speed, latitude);
    default:
      gahm_throw_exception("No valid pressure method selected");
      return 0.0;
  }
}

double HurricanePressure::knaffzehr(const double wind_speed) {
  return HurricanePressure::computePressureCurveFit(wind_speed, 1010.0, 2.3,
                                                    0.760);
}

double HurricanePressure::dvorak(const double wind_speed) {
  return HurricanePressure::computePressureCurveFit(wind_speed, 1015.0, 3.92,
                                                    0.644);
}

double HurricanePressure::ah77(const double wind_speed) {
  return HurricanePressure::computePressureCurveFit(wind_speed, 1010.0, 3.4,
                                                    0.644);
}

double HurricanePressure::computePressureCurveFit(const double wind_speed,
                                                  const double a,
                                                  const double b,
                                                  const double c) {
  return a - std::pow(wind_speed / b, 1.0 / c);
}

double HurricanePressure::courtneyknaff(const double wind_speed,
                                        const double forward_speed,
                                        const double eye_latitude) {
  const double background_pressure = 1013.0;

  // Below from Courtney and Knaff 2009
  const double vsrm1 = wind_speed * 1.5 * std::pow(forward_speed, 0.63);

  const double rmax = 66.785 - 0.09102 * wind_speed +
                      1.0619 * (eye_latitude - 25.0);  // Knaff and Zehr 2007

  // Two options for v500 ... I assume that vmax is
  // potentially more broadly applicable than r34
  // option 1
  // v500 = r34 / 9.0 - 3.0

  // option 2
  const double v500 =
      wind_speed *
      std::pow(
          66.785 - 0.09102 * wind_speed + 1.0619 * (eye_latitude - 25) / 500,
          0.1147 + 0.0055 * wind_speed - 0.001 * (eye_latitude - 25));

  // Knaff and Zehr computes v500c
  const double v500c =
      wind_speed * std::pow(rmax / 500, (0.1147 + 0.0055 * wind_speed -
                                         0.001 * (eye_latitude - 25.0)));

  // Storm size parameter
  const double S = std::max(v500 / v500c, 0.4);

  const double dp =
      eye_latitude < 18.0
          ? 5.962 - 0.267 * vsrm1 - std::pow(vsrm1 / 18.26, 2.0) - 6.8 * S
          : 23.286 - 0.483 * vsrm1 - std::pow(vsrm1 / 24.254, 2.0) -
                12.587 * S - 0.483 * eye_latitude;

  return dp + background_pressure;
}

double HurricanePressure::computeInitialPressureEstimate(
    const double wind_speed, const double last_vmax,
    const double last_pressure) {
  double our_last_pressure = last_pressure;
  if (last_pressure == 0.0) {
    if (last_vmax != 0.0) {
      our_last_pressure = HurricanePressure::dvorak(last_vmax);
    }
  }

  double p = our_last_pressure;
  if (wind_speed > last_vmax) {
    p = 1040.0 - 0.877 * wind_speed;
  } else if (wind_speed < last_vmax) {
    p = 1000.0 - 0.65 * wind_speed;
    if (p < our_last_pressure) {
      p = our_last_pressure + 0.65 * (last_vmax - wind_speed);
    }
  }

  return p;
}

double HurricanePressure::asgs2012(const double wind_speed,
                                   const double vmax_global,
                                   const double last_vmax,
                                   const double last_pressure) {
  double p = HurricanePressure::computeInitialPressureEstimate(
      wind_speed, last_vmax, last_pressure);
  if (wind_speed <= 35.0) {
    if (vmax_global > 39.0) {
      p = HurricanePressure::dvorak(wind_speed);
    } else {
      p = HurricanePressure::ah77(wind_speed);
    }
  }
  return p;
}

double HurricanePressure::twoslope(const double wind_speed,
                                   const double last_vmax,
                                   const double last_pressure) {
  return wind_speed < 30.0 ? last_pressure
                           : HurricanePressure::computeInitialPressureEstimate(
                                 wind_speed, last_vmax, last_pressure);
}

std::string HurricanePressure::pressureMethodString(
    const HurricanePressure::PressureMethod &method) {
  switch (method) {
    case KNAFFZEHR:
      return "Knaff-Zehr";
    case DVORAK:
      return "Dvorak";
    case AH77:
      return "AH77";
    case ASGS2012:
      return "ASGS2012";
    case TWOSLOPE:
      return "Two Slope";
    case COURTNEYKNAFF:
      return "Courtney-Knaff";
  }
}
HurricanePressure::PressureMethod HurricanePressure::pressureMethod() const {
  return m_pressureMethod;
}
