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
#include <memory>
#include <tuple>
#include <utility>
#include <vector>

#include "Assumptions.h"
#include "AtcfLine.h"
#include "Constants.h"
#include "ParameterPack.h"

namespace Gahm {
class Vortex {
 public:
  Vortex(Gahm::AtcfLine *atcf, size_t currentRecord, size_t currentIsotach,
         std::shared_ptr<Gahm::Assumptions> assumptions = nullptr);

  explicit Vortex(Gahm::AtcfLine *atcf,
                  std::shared_ptr<Gahm::Assumptions> assumptions = nullptr);

  void setStormData(Gahm::AtcfLine *atcf);

  enum VortexParameterType { RMAX, B, VMBL };

  /// Number of quadrants for which wind radii are provided
  static const size_t nQuads = 4;

  /// Number of isotachs for which wind speeds may be provided
  static const size_t maxIsotachs = 4;

  Gahm::ParameterPack getParameters(double angle, double distance) const;

  int computeRadiusToWind();

  unsigned currentQuadrant() const;
  void setCurrentQuadrant(unsigned quad);

  unsigned currentIsotach() const;
  void setCurrentIsotach(unsigned currentIsotach);

  static std::pair<double, double> rotateWinds(double x, double y, double angle,
                                               double latitude) noexcept;

  constexpr static double default_inner_radius() {
    return 1.0 *
           Units::convert(Gahm::Units::NauticalMile, Gahm::Units::Kilometer);
  }

  constexpr static double default_outer_radius() {
    return 400.0 * Gahm::Units::convert(Gahm::Units::NauticalMile,
                                        Gahm::Units::Kilometer);
  }

  size_t currentRecord() const;
  void setCurrentRecord(const size_t &currentRecord);

  Gahm::Date datetime() const;

 private:
  unsigned m_currentQuadrant;
  unsigned m_currentIsotach;
  size_t m_currentRecord;
  Gahm::AtcfLine *m_stormData;
  std::shared_ptr<Gahm::Assumptions> m_assumptions;

  static constexpr size_t m_max_it = 400;

  static std::pair<int, double> getBaseQuadrant(double angle);

  static constexpr double rossbyNumber(double vmaxBoundaryLayer,
                                       double radiusToMaxWinds,
                                       double coriolis);

  static constexpr double computePhi(double vmaxBoundaryLayer,
                                     double radiusToMaxWinds, double b,
                                     double coriolis);

  static double computeBg(double vmaxBoundaryLayer, double radiusToMaxWinds,
                          double phi, double dp, double coriolis, double rho);

  static std::tuple<double, double> computeBandPhi(double vmax, double root,
                                                   double b, double cor,
                                                   double dp);

  Gahm::ParameterPack interpolateParameters(int quad, double distance,
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
}  // namespace Gahm
#endif  // VORTEX_H
