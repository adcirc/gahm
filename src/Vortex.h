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
#include <cassert>
#include <utility>
#include <vector>
#include "AtcfLine.h"
#include "Physical.h"

class Vortex {
 public:
  Vortex();

  Vortex(AtcfLine *atcf, size_t currentIsotach);

  void setStormData(AtcfLine *atcf);

  enum VortexParameterType { RMAX, B, VMBL };

  /// Number of quadrants for which wind radii are provided
  static const size_t nQuads = 4;

  /// Number of isotachs for which wind speeds may be provided
  static const size_t maxIsotachs = 4;

  template <VortexParameterType index>
  double interpolateParameter(const double angle, const double distance) {
    switch (index) {
      case RMAX:
        return this->spInterp<index>(angle, distance);
      case B:
        return this->spInterp<index>(angle, distance);
      case VMBL:
        return this->spInterp<index>(angle, distance);
    }
  }

  int computeRadiusToWind();

  size_t currentQuadrant() const;
  void setCurrentQuadrant(size_t quad);

  size_t currentIsotach() const;
  void setCurrentIsotach(const size_t &currentIsotach);

  static std::pair<double, double> rotateWinds(double x, double y, double angle,
                                               double whichWay) noexcept;

  constexpr static double default_inner_radius() { return 1.0; }

  constexpr static double default_outer_radius() { return 400.0; }

 private:
  size_t m_currentQuadrant;
  size_t m_currentIsotach;
  AtcfLine *m_stormData;

  static std::pair<int, double> getBaseQuadrant(double angle);

  template <Vortex::VortexParameterType type, bool edge>
  double getParameterValue(const size_t isotach, const size_t quad);

  template <VortexParameterType type>
  double spInterp(double angle, double distance) const;

  template <VortexParameterType type>
  double interpR(int quad, double r) const;

  static constexpr double rossbyNumber(double vmaxBoundaryLayer,
                                       double radiausToMaxWinds,
                                       double coriolis) noexcept;

  static constexpr double computePhi(double vmaxBoundaryLayer,
                                     double radiusToMaxWinds, double b,
                                     double coriolis) noexcept;

  static double computeBg(const double vmaxBoundaryLayer,
                          const double radiusToMaxWinds, const double phi,
                          const double dp, const double coriolis,
                          const double rho) noexcept;

  enum VhType { VhNoCori, VhWithCori, VhWithCoriFull };

  template <Vortex::VhType vh>
  std::pair<double, double> getVh(double aa) const;

  struct Root {
    double root;
    double left;
    double right;
    Root() = default;
    Root(double left, double right, double root)
        : left(left), right(right), root(root){};
  };

  Root findRoot(double aa, double bb, double zoom_window) const;

  std::pair<double, double> vhNoCori(double r) const;
  std::pair<double, double> vhWithCori(double r) const;
  std::pair<double, double> vgWithCoriFull(double r) const;
};

#endif  // VORTEX_H
