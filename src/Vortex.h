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
#ifndef VORTEX_H
#define VORTEX_H

#include <array>
#include <utility>
#include <vector>

class Vortex {
 public:
  Vortex(double pinf, double p0, double lon, double lat, double vmax);

  void setVortex(double pinf, double p0, double lon, double lat,
                 double vmax) noexcept;

  void setRadii(size_t index, std::array<double, 4> quadFlag,
                std::array<double, 4> rmax, std::array<double, 4> quadIr,
                std::array<double, 4> b, std::array<double, 4> vmbl);

  void fillRadii();

  enum VortexParameterType { RMAX, B, VMBL };

  template <VortexParameterType index>
  double interpolateParameter(const double angle, const double distance) {
    switch (index) {
      case RMAX:
        return this->spInterp(m_rMaxes4, angle, distance);
      case B:
        return this->spInterp(m_bs4, angle, distance);
      case VMBL:
        return this->spInterp(m_vmBL4, angle, distance);
    }
  }

  static double frictionAngle(const double r, const double rmx) noexcept;
  static std::pair<double, double> rotateWinds(double x, double y,
                                               const double angle,
                                               const double whichWay) noexcept;

 private:
  /// Number of quadrants for which wind radii are provided
  static constexpr size_t nQuads = 4;

  /// Number of points for curve fit
  static constexpr size_t nPoints = nQuads + 2;

  /// Ambient surface pressure (mb)
  double m_pn;

  /// Central pressure of vortex (mb)
  double m_pc;

  /// Latitude of storm center
  double m_cLat;

  /// Longitude of storm center
  double m_cLon;

  /// Maximum sustained wind velocity
  double m_vMax;

  /// Exponential shape parameter
  double m_b;

  /// Coriolis force (1/s)
  double m_corio;

  /// Velocity @ wind radii
  double m_vr;

  double m_phi;

  template <typename T>
  using WindArray = std::array<std::array<T, nQuads>, nPoints>;

  /// Radius of maximum winds
  WindArray<double> m_rMaxes4;

  WindArray<double> m_bs4;

  /// Correction factor to B and Vh
  WindArray<double> m_phis4;

  WindArray<double> m_vmBL4;

  WindArray<int> m_quadFlag4;

  WindArray<double> m_quadIr4;

  static std::pair<int, double> getBaseQuadrant(double angle);

  double spInterp(const WindArray<double> &array, double angle,
                  double distance) const;

  double interpR(const WindArray<double> &array, int quad, double r) const;

  static double coriolis(double lat) noexcept;

  static double calcHollandB(double vmax, double p0, double pinf) noexcept;

  template <typename T>
  static void fillEdges(WindArray<T> &arr) {
    arr[0] = arr[4];
    arr[5] = arr[1];
    return;
  }
};

#endif  // VORTEX_H
