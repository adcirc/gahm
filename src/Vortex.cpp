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
#include "boost/cstdint.hpp"
#include "boost/math/tools/roots.hpp"

Vortex::Vortex() : m_stormData(nullptr) {}

void Vortex::setStormData(const AtcfLine *atcf) { m_stormData = atcf; }

std::pair<double, double> Vortex::rotateWinds(const double x, const double y,
                                              const double angle,
                                              const double whichWay) noexcept {
  double a = Physical::deg2rad() * angle;
  if (whichWay >= 0.0) a *= -1.0;
  const double cosA = std::cos(a);
  const double sinA = std::sin(a);
  return {x * cosA - y * sinA, x * sinA + y * cosA};
}

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

template <Vortex::VhType vh>
double Vortex::findRoot(const double x1, const double dx, double &aa,
                        double &bb) const noexcept {
#ifndef GAHM_USE_ASWIP_SOLVER
  constexpr boost::uintmax_t itmax = 400;
  constexpr double innerRadius = 1.0;
  constexpr double outerRadius = 500.0;
  boost::uintmax_t it = itmax;
  constexpr int digits =
      std::numeric_limits<double>::digits;  // Maximum possible binary digits
                                            // accuracy for type double.
  constexpr int get_digits = static_cast<int>(
      digits * 0.6);  // Accuracy doubles with each step, so stop when we have
                      // just over half the digits correct.
  double result = boost::math::tools::newton_raphson_iterate(
      Vortex::getVh<vh>(x1), aa, innerRadius, outerRadius, get_digits, it);

  if (it < itmax) {
    Logging::log("Newton-Raphson solver converged in " + std::to_string(it) +
                 " iterations.");
    return result;
  } else {
    Logging::log("Newton-Raphson solver failed to converge.");
    return -std::numeric_limits<double>::max();
  }
#else
  constexpr size_t itmax = 400;
  aa = x1;
  double fa = this->getVh<vh>(aa);

  for (size_t it = 0; it < itmax; ++it) {
    bb = x1 + it * dx;
    double fb = this->getVh<vh>(bb);
    if (fa * fb < 0.0 || std::abs(fb) > std::abs(fa)) {
      if (std::abs(fb) > std::abs(fa)) {
        return aa;
      } else {
        return bb;
      }
    }
    aa = bb;
    fa = fb;
  }
  Logging::log("Bracket solver failed to converge");
  return -std::numeric_limits<double>::max();
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

std::pair<double, double> Vortex::vgWithCoriFull(double r) const {
  //...Input parameters

  // clang-format off
  const double rmax = m_stormData->radiusMaxWinds();
  const double vmax = m_stormData->cisotach(m_currentIsotach)->cvmaxBl()->at(m_currentQuadrant);
  const double bg = m_stormData->cisotach(m_currentIsotach)->chollandB()->at(m_currentQuadrant);
  const double phi = m_stormData->cisotach(m_currentIsotach)->cphi()->at(m_currentQuadrant);
  const double fc = m_stormData->coriolis();
  // clang-format on

  //...Intermediate values
  const double alpha = rmax / r;
  const double beta = phi * (1.0 - std::pow(alpha, bg));
  const double r0 = vmax / (rmax * fc);
  const double kappa = (1.0 / r0) + 1.0;

  //...Reused values
  const double vmaxsq = vmax * vmax;
  const double rmaxsq = rmax * rmax;
  const double expbeta = std::exp(beta);

  //...f(r)
  const double f =
      std::sqrt(vmaxsq * kappa * expbeta * alpha + (r * fc) / 2.0) -
      (r * fc) / 2.0;

  //...f'(r)
  const double fprime_a = (fc / 2.0);
  const double fprime_b = (r * vmaxsq * expbeta * kappa) / (r * r);
  const double fprime_c = bg * rmaxsq * vmaxsq * phi * expbeta *
                          std::pow(alpha, bg - 1.0) / (r * r * r);
  const double fprime_d =
      2.0 * std::sqrt((fc * r) / 2.0 + (rmax * vmaxsq * expbeta * kappa) / r);

  const double fprime = (fprime_a - fprime_b + fprime_c) / fprime_d - fprime_a;

  return std::make_pair(f, fprime);
}
