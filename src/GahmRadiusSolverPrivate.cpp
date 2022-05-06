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
#include "GahmRadiusSolverPrivate.h"

using namespace Gahm;

GahmRadiusSolverPrivate::GahmRadiusSolverPrivate(double isotachRadius,
                                                 double isotachSpeed,
                                                 double vmax, double fc,
                                                 double bg)
    : m_isotachRadius(isotachRadius),
      m_vmax(vmax),
      m_fc(fc),
      m_isotachSpeed(isotachSpeed),
      m_bg(bg) {}

/**
 * Function to solve the Vg function and first derivative for the
 * Newton-Raphson solver
 * @param rmax radius to max winds to solve at
 * @return std::pair containing the solution to Vg and Vg'
 */
std::pair<double, double> GahmRadiusSolverPrivate::operator()(
    const double &rmax) const {
  const double vg = GahmRadiusSolverPrivate::f(rmax, m_vmax, m_isotachSpeed,
                                               m_isotachRadius, m_fc, m_bg);
  const double vgp = GahmRadiusSolverPrivate::f_prime(
      rmax, m_vmax, m_isotachRadius, m_fc, m_bg);
  return {vg, vgp};
}

/**
 * Set the GAHM Holland B parameter
 * @param bg GAHM Holland B
 */
void GahmRadiusSolverPrivate::setBg(double bg) { m_bg = bg; }

/**
 * Returns the solvers current GAHM Holland B
 * @return GAHM B
 */
double GahmRadiusSolverPrivate::bg() const { return m_bg; }

/**
 * Function to solve \f$V_g(r)\f$
 * \f[
 * V_g(r) =
 * \sqrt{\frac{f^2r^2}{4}+{v_m}^2{e}^\beta\left(\gamma+1\right)\alpha^{b_g}}-\frac{fr}{2}
 * \f]
 * where:
 * \f$\alpha = \frac{r_m}{r}\f$
 * \f$\beta  = -\phi \left(\alpha^{b_g}-1\right)\f$
 * \f$\gamma = \frac{fr_m}{v_m}\f$
 *
 * @param rmax radius to max winds
 * @param vmax maximum wind speed
 * @param isotach_speed speed of the current isotach
 * @param isotach_radius radius of the current isotach
 * @param fc coriolis force
 * @param bg GAHM Holland B
 * @return Solution to gradient wind
 */
double GahmRadiusSolverPrivate::f(double rmax, double vmax,
                                  double isotach_speed, double isotach_radius,
                                  double fc, double bg) {
  const auto phi = GahmRadiusSolverPrivate::computePhi(vmax, rmax, bg, fc);
  const auto ro = Atmospheric::rossbyNumber(vmax, rmax, fc);
  const auto rmbg = std::pow(rmax / isotach_radius, bg);
  const auto vg = (std::sqrt(std::pow(vmax, 2.0) * (1.0 + 1.0 / ro) *
                                 std::exp(phi * (1.0 - rmbg)) * rmbg +
                             std::pow((isotach_radius * fc) / 2.0, 2.0)) -
                   (isotach_radius * fc) / 2.0) -
                  isotach_speed;
  return vg;
}

/**
 * Function to solve \f$V_g^{\prime}(r)\f$
 * \f[ V_g^{\prime}(r) = \frac{f v_m
 * e^{\beta}{\alpha}^{b_g}+\frac{b_g{v_m}^2{e}^{\beta}\left(\gamma+1\right)\alpha^{b_g-1}}{r}-\frac{b_g\phi
 * {v_m}^2{e}^{\beta}\left(\gamma+1\right)\alpha^{b_g}\alpha^{b_g-1}}{r}}{2\sqrt{\frac{f^2r^2}{4}+{v_m}^2{e}^{\beta}\left(\gamma+1\right)\alpha^{b_g}}}
 * \f]
 * * where:
 * \f$\alpha = \frac{r_m}{r}\f$
 * \f$\beta  = -\phi \left(\alpha^{b_g}-1\right)\f$
 * \f$\gamma = \frac{fr_m}{v_m}\f$
 *
 * @param rmax Radius to max winds
 * @param vmax Maximum wind speed
 * @param isotach_radius radius of the current isotach
 * @param fc coriolis force
 * @param bg GAHM Holland B parameter
 * @return Solution to first derivative
 */
double GahmRadiusSolverPrivate::f_prime(double rmax, double vmax,
                                        double isotach_radius, double fc,
                                        double bg) {
  const auto phi = GahmRadiusSolverPrivate::computePhi(vmax, rmax, bg, fc);
  const auto f3 = std::pow(rmax / isotach_radius, bg);
  const auto f4 = std::pow(rmax / isotach_radius, bg - 1.0);
  const auto f1 = std::exp(-phi * (f3 - 1));
  const auto f2 = ((fc * rmax) / vmax) + 1;
  const auto a = fc * vmax * f1 * f3;
  const auto b = (bg * vmax * vmax * f1 * f2 * f4) / isotach_radius;
  const auto c = (bg * phi * vmax * vmax * f1 * f2 * f3 * f4) / isotach_radius;
  const auto d =
      2.0 * std::sqrt((fc * fc * isotach_radius * isotach_radius) / 4.0 +
                      (vmax * vmax * f1 * f2 * f3));
  const auto vgp = (a + b - c) / d;
  return vgp;
}