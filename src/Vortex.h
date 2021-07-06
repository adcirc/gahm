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
  Vortex() = default;

  Vortex(AtcfLine *atcf, size_t currentRecord, size_t currentIsotach,
         Assumptions *assumptions);

  Vortex(AtcfLine *atcf, Assumptions *assumptions);

  void setStormData(AtcfLine *atcf);

  enum VortexParameterType { RMAX, B, VMBL };

  /// Number of quadrants for which wind radii are provided
  static const size_t nQuads = 4;

  /// Number of isotachs for which wind speeds may be provided
  static const size_t maxIsotachs = 4;

  template <VortexParameterType index>
  double interpolateParameter(const double angle, const double distance) {
    return this->spInterp<index>(angle, distance);
  }

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

 private:
  unsigned m_currentQuadrant;
  unsigned m_currentIsotach;
  size_t m_currentRecord;
  AtcfLine *m_stormData;
  Assumptions *m_assumptions;

  static constexpr size_t m_max_it = 400;

  static std::pair<int, double> getBaseQuadrant(double angle);

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

  template <Vortex::VortexParameterType type>
  double spInterp(const double angle, const double distance) const {
    int base_quadrant = 0;
    double delta_angle = 0.0;
    std::tie(base_quadrant, delta_angle) = Vortex::getBaseQuadrant(angle);
    if (delta_angle < 1.0) {
      return this->interpR<type>(base_quadrant - 1, distance);
    } else if (delta_angle > 89.0) {
      return this->interpR<type>(base_quadrant, distance);
    } else {
      const double t1 = this->interpR<type>(base_quadrant - 1, distance);
      const double t2 = this->interpR<type>(base_quadrant, distance);
      return (t1 / std::pow(delta_angle, 2.0) +
              t2 / std::pow(90.0 - delta_angle, 2.0)) /
             (1.0 / std::pow(delta_angle, 2.0) +
              1.0 / std::pow(90.0 - delta_angle, 2.0));
    }
  }

  template <Vortex::VortexParameterType type>
  double getParameterValue(const size_t isotach, const unsigned quad) const {
    if (type == Vortex::VortexParameterType::B) {
      return m_stormData->cisotach(isotach)->chollandB()->at(quad);
    } else if (type == Vortex::VortexParameterType::RMAX) {
      return m_stormData->cisotach(isotach)->crmax()->at(quad);
    } else if (type == Vortex::VortexParameterType::VMBL) {
      return m_stormData->cisotach(isotach)->cvmaxBl()->at(quad);
    } else {
      return 0.0;
    }
  }

  template <Vortex::VortexParameterType type>
  double interpR(const int quad, const double r) const {
    // These are somewhat ordered according to likelihood. This is a firey hot
    // section of code. It's most likely for a typical adcirc mesh that much of
    // your storm will be outside of the last isotach for the largest portion of
    // the storm. That case goes first to avoid additional lookups when they're
    // not needed. It's least likely that you're inside the smallest isotach,
    // but it's a single lookup to determine that and the same lookup would be
    // necessary to determine the else case anyway.

    //...Handle case where there are no isotachs
    if (m_stormData->nIsotach() == 0) {
      if (type == VortexParameterType::VMBL) {
        return m_stormData->vmaxBl();
      } else if (type == VortexParameterType::RMAX) {
        return m_stormData->radiusMaxWinds();
      } else if (type == VortexParameterType::B) {
        return m_stormData->hollandB();
      }
    }

    if (r >
        m_stormData->isotach(m_stormData->nIsotach() - 1)->crmax()->at(quad)) {
      return this->getParameterValue<type>(m_stormData->nIsotach() - 1, quad);
    } else if (r < m_stormData->isotach(0)->crmax()->at(quad)) {
      return this->getParameterValue<type>(0, quad);
    } else {
      const auto radii = m_stormData->isotachRadii(quad);
      const auto it = std::lower_bound(radii.begin(), radii.end(), r);
      const auto p = it - radii.begin();
      double fac = (r - radii[p]) / (radii[p + 1] - radii[p]);
      return fac * this->getParameterValue<type>(p, quad) +
             (1.0 - fac) * this->getParameterValue<type>(p + 1, quad);
    }
  }
};

#endif  // VORTEX_H
