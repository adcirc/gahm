//
// Created by Zach Cobell on 5/10/22.
//

#include "Vortex.h"

#include <utility>

using namespace Gahm;

Vortex::Vortex(std::string filename) : m_atcf(std::move(filename)) {
  this->m_atcf.read();
}

const Gahm::Atcf& Vortex::atcf() const { return m_atcf; }

Gahm::Atcf& Vortex::atcf() { return m_atcf; }

double Vortex::gamma(const StormPosition& position, const double vmax,
                     const double vr, const double u_vr, const double v_vr,
                     const double u_transit, const double v_transit) {
  constexpr double windReductionSquared =
      Physical::windReduction() * Physical::windReduction();

  const double gamma =
      ((2.0 * u_vr * u_transit + 2.0 * v_vr * v_transit) -
       std::sqrt(
           std::pow(2.0 * u_vr * u_transit + 2.0 * v_vr * v_transit, 2.0) -
           4.0 *
               (std::pow(position.transitSpeed(), 2.0) -
                std::pow(vmax, 2.0) * windReductionSquared) *
               std::pow(vr, 2.0))) /
      (2.0 * (std::pow(position.transitSpeed(), 2.0) -
              std::pow(vmax, 2.0) * windReductionSquared));
  return std::max(std::min(gamma, 1.0), 0.0);
}