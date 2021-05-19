#ifndef VORTEXSOLVER_H
#define VORTEXSOLVER_H

#include <utility>
#include "AtcfLine.h"

enum VortexSolverType { Derivative, NoDerivative };

class VortexSolverInternal {
 public:
  VortexSolverInternal(double rmax, double vmax, double bg, double phi,
                       double fc, double vr);

  template <VortexSolverType T, std::enable_if_t<T == Derivative, bool> = true>
  std::pair<double, double> operator()(const double &r) const {
    return std::make_pair(f(r), fprime(r));
  }

  template <VortexSolverType T,
            std::enable_if_t<T == NoDerivative, bool> = true>
  double operator()(const double &r) const {
    return f(r);
  }

 private:
  double m_rmax;
  double m_vmax;
  double m_bg;
  double m_phi;
  double m_fc;
  double m_vr;

  double f(const double &r) const;
  double fprime(const double &r) const;
};

template <VortexSolverType T>
class VortexSolver {
 public:
  VortexSolver(double rmax, double vmax, double bg, double phi, double fc,
               double vr)
      : m_data(rmax, vmax, bg, phi, fc, vr) {}

  VortexSolver(const AtcfLine *data, const size_t currentIsotach,
               const size_t current_quadrant)
      : m_data(
            data->radiusMaxWinds(),
            data->cisotach(currentIsotach)->cvmaxBl()->at(current_quadrant),
            data->cisotach(currentIsotach)->chollandB()->at(current_quadrant),
            data->cisotach(currentIsotach)->cphi()->at(current_quadrant),
            data->coriolis(),
            data->cisotach(currentIsotach)
                ->cquadrantVr()
                ->at(current_quadrant)) {}

  auto operator()(const double &r) { return m_data.operator()<T>(r); }

 private:
  VortexSolverInternal m_data;
};

#endif  // VORTEXSOLVER_H
