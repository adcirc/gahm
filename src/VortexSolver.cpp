#include "VortexSolver.h"
#include <cmath>
#include <iostream>
#include "Physical.h"

VortexSolverInternal::VortexSolverInternal(double rmax, double vmax, double bg,
                                           double phi, double fc, double vr)
    : m_rmax(rmax), m_vmax(vmax), m_bg(bg), m_phi(phi), m_fc(fc), m_vr(vr) {}

double VortexSolverInternal::f(const double &r) const {
  //...Reused variables
  const double vmaxSq = std::pow(m_vmax, 2.0);
  const double rOverRmaxBg = std::pow(r / m_rmax, m_bg);
  const double expPhiOneMinusROverRmaxBg =
      std::exp(m_phi * (1.0 - rOverRmaxBg));

  //...Intermediate
  const double fa = vmaxSq + m_vmax * r * Physical::km2m() * m_fc;
  const double fb = rOverRmaxBg * expPhiOneMinusROverRmaxBg;
  const double fc = std::pow((m_rmax * Physical::km2m() * m_fc) / 2.0, 2.0);
  const double fd = (Physical::km2m() * m_rmax * m_fc) / 2.0;

  return std::sqrt(fa * fb + fc) - fd - m_vr;
}

double VortexSolverInternal::fprime(const double &r) const {
  constexpr double dx = 0.001;
  double f0 = f(r - dx);
  double f1 = f(r + dx);
  return (f1 - f0) / dx;
}
