#include "VortexSolver.h"
#include <cmath>
#include <iostream>
#include "Physical.h"

VortexSolver::VortexSolver(double rmax, double vmax, double bg, double phi,
                           double fc, double vr)
    : m_rmax(rmax), m_vmax(vmax), m_bg(bg), m_phi(phi), m_fc(fc), m_vr(vr) {}

// std::pair<double, double> VortexSolver::operator()(const double &r) const {
//  //...Intermediate values
//  const double alpha = m_rmax / r;
//  const double beta = m_phi * (1.0 - std::pow(alpha, m_bg));
//  const double r0 = m_vmax / (m_rmax * m_fc);
//  const double kappa = (1.0 / r0) + 1.0;

//  //...Reused values
//  const double vmaxsq = m_vmax * m_vmax;
//  const double rmaxsq = m_rmax * m_rmax;
//  const double expbeta = std::exp(beta);

//  //...f(r)
//  const double f =
//      std::sqrt(vmaxsq * kappa * expbeta * alpha + (r * m_fc) / 2.0) -
//      (r * m_fc) / 2.0;

//  //...f'(r)
//  const double fprime_a = (m_fc / 2.0);
//  const double fprime_b = (r * vmaxsq * expbeta * kappa) / (r * r);
//  const double fprime_c = m_bg * rmaxsq * vmaxsq * m_phi * expbeta *
//                          std::pow(alpha, m_bg - 1.0) / (r * r * r);
//  const double fprime_d =
//      2.0 *
//      std::sqrt((m_fc * r) / 2.0 + (m_rmax * vmaxsq * expbeta * kappa) / r);

//  const double fprime = (fprime_a - fprime_b + fprime_c) / fprime_d -
//  fprime_a;

//  return std::make_pair(f, fprime);
//}

double VortexSolver::f(double r) const {
  double f =
      std::sqrt((std::pow(m_vmax, 2.0) + m_vmax * r * 1000.0 * m_fc) *
                    std::pow(r / m_rmax, m_bg) *
                    std::exp(m_phi * (1.0 - std::pow(r / m_rmax, m_bg))) +
                std::pow((m_rmax * 1000 * m_fc) / 2.0, 2.0)) -
      (1000 * m_rmax * m_fc) / 2.0;
  std::cout << "VMX: " << m_vmax * Physical::ms2kt() << " FC: " << m_fc
            << " RMX: " << m_rmax * Physical::km2nmi()
            << " BG: " << m_bg << std::endl;
  std::cout << "RET: " << f * Physical::ms2kt() << " "
            << m_vr * Physical::ms2kt() << " " << (f - m_vr) * Physical::ms2kt()
            << std::endl;
  f -= m_vr;
  return f;
}

double VortexSolver::fprime(double r) const {
  constexpr double dx = 0.01;
  double f0 = f(r - dx);
  double f1 = f(r + dx);
  return (f1 - f0) / dx;
}

std::pair<double, double> VortexSolver::operator()(const double &r) const {
  return {f(r), fprime(r)};
}
