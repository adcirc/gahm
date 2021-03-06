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

#include <algorithm>
#include <cassert>
#include <utility>

#include "Interpolation.h"
#include "Logging.h"
#include "Physical.h"
#include "VortexSolver.h"

using namespace Gahm;

Vortex::Vortex(AtcfLine *atcf, std::shared_ptr<Assumptions> assumptions)
    : m_stormData(atcf),
      m_assumptions(std::move(assumptions)),
      m_currentQuadrant(0),
      m_currentIsotach(0),
      m_currentRecord(0) {
  if (!m_assumptions) m_assumptions = std::make_shared<Assumptions>();
}

Vortex::Vortex(AtcfLine *atcf, const size_t currentRecord,
               const size_t currentIsotach,
               std::shared_ptr<Assumptions> assumptions)
    : m_currentQuadrant(0),
      m_currentIsotach(currentIsotach),
      m_currentRecord(currentRecord),
      m_stormData(atcf),
      m_assumptions(std::move(assumptions)) {
  if (!m_assumptions) m_assumptions = std::make_shared<Assumptions>();
}

void Vortex::setStormData(AtcfLine *atcf) { m_stormData = atcf; }

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
                         const double rho) {
  constexpr double km2m = Units::convert(Units::Kilometer, Units::Meter);
  constexpr double mb2pa = Units::convert(Units::Millibar, Units::Pascal);
  return (std::pow(vmaxBoundaryLayer, 2.0) +
          vmaxBoundaryLayer * radiusToMaxWinds * km2m * coriolis) *
         rho * std::exp(phi) / (phi * (dp * mb2pa));
}

/**
 * Compute the Phi parameter in the GAHM
 * @param vmaxBoundaryLayer max wind speed for stationary storm at top of
 * boundary layer
 * @param radiusToMaxWinds radius from storm center to vmaxBoundaryLayer
 * @param b GAHM b_g parameter, similar to Holland's B parameter
 * @param coriolis Coriolis parameter
 * @return phi
 */
constexpr double Vortex::computePhi(const double vmaxBoundaryLayer,
                                    const double radiusToMaxWinds,
                                    const double b, const double coriolis) {
  constexpr double km2m = Units::convert(Units::Kilometer, Units::Meter);
  return 1.0 + (vmaxBoundaryLayer * radiusToMaxWinds * km2m * coriolis) /
                   (b * vmaxBoundaryLayer * vmaxBoundaryLayer +
                    vmaxBoundaryLayer * radiusToMaxWinds * km2m * coriolis);
}

std::tuple<double, double> Vortex::computeBandPhi(double vmax, double root,
                                                  double b, double cor,
                                                  double dp) {
  double phi_new = Vortex::computePhi(vmax, root, b, cor);
  double b_new =
      Vortex::computeBg(vmax, root, phi_new, dp, cor, Physical::rhoAir());
  return std::make_tuple(phi_new, b_new);
}

Vortex::ShapeTerms Vortex::iterateShapeTerms(const double root) const {
  constexpr double accuracy = 0.01;

  double vmax =
      m_stormData->isotach(m_currentIsotach).vmaxBl().at(m_currentQuadrant);
  double b =
      m_stormData->isotach(m_currentIsotach).hollandB().at(m_currentQuadrant);
  double cor = m_stormData->coriolis();
  double dp = Physical::backgroundPressure() - m_stormData->centralPressure();

  double phi_new, b_new;
  std::tie(phi_new, b_new) = Vortex::computeBandPhi(vmax, root, b, cor, dp);

  for (auto i = 0; i < m_max_it; ++i) {
    double b_new1 = b_new;

    std::tie(phi_new, b_new) =
        Vortex::computeBandPhi(vmax, root, b_new1, cor, dp);

    if (std::abs(b_new1 - b_new) < accuracy) {
      Logging::debug("Shape iteration converged in " + std::to_string(i + 1) +
                     " iterations.");
      return {b_new, phi_new, true};
    }
  }
  return {b_new, phi_new, false};
}

double Vortex::iterateRadius() const {
  constexpr size_t max_root_iterations = 3;
  constexpr double zoom_size = 0.01;
  double r1 = Vortex::default_inner_radius();
  double r2 = Vortex::default_outer_radius();
  Root r(r1, r2, 0.0);
  double dr = 1.0;
  for (auto root_iteration = 0; root_iteration < max_root_iterations;
       ++root_iteration) {
    r = this->findRoot(r1, r2, dr);
    r1 = r.left;
    r2 = r.right;
    dr *= zoom_size;
  }
  return r.root;
}

int Vortex::computeRadiusToWind() {
  for (auto quad = 0; quad < 4; quad++) {
    this->setCurrentQuadrant(quad);

    for (size_t it = 0; it < m_max_it; ++it) {
      double root = this->iterateRadius();
      if (root < 0.0) {
        root = m_stormData->isotach(m_currentIsotach).isotachRadius().at(quad);
        m_stormData->isotach(m_currentIsotach).rmax().set(quad, root);
        m_assumptions->add(generate_assumption(
            Assumption::MAJOR,
            "Root could not be found for record " +
                std::to_string(m_currentRecord) +
                ", Isotach: " + std::to_string(m_currentIsotach) +
                ", Quadrant: " + std::to_string(quad)));
      } else {
        m_stormData->isotach(m_currentIsotach).rmax().set(quad, root);
      }

      const ShapeTerms st = this->iterateShapeTerms(root);
      if (st.converged) {
        Logging::debug("Radius iteration converged in " +
                       std::to_string(it + 1) + " iterations.");
        m_stormData->isotach(m_currentIsotach).hollandB().set(quad, st.b);
        m_stormData->isotach(m_currentIsotach).phi().set(quad, st.phi);
        break;
      }
      if (it == m_max_it - 1 && !st.converged) {
        Logging::debug("Radius iteration failed to converged.");
      }
    }
  }
  return 0;
}

std::pair<double, double> Vortex::rotateWinds(const double x, const double y,
                                              double angle,
                                              const double latitude) noexcept {
  if (latitude < 0.0) angle *= -1.0;
  const double cosA = gahm_cos(angle);
  const double sinA = gahm_sin(angle);
  return std::make_pair(x * cosA - y * sinA, x * sinA + y * cosA);
}

size_t Vortex::currentRecord() const { return m_currentRecord; }

void Vortex::setCurrentRecord(const size_t &currentRecord) {
  m_currentRecord = currentRecord;
}

unsigned Vortex::currentIsotach() const { return m_currentIsotach; }

void Vortex::setCurrentIsotach(const unsigned currentIsotach) {
  m_currentIsotach = currentIsotach;
}

unsigned Vortex::currentQuadrant() const { return m_currentQuadrant; }

void Vortex::setCurrentQuadrant(unsigned quad) { m_currentQuadrant = quad; }

std::pair<int, double> Vortex::getBaseQuadrant(double angle) {
  auto angle2 = angle + Constants::quarterpi();
  if(angle2 > Constants::twopi()) angle2 -= Constants::twopi();
  auto quad = std::fmod(angle2 / Constants::halfpi(), 4);
  const double rem = angle2 - quad * Constants::halfpi();
  return {quad, rem};
}

/**
 * Computes the estimate of the Rossby number in a tropical cyclone
 * @param vmaxBoundaryLayer max wind speed for stationary storm at top of
 * boundary layer
 * @param radiusToMaxWinds radius from storm center to vmaxBoundaryLayer
 * @param coriolis Coriolis parameter
 * @return estimate to Rossby number
 */
constexpr double Vortex::rossbyNumber(const double vmaxBoundaryLayer,
                                      const double radiusToMaxWinds,
                                      const double coriolis) {
  assert(coriolis > 0.0);
  assert(radiusToMaxWinds > 0.0);
  return vmaxBoundaryLayer / (coriolis * radiusToMaxWinds);
}

Vortex::Root Vortex::findRoot(double aa, double bb,
                              const double zoom_window) const {
  constexpr size_t max_iterations = 400;
  auto vortex_function = VortexSolver<VortexSolverType::NoDerivative>(
      m_stormData, m_currentIsotach, m_currentQuadrant);
  double fa = vortex_function(aa);

  for (auto it = 0; it < max_iterations; ++it) {
    bb = aa + static_cast<double>(it + 1) * zoom_window;
    double fb = vortex_function(bb);

    if (fa * fb < 0.0 || (std::abs(fb) > std::abs(fa))) {
      Logging::debug("Bracket solver converged in " + std::to_string(it + 1) +
                     " iterations.");
      if (std::abs(fb) > std::abs(fa)) {
        return {aa, bb, aa};
      } else {
        return {bb, aa, bb};
      }
    }
    aa = bb;
    fa = fb;
  }
  Logging::debug("Bracket solver failed to converge in" +
                 std::to_string(max_iterations));
  return {aa, bb, -1.0};
}

ParameterPack Vortex::getParameters(const double angle,
                                    const double distance) const {
  constexpr double deg2rad = Units::convert(Units::Degree, Units::Radian);
  constexpr double m2km = Units::convert(Units::Meter, Units::Kilometer);
  constexpr double angle_1 = deg2rad;
  constexpr double angle_89 = 89.0 * deg2rad;
  constexpr double angle_90 = 90.0 * deg2rad;
  const double dis = distance * m2km;

  int base_quadrant = -2;
  double delta_angle = 0.0;
  std::tie(base_quadrant, delta_angle) = Vortex::getBaseQuadrant(angle);

  if (delta_angle < angle_1) {
    return this->interpolateParameters(base_quadrant - 1, dis, delta_angle);
  } else if (delta_angle > angle_89) {
    return this->interpolateParameters(base_quadrant, dis, delta_angle);
  } else {
    auto pack1 =
        this->interpolateParameters(base_quadrant - 1, dis, delta_angle);
    auto pack2 = this->interpolateParameters(base_quadrant, dis, delta_angle);
    auto d1 = 1.0 / (delta_angle * delta_angle);
    auto d2 = 1.0 / ((angle_90 - delta_angle) * (angle_90 - delta_angle));

    double rmax = Interpolation::quadrantInterp(
        d1, d2, pack1.radiusToMaxWinds(), pack2.radiusToMaxWinds());
    double rmaxtrue = Interpolation::quadrantInterp(
        d1, d2, pack1.radiusToMaxWindsTrue(), pack2.radiusToMaxWindsTrue());
    double vmaxbl = Interpolation::quadrantInterp(
        d1, d2, pack1.vmaxBoundaryLayer(), pack2.vmaxBoundaryLayer());
    double b = Interpolation::quadrantInterp(d1, d2, pack1.hollandB(),
                                             pack2.hollandB());
    return {vmaxbl, rmax, rmaxtrue, b};
  }
}

ParameterPack Vortex::interpolateParameters(int quad, double distance,
                                            double angle) const {
  const auto radii = m_stormData->isotachRadii(quad);

  if (radii->empty()) {
    return {m_stormData->vmaxBl(), m_stormData->radiusMaxWinds(),
            m_stormData->radiusMaxWinds(), m_stormData->hollandB()};
  } else if (distance >= radii->front()) {
    return m_stormData->isotach(0).parameterPack(quad);
  } else if (distance <= radii->back()) {
    return m_stormData->isotach(radii->size() - 1).parameterPack(quad);
  } else {
    const auto it =
        std::upper_bound(radii->rbegin(), radii->rend(), distance).base();
    const auto pos1 = it - radii->begin();
    const auto pos2 = pos1 - 1;
    const auto r1 = *(it);
    const auto r2 = *(it - 1);
    const double fac = (distance - r1) / (r2 - r1);
    const auto p1 = m_stormData->isotach(pos1).parameterPack(quad);
    const auto p2 = m_stormData->isotach(pos2).parameterPack(quad);
    double vmbl = Interpolation::linearInterp(fac, p1.vmaxBoundaryLayer(),
                                              p2.vmaxBoundaryLayer());
    double rmax = Interpolation::linearInterp(fac, p1.radiusToMaxWinds(),
                                              p2.radiusToMaxWinds());
    double b = Interpolation::linearInterp(fac, p1.hollandB(), p2.hollandB());
    double rmaxtrue = Interpolation::linearInterp(
        fac, p1.radiusToMaxWindsTrue(), p2.radiusToMaxWindsTrue());
    return {vmbl, rmax, rmaxtrue, b};
  }
}

Date Vortex::datetime() const { return m_stormData->datetime(); }
