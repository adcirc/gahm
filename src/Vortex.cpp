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
#include "Vortex.h"

#include <cassert>
#include <limits>
#include <numeric>
#include <utility>
#include "Logging.h"
#include "Physical.h"
#include "VortexSolver.h"

Vortex::Vortex(Assumptions *assumptions)
    : m_currentQuadrant(0),
      m_currentIsotach(0),
      m_currentRecord(0),
      m_stormData(nullptr),
      m_assumptions(assumptions) {}

Vortex::Vortex(AtcfLine *atcf, const size_t currentRecord,
               const size_t currentIsotach, Assumptions *assumptions)
    : m_currentQuadrant(0),
      m_currentIsotach(currentIsotach),
      m_currentRecord(currentRecord),
      m_stormData(atcf),
      m_assumptions(assumptions) {}

void Vortex::setStormData(AtcfLine *atcf) { m_stormData = atcf; }

std::tuple<double, double, bool> Vortex::iterateShapeTerms(
    const double root) const {
  constexpr double accuracy = 0.01;
  double phi_new = Vortex::computePhi(m_stormData->vmax(), root,
                                      m_stormData->cisotach(m_currentIsotach)
                                          ->chollandB()
                                          ->at(m_currentQuadrant),
                                      m_stormData->coriolis());
  double b_new = Vortex::computeBg(
      m_stormData->cisotach(m_currentIsotach)->cvmaxBl()->at(m_currentQuadrant),
      m_stormData->radiusMaxWinds(), phi_new, m_stormData->centralPressure(),
      m_stormData->coriolis(), Physical::rhoAir());
  for (size_t i = 0; i < m_max_it; ++i) {
    double b_new1 = b_new;
    phi_new = Vortex::computePhi(m_stormData->vmax(), root, b_new,
                                 m_stormData->coriolis());
    b_new = Vortex::computeBg(m_stormData->cisotach(m_currentIsotach)
                                  ->cvmaxBl()
                                  ->at(m_currentQuadrant),
                              m_stormData->radiusMaxWinds(), phi_new,
                              m_stormData->centralPressure(),
                              m_stormData->coriolis(), Physical::rhoAir());
    if (std::abs(b_new1 - b_new) < accuracy) {
      Logging::debug("Shape iteration converged in " + std::to_string(i + 1) +
                     " iterations.");
      return std::make_tuple(b_new, phi_new, true);
    }
  }
  return std::make_tuple(b_new, phi_new, false);
}

double Vortex::iterateRadius() const {
  constexpr size_t max_root_it = 3;
  constexpr double zoom_size = 0.01;
  double r1 = Vortex::default_inner_radius();
  double r2 = Vortex::default_outer_radius();
  Root r(r1, r2, 0.0);
  double dr = 1.0;
  for (size_t rootit = 0; rootit < max_root_it; ++rootit) {
    r = this->findRoot(r1, r2, dr);
    r1 = r.left;
    r2 = r.right;
    dr *= zoom_size;
  }
  return r.root;
}

int Vortex::computeRadiusToWind() {
  for (size_t quad = 0; quad < 4; quad++) {
    this->setCurrentQuadrant(quad);

    bool converged = false;
    for (size_t it = 0; it < m_max_it; ++it) {
      double root = this->iterateRadius();
      if (root < 0.0) {
        root =
            m_stormData->isotach(m_currentIsotach)->isotachRadius()->at(quad);
        m_stormData->isotach(m_currentIsotach)->rmax()->set(quad, root);
        m_assumptions->add(generate_assumption(
            Assumption::MAJOR,
            "Root could not be found for record " +
                std::to_string(m_currentRecord) +
                ", Isotach: " + std::to_string(m_currentIsotach) +
                ", Quadrant: " + std::to_string(quad)));
      } else {
        m_stormData->isotach(m_currentIsotach)->rmax()->set(quad, root);
      }

      double b_new, phi_new;
      std::tie(b_new, phi_new, converged) = this->iterateShapeTerms(root);
      if (converged) {
        Logging::debug("Radius iteration converged in " +
                       std::to_string(it + 1) + " iterations.");
        m_stormData->isotach(m_currentIsotach)->hollandB()->set(quad, b_new);
        m_stormData->isotach(m_currentIsotach)->phi()->set(quad, phi_new);
        break;
      }
      if (it == m_max_it - 1 && !converged) {
        Logging::debug("Radius iteration failed to converged.");
      }
    }
  }
  return 0;
}

std::pair<double, double> Vortex::rotateWinds(const double x, const double y,
                                              const double angle,
                                              const double whichWay) noexcept {
  double a = Units::convert(Units::Degree, Units::Radian) * angle;
  if (whichWay >= 0.0) a *= -1.0;
  const double cosA = std::cos(a);
  const double sinA = std::sin(a);
  return std::make_pair(x * cosA - y * sinA, x * sinA + y * cosA);
}

size_t Vortex::currentRecord() const { return m_currentRecord; }

void Vortex::setCurrentRecord(const size_t &currentRecord) {
  m_currentRecord = currentRecord;
}

size_t Vortex::currentIsotach() const { return m_currentIsotach; }

void Vortex::setCurrentIsotach(const size_t &currentIsotach) {
  m_currentIsotach = currentIsotach;
}

size_t Vortex::currentQuadrant() const { return m_currentQuadrant; }

void Vortex::setCurrentQuadrant(size_t quad) { m_currentQuadrant = quad; }

std::pair<int, double> Vortex::getBaseQuadrant(const double angle) {
  assert(angle <= 360.0);
  assert(angle >= 0.0);
  if (angle <= 45.0) {
    return std::make_pair(5, 45.0 + angle);
  } else if (angle <= 135.0) {
    return std::make_pair(2, angle - 45.0);
  } else if (angle <= 225.0) {
    return std::make_pair(3, angle - 135.0);
  } else if (angle <= 315.0) {
    return std::make_pair(4, angle - 225.0);
  } else if (angle > 315.0) {
    return std::make_pair(5, angle - 315.0);
  }
  gahm_throw_exception("Invalid angle provided!");
  return std::make_pair(0, 0.0);
}

template <Vortex::VortexParameterType type>
double Vortex::spInterp(const double angle, const double distance) const {
  int base_quadrant = 0;
  double delta_angle = 0.0;
  std::tie(base_quadrant, delta_angle) = this->getBaseQuadrant(angle);
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

template <Vortex::VortexParameterType type, bool edge>
double Vortex::getParameterValue(const size_t isotach, const size_t quad) {
  if (type == Vortex::VortexParameterType::B) {
    if (edge)
      return m_stormData->cisotach(isotach)->chollandB()->at(quad);
    else
      return m_stormData->cisotach(isotach)->chollandB()->at(quad);
  } else if (type == Vortex::VortexParameterType::RMAX) {
    if (edge)
      return m_stormData->cisotach(isotach)->crmax()->at(quad);
    else
      return m_stormData->cisotach(isotach)->crmax()->at(quad);
  } else if (type == Vortex::VortexParameterType::VMBL) {
    if (edge)
      return m_stormData->cisotach(isotach)->cvmaxBl()->at(quad);
    else
      return m_stormData->cisotach(isotach)->cvmaxBl()->at(quad);
  } else {
    return 0.0;
  }
}

template <Vortex::VortexParameterType type>
double Vortex::interpR(const int quad, const double r) const {
  size_t total_isotachs = 0;
  size_t last_isotach = 0;
  for (size_t i = 0; i < m_stormData->nIsotach(); ++i) {
    if (m_stormData->cisotach(i)->cquadFlag()->at(quad)) {
      total_isotachs++;
      last_isotach = i;
    }
  }

  assert(total_isotachs > 0);

  if (r < m_stormData->cisotach(0)->crmax()->at(quad)) {
    return this->getParameterValue<type>(0, quad);
  }

  if (r > m_stormData->cisotach(last_isotach)->crmax()->at(quad)) {
    return this->getParameterValue<type>(last_isotach, quad);
  }

  for (size_t i = 1; i < total_isotachs; ++i) {
    if (r > m_stormData->cisotach(i)->crmax()->at(quad)) {
      const double fac = (r - m_stormData->cisotach(i)->crmax()->at(quad)) /
                         (m_stormData->cisotach(i - 1)->crmax()->at(quad) -
                          m_stormData->cisotach(i)->crmax()->at(quad));
      return fac * this->getParameterValue<type>(i - 1, quad) +
             (1.0 - fac) * this->getParameterValue<type>(i, quad);
    }
  }
  return this->getParameterValue<type>(last_isotach, quad);
}

/**
 * Computes the estimate of the Rossby number in a tropical cyclone
 * @param vmaxBoundaryLayer max wind speed for stationary storm at top of
 * boundary layer
 * @param radiausToMaxWinds radius from storm center to vmaxBoundaryLayer
 * @param coriolis Coriolis parameter
 * @return estimate to Rossby number
 */
constexpr double Vortex::rossbyNumber(const double vmaxBoundaryLayer,
                                      const double radiausToMaxWinds,
                                      const double coriolis) noexcept {
  assert(coriolis > 0.0);
  assert(radiausToMaxWinds > 0.0);
  return vmaxBoundaryLayer / (coriolis * radiausToMaxWinds);
}

/**
 * Compute the Phi parameter in the GAHM
 * @param vmaxBoundaryLayer max wind speed for stationary storm at top of
 * boundary layer
 * @param radiausToMaxWinds radius from storm center to vmaxBoundaryLayer
 * @param b GAHM b_g parameter, simliar to Holland's B parameter
 * @param coriolis Coriolis parameter
 * @return phi
 */
constexpr double Vortex::computePhi(const double vmaxBoundaryLayer,
                                    const double radiusToMaxWinds,
                                    const double b,
                                    const double coriolis) noexcept {
  const double rossby =
      Vortex::rossbyNumber(vmaxBoundaryLayer, radiusToMaxWinds, coriolis);
  return 1.0 + 1.0 / (rossby * b * (1.0 + 1.0 / rossby));
}

/**
 * Computes the GAHM b_g parameter
 * @param vmaxBoundaryLayer max wind speed for stationary storm at top of
 * boundary layer
 * @param radiusToMaxWinds radius from storm center to vmaxBoundaryLayer
 * @param phi Correction factor
 * @param dp central pressure deficit
 * @param coriolis Coriolis parameter
 * @param rho density of air
 * @return GAHM b_g parameter
 */
double Vortex::computeBg(const double vmaxBoundaryLayer,
                         const double radiusToMaxWinds, const double phi,
                         const double dp, const double coriolis,
                         const double rho) noexcept {
  return (std::pow(vmaxBoundaryLayer, 2.0) +
          vmaxBoundaryLayer * radiusToMaxWinds * coriolis) *
         rho * std::exp(phi) / (phi * dp);
}

Vortex::Root Vortex::findRoot(double aa, double bb,
                              const double zoom_window) const {
  constexpr size_t itmax = 400;
  auto vortex_function = VortexSolver<VortexSolverType::NoDerivative>(
      m_stormData, m_currentQuadrant, m_currentIsotach);
  double fa = vortex_function(aa);

  for (size_t it = 0; it < itmax; ++it) {
    bb = aa + (it + 1) * zoom_window;
    double fb = vortex_function(bb);

    if (fa * fb < 0.0 || (std::abs(fb) > std::abs(fa))) {
      Logging::debug("Bracket solver converged in " + std::to_string(it + 1) +
                     " iterations.");
      if (std::abs(fb) > std::abs(fa)) {
        return Root(aa, bb, aa);
      } else {
        return Root(bb, aa, bb);
      }
    }
    aa = bb;
    fa = fb;
  }
  Logging::debug("Bracket solver failed to converge in" +
                 std::to_string(itmax));
  return Root(aa, bb, -1.0);
}
