//
// Created by Zach Cobell on 6/15/21.
//

#ifndef GAHM_SRC_GAHMSOLUTIONPOINT_H_
#define GAHM_SRC_GAHMSOLUTIONPOINT_H_

#include "Constants.h"

class GahmSolutionPoint {
 public:
  GahmSolutionPoint()
      : m_u(0.0), m_v(0.0), m_p(Constants::backgroundPressure()) {}

  GahmSolutionPoint(double u, double v, double p) : m_u(u), m_v(v), m_p(p) {}

  double u() const { return m_u; }
  void setU(double u) { m_u = u; }

  double v() const { return m_v; }
  void setV(double v) { m_v = v; }

  double p() const { return m_p; }
  void setP(double p) { m_p = p; }

 private:
  double m_u;
  double m_v;
  double m_p;
};

#endif  // GAHM_SRC_GAHMSOLUTIONPOINT_H_
