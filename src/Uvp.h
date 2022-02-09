//
// Created by Zach Cobell on 12/13/21.
//

#ifndef GAHM_SRC_UVP_H_
#define GAHM_SRC_UVP_H_

namespace Gahm {

class Uvp {
 public:
  Uvp() : m_u(0.0), m_v(0.0), m_p(Physical::backgroundPressure()) {}

  Uvp(double u, double v, double p) : m_u(u), m_v(v), m_p(p) {}

  constexpr double u() const { return m_u; }
  constexpr double v() const { return m_v; }
  constexpr double p() const { return m_p; }

  void set(double u, double v, double p) {
    m_u = u;
    m_v = v;
    m_p = p;
  }

  void setU(double u) { m_u = u; }

  void setV(double v) { m_v = v; }

  void setP(double p) { m_p = p; }

 private:
  double m_u;
  double m_v;
  double m_p;
};

}  // namespace Gahm
#endif  // GAHM_SRC_UVP_H_
