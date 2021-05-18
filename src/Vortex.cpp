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
#include "boost/cstdint.hpp"
#include "boost/math/tools/roots.hpp"

Vortex::Vortex()
    : m_currentQuadrant(0), m_currentIsotach(0), m_stormData(nullptr) {}

Vortex::Vortex(AtcfLine *atcf, const size_t currentIsotach)
    : m_currentQuadrant(0),
      m_currentIsotach(currentIsotach),
      m_stormData(atcf) {}

void Vortex::setStormData(AtcfLine *atcf) { m_stormData = atcf; }

int Vortex::computeRadiusToWind() {
  const size_t max_it = 400;
  const size_t max_root_it = 3;
  const double zoom_size = 0.01;

  for (size_t quad = 0; quad < 4; quad++) {
    bool rootFailed = false;
    double r1 = Vortex::default_inner_radius();
    double r2 = Vortex::default_outer_radius();
    for (size_t it = 0; it < max_it; ++it) {
      rootFailed = false;

      Root r(r1, r2, 0.0);
      double dr = 1.0;
      for (size_t rootit = 0; rootit < max_root_it; ++rootit) {
        r = this->findRoot(r1, r2, dr);
        r1 = r.left;
        r2 = r.right;
        dr *= zoom_size;
      }
      std::cout << r.root * Physical::km2nmi() << std::endl;
      if (r.root < 0.0) {
        r.root =
            m_stormData->isotach(m_currentIsotach)->isotachRadius()->at(quad);
        rootFailed = true;
      }
      m_stormData->isotach(m_currentIsotach)->rmax()->set(quad, r.root);
    }
  }
  return 0;
}

std::pair<double, double> Vortex::rotateWinds(const double x, const double y,
                                              const double angle,
                                              const double whichWay) noexcept {
  double a = Physical::deg2rad() * angle;
  if (whichWay >= 0.0) a *= -1.0;
  const double cosA = std::cos(a);
  const double sinA = std::sin(a);
  return std::make_pair(x * cosA - y * sinA, x * sinA + y * cosA);
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

constexpr int generate_digits_accuracy() {
  constexpr int digits =
      std::numeric_limits<double>::digits;  // Maximum possible binary digits
                                            // accuracy for type double.
  constexpr int get_digits = static_cast<int>(
      digits * 0.6);  // Accuracy doubles with each step, so stop when we have
                      // just over half the digits correct.
  return get_digits;
}

constexpr double generate_accuracy() {
  double d = 1.0;
  for (size_t i = 0; i < generate_digits_accuracy(); ++i) {
    d *= 10.0;
  }
  return 1.0 / d;
}

Vortex::Root Vortex::findRoot(double aa, double bb,
                              const double zoom_window) const {
  auto vortex_function = VortexSolver(
      m_stormData->radiusMaxWinds(),
      m_stormData->cisotach(m_currentIsotach)->cvmaxBl()->at(m_currentQuadrant),
      m_stormData->cisotach(m_currentIsotach)
          ->chollandB()
          ->at(m_currentQuadrant),
      m_stormData->cisotach(m_currentIsotach)->cphi()->at(m_currentQuadrant),
      m_stormData->coriolis(),
      m_stormData->isotach(m_currentIsotach)
          ->quadrantVr()
          ->at(m_currentQuadrant));

#ifdef GAHM_USE_ASWIP_SOLVER
  constexpr double accuracy = generate_accuracy();
  constexpr boost::uintmax_t itmax = 400;
  boost::uintmax_t it = itmax;

  double result = boost::math::tools::newton_raphson_iterate(
      vortex_function, m_stormData->radiusMaxWinds(),
      Vortex::default_inner_radius(), Vortex::default_outer_radius(),
      generate_digits_accuracy(), it);

  if (it < itmax) {
    Logging::log("Newton-Raphson solver converged in " + std::to_string(it) +
                 " iterations.");
    std::cout << result << std::endl;
    return {0, 0, result};
  } else {
    Logging::log("Newton-Raphson solver failed to converge.");
    return {0, 0, -std::numeric_limits<double>::max()};
  }
#else
  constexpr size_t itmax = 400;
  double fa, fprimea;
  std::tie(fa, fprimea) = vortex_function(aa);

  for (size_t it = 0; it < itmax; ++it) {
    bb = aa + (it + 1) * zoom_window;
    double fb, fprimeb;
    std::tie(fb, fprimeb) = vortex_function(bb);

    if (fa * fb < 0.0 || (std::abs(fb) > std::abs(fa))) {
      if (std::abs(fb) > std::abs(fa)) {
        return Root(aa, bb, aa);
      } else {
        return Root(bb, aa, bb);
      }
    }

    aa = bb;
    fa = fb;
  }
  Logging::log("Bracket solver failed to converge");
  return Root(aa, bb, -1.0);
#endif
}

template <Vortex::VhType vh>
std::pair<double, double> Vortex::getVh(double aa) const {
  if (vh == Vortex::VhType::VhNoCori) {
    return this->vhNoCori(aa);
  } else if (vh == Vortex::VhType::VhWithCori) {
    // return this->vhWithCori(aa);
    return {0, 0};
  } else if (vh == Vortex::VhType::VhWithCoriFull) {
    return this->vgWithCoriFull(aa);
  }
}

std::pair<double, double> Vortex::vhNoCori(double r) const {
  //...Input values
  const double vr =
      m_stormData->cisotach(m_currentIsotach)->cvmaxBl()->at(m_currentQuadrant);
  const double b = m_stormData->cisotach(m_currentIsotach)
                       ->chollandB()
                       ->at(m_currentQuadrant);
  const double rq = m_stormData->cisotach(m_currentIsotach)
                        ->cisotachRadius()
                        ->at(m_currentQuadrant);
  const double vmax = m_stormData->vmax();

  //...Precompute factors used multiple times
  const double vmaxsquared = vmax * vmax;
  const double alpha = r / rq;
  const double alphab = std::pow(alpha, b);
  const double expOneMinusAlphaB = std::exp(1.0 - alphab);

  //...Compute value of function
  const double f = std::sqrt(vmaxsquared * expOneMinusAlphaB * alphab) - vr;

  //...Compute components of derivative
  const double fprime_a = b * r * vmaxsquared * expOneMinusAlphaB / (rq * rq);
  const double fprime_b =
      b * r * vmaxsquared * expOneMinusAlphaB * alphab * std::pow(alpha, b - 1);
  const double fprime_c =
      2.0 * std::sqrt(vmaxsquared * expOneMinusAlphaB * alphab);

  //...Compute derivative
  const double fprime = -(fprime_a - fprime_b) / fprime_c;

  return std::make_pair(f, fprime);
}
