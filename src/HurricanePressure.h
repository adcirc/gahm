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
#ifndef HURRICANEPRESSURE_H
#define HURRICANEPRESSURE_H

#include <string>

class HurricanePressure {
 public:
  enum PressureMethod {
    KNAFFZEHR,
    DVORAK,
    AH77,
    ASGS2012,
    TWOSLOPE,
    COURTNEYKNAFF
  };

  HurricanePressure(PressureMethod p = TWOSLOPE);

  double computePressure(double wind_speed);

  double computePressure(double wind_speed, double vmax_global,
                         double last_vmax, double last_pressure);

  double computePressure(double wind_speed, double latitude,
                         double forward_speed);

  double computePressure(double wind_speed, double vmax_global,
                         double last_vmax, double last_pressure,
                         double latitude, double forward_speed);

  static double computeInitialPressureEstimate(double wind_speed,
                                               double last_vmax = 0.0,
                                               double last_pressure = 0.0);

  static std::string pressureMethodString(
      const HurricanePressure::PressureMethod &method);

  HurricanePressure::PressureMethod pressureMethod() const;

 private:
  static double knaffzehr(double wind_speed);
  static double dvorak(double wind_speed);
  static double ah77(double wind_speed);

  static double asgs2012(double wind_speed, double vmax_global,
                         double last_vmax, double last_pressure);
  static double twoslope(double wind_speed, double last_vmax,
                         double last_pressure);
  static double courtneyknaff(double wind_speed, double forward_speed,
                              double eye_latitude);
  static double computePressureCurveFit(double wind_speed, double a, double b,
                                        double c);

  const PressureMethod m_pressureMethod;
};

#endif  // HURRICANEPRESSURE_H
