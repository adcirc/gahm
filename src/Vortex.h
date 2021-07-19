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
#include <tuple>
#include <utility>
#include <vector>

#include "Assumptions.h"
#include "AtcfLine.h"
#include "Constants.h"

class Vortex {
 public:
  Vortex(AtcfLine *atcf, size_t currentRecord, size_t currentIsotach,
         Assumptions *assumptions);

  Vortex(AtcfLine *atcf, Assumptions *assumptions);

  void setStormData(AtcfLine *atcf);

  enum VortexParameterType { RMAX, B, VMBL };

  /// Number of quadrants for which wind radii are provided
  static const size_t nQuads = 4;

  /// Number of isotachs for which wind speeds may be provided
  static const size_t maxIsotachs = 4;

  struct ParameterPack {
    double vmaxBoundaryLayer;
    double radiusToMaxWinds;
    double radiusToMaxWindsTrue;
    double hollandB;
    int base_quadrant;
    double delta_angle;
    ParameterPack(double vmaxbl, double rmax, double rmaxtrue, double b,
                  int quad, double angle)
        : vmaxBoundaryLayer(vmaxbl),
          radiusToMaxWinds(rmax),
          radiusToMaxWindsTrue(rmaxtrue),
          hollandB(b),
          base_quadrant(quad),
          delta_angle(angle) {}
  };

  Vortex::ParameterPack getParameters(double angle, double distance) const;

  int computeRadiusToWind();

  unsigned currentQuadrant() const;
  void setCurrentQuadrant(unsigned quad);

  unsigned currentIsotach() const;
  void setCurrentIsotach(unsigned currentIsotach);

  static std::pair<double, double> rotateWinds(double x, double y, double angle,
                                               double whichWay) noexcept;

  constexpr static double default_inner_radius() {
    return 1.0 * Units::convert(Units::NauticalMile, Units::Kilometer);
  }

  constexpr static double default_outer_radius() {
    return 400.0 * Units::convert(Units::NauticalMile, Units::Kilometer);
  }

  size_t currentRecord() const;
  void setCurrentRecord(const size_t &currentRecord);

  Date datetime() const;

 private:
  unsigned m_currentQuadrant;
  unsigned m_currentIsotach;
  size_t m_currentRecord;
  AtcfLine *m_stormData;
  Assumptions *m_assumptions;

  static constexpr size_t m_max_it = 400;

  constexpr static std::pair<int, double> getBaseQuadrant(double angle);

  static constexpr double rossbyNumber(double vmaxBoundaryLayer,
                                       double radiusToMaxWinds,
                                       double coriolis);

  static constexpr double computePhi(double vmaxBoundaryLayer,
                                     double radiusToMaxWinds, double b,
                                     double coriolis);

  static constexpr double computeBg(double vmaxBoundaryLayer,
                                    double radiusToMaxWinds, double phi,
                                    double dp, double coriolis, double rho);

  static std::tuple<double, double> computeBandPhi(double vmax, double root,
                                                   double b, double cor,
                                                   double dp);

  Vortex::ParameterPack interpolateParameters(int quad, double distance,
                                              double angle) const;

  double iterateRadius() const;

  struct ShapeTerms {
    double b;
    double phi;
    bool converged;
    ShapeTerms(double b, double phi, bool converged)
        : b(b), phi(phi), converged(converged) {}
  };

  ShapeTerms iterateShapeTerms(double root) const;

  struct Root {
    double root;
    double left;
    double right;
    Root() = default;
    Root(double left, double right, double root)
        : left(left), right(right), root(root){};
  };

  Root findRoot(double aa, double bb, double zoom_window) const;
};

#endif  // VORTEX_H
