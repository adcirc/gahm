//
// Created by Zach Cobell on 5/10/22.
//

#include "GahmVortex.h"

#include "GahmSolver.h"

using namespace Gahm;

GahmVortex::GahmVortex(std::string filename) : Vortex(std::move(filename)) {}

void GahmVortex::solve() {
  for (auto &period : this->atcf().data()) {
    for (auto &isotach : period.isotachs()) {
      for (auto &quadrant : isotach.quadrants()) {
        GahmVortex::computeQuadrantSolution(period, isotach, quadrant);
      }
    }
  }
}

void GahmVortex::computeQuadrantSolution(const ForecastPeriod &period,
                                         const StormIsotach &isotach,
                                         StormQuadrant &quadrant) {
  constexpr double angle_15p5 =
      15.5 * Units::convert(Units::Degree, Units::Radian);
  const auto vr = isotach.windSpeed();
  const auto u_vr =
      vr * std::cos(Constants::quadrantAngle(quadrant.index()) + angle_15p5);
  const auto v_vr =
      vr * std::sin(Constants::quadrantAngle(quadrant.index()) + angle_15p5);
  const auto [u_transit, v_transit] =
      period.stormPosition().transitSpeedComponents();
  const auto gamma =
      Vortex::gamma(period.stormPosition(), period.vmaxAtBoundaryLayer(), vr,
                    u_vr, v_vr, u_transit, v_transit);

  quadrant.set_isotach_speed_at_boundary_layer(
      std::sqrt(std::pow(u_vr - gamma * u_transit, 2.0) +
                std::pow(v_vr - gamma * v_transit, 2.0)) /
      Physical::windReduction());

  GahmSolver g(quadrant.isotach_radius(),
               quadrant.isotach_speed_at_boundary_layer(),
               period.vmaxAtBoundaryLayer(), period.centralPressure(),
               period.backgroundPressure(), period.stormPosition().latitude());
  g.solve();
  quadrant.set_radius_to_max_winds(g.rmax());
  quadrant.set_gahm_holland_b(g.bg());
}
