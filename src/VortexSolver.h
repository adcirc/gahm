#ifndef VORTEXSOLVER_H
#define VORTEXSOLVER_H

#include <utility>

class VortexSolver {
 public:
  VortexSolver(double rmax, double vmax, double bg, double phi, double fc,
               double vr);

  std::pair<double, double> operator()(const double &r) const;

 private:
  double m_rmax;
  double m_vmax;
  double m_bg;
  double m_phi;
  double m_fc;
  double m_vr;

  double f(double r) const;
  double fprime(double r) const;
};

#endif  // VORTEXSOLVER_H
