//
// Created by Zach Cobell on 5/8/22.
//

#include <fstream>

#include "Atcf.h"
#include "Atmospheric.h"
#include "ForecastPeriod.h"
#include "GahmEquations.h"
#include "GahmVortex.h"
#include "Interpolation.h"
#include "StormIsotach.h"
#include "UnitConversion.h"
#include "VortexSolution.h"
#include "WindGrid.h"

#define FMT_HEADER_ONLY
#include "fmt/core.h"

struct Params {
  double rmax;
  double vmax;
  double b;
  int isotach;
  int quadrant;
};

Params interpolateParameters(const Gahm::ForecastPeriod &snap, int isotach,
                             int quadrant, double distance, double angle,
                             double weight) {
  if (snap.nIsotachs() == 0 || isotach == -1) {
    return {snap.radiusToMaxWinds(), snap.maximumWindVelocity(),
            Gahm::Atmospheric::calcHollandB(snap.maximumWindVelocity(),
                                            snap.centralPressure(),
                                            snap.backgroundPressure()),
            isotach, quadrant};
  } else if (snap.nIsotachs() == 1 ||
             distance >=
                 snap.isotach(isotach).quadrant(quadrant).isotach_radius()) {
    return {snap.isotach(isotach).quadrant(quadrant).radius_to_max_winds(),
            snap.isotach(isotach).quadrant(quadrant).vmax_at_boundary_layer(),
            snap.isotach(isotach).quadrant(quadrant).gahm_holland_b(), isotach,
            quadrant};
  } else {
    const auto p1 = Params{
        snap.isotach(isotach).quadrant(quadrant).radius_to_max_winds(),
        snap.isotach(isotach).quadrant(quadrant).vmax_at_boundary_layer(),
        snap.isotach(isotach).quadrant(quadrant).gahm_holland_b()};
    const auto p2 = Params{
        snap.isotach(isotach - 1).quadrant(quadrant).radius_to_max_winds(),
        snap.isotach(isotach - 1).quadrant(quadrant).vmax_at_boundary_layer(),
        snap.isotach(isotach - 1).quadrant(quadrant).gahm_holland_b()};
    const auto rmax =
        Gahm::Interpolation::linearInterp(weight, p1.rmax, p2.rmax);
    const auto vmax =
        Gahm::Interpolation::linearInterp(weight, p1.vmax, p2.vmax);
    const auto b = Gahm::Interpolation::linearInterp(weight, p1.b, p2.b);
    return {rmax, vmax, b, isotach, quadrant};
  }
}

Params getStormParameters(const Gahm::ForecastPeriod &snap, double distance,
                          double azimuth, const Gahm::PositionWeights &w) {
  static constexpr double deg2rad =
      Gahm::Units::convert(Gahm::Units::Degree, Gahm::Units::Radian);
  static constexpr double angle_1 = 1.0 * deg2rad;
  static constexpr double angle_89 = 89.0 * deg2rad;

  if (w.delta_angle() < angle_1) {
    return interpolateParameters(snap, w.isotach(), w.quadrant() - 1, distance,
                                 azimuth, w.isotach_weight());
  } else if (w.delta_angle() > angle_89) {
    return interpolateParameters(snap, w.isotach(), w.quadrant(), distance,
                                 azimuth, w.isotach_weight());
  } else {
    auto p1 = interpolateParameters(snap, w.isotach(), w.quadrant() - 1,
                                    distance, azimuth, w.isotach_weight());
    auto p2 = interpolateParameters(snap, w.isotach(), w.quadrant(), distance,
                                    azimuth, w.isotach_weight());

    auto [angle_weight1, angle_weight2] = w.delta_angle_weights();
    double rmax = Gahm::Interpolation::quadrantInterp(
        angle_weight1, angle_weight2, p1.rmax, p2.rmax);
    double vmax = Gahm::Interpolation::quadrantInterp(
        angle_weight1, angle_weight2, p1.vmax, p2.vmax);
    double b = Gahm::Interpolation::quadrantInterp(angle_weight1, angle_weight2,
                                                   p1.b, p2.b);
    return {rmax, vmax, b};
  }
}

std::tuple<double, double> rotateWinds(double u, double v, double latitude) {
  static constexpr double deg2rad =
      Gahm::Units::convert(Gahm::Units::Degree, Gahm::Units::Radian);
  double dummy_fang = 15.5 * deg2rad;
  const auto which_way = [latitude]() {
    if (latitude < 0.0) {
      return -1.0;
    } else {
      return 1.0;
    }
  }();

  const auto a = which_way * dummy_fang;
  const auto cos_a = std::cos(a);
  const auto sin_a = std::sin(a);

  const auto ur = u * cos_a - v * sin_a;
  const auto vr = u * sin_a + v * cos_a;

  return {ur, vr};
}

std::tuple<double, double, double, int, int> uvp(
    const Gahm::ForecastPeriod &snap, double longitude, double latitude) {
  const auto [distance, azimuth] = snap.distanceAndAzimuth(
      longitude, latitude, snap.stormPosition().longitude(),
      snap.stormPosition().latitude());
  const auto p = snap.selectIsotachAndQuadrant(distance, azimuth);
  const auto s = getStormParameters(snap, distance, azimuth, p);

  double corio = Gahm::Physical::coriolis(latitude);
  double wind_speed = Gahm::GahmEquations::GahmFunction(s.rmax, s.vmax, 0,
                                                        distance, corio, s.b) *
                      Gahm::Physical::windReduction();
  double wind_speed_u = -wind_speed * std::cos(azimuth);
  double wind_speed_v = wind_speed * std::sin(azimuth);

  //...Rotate the winds for inflow
  auto [wind_speed_u_rotated, wind_speed_v_rotated] =
      rotateWinds(wind_speed_u, wind_speed_v, snap.stormPosition().latitude());

  //...Add in the storm translation speed
  auto [tx, ty] = snap.stormPosition().transitSpeedComponents();
  wind_speed_u_rotated += tx;
  wind_speed_v_rotated += ty;

  //...Convert from one minute to ten minute
  wind_speed_u_rotated *= Gahm::Physical::one2ten();
  wind_speed_v_rotated *= Gahm::Physical::one2ten();

  double phi = Gahm::GahmEquations::phi(s.vmax, s.rmax, s.b, corio);

  double pressure = Gahm::GahmEquations::GahmPressure(
      snap.centralPressure(), snap.backgroundPressure(), distance, s.rmax, s.b,
      phi);

  return {wind_speed_u_rotated, wind_speed_v_rotated, pressure, p.isotach(),
          p.quadrant()};
}

int main() {
  //  constexpr auto nmi2m =
  //      Gahm::Units::convert(Gahm::Units::NauticalMile, Gahm::Units::Meter);
  //  constexpr auto kt2ms =
  //      Gahm::Units::convert(Gahm::Units::Knot, Gahm::Units::MetersPerSecond);
  //  constexpr auto deg2rad =
  //      Gahm::Units::convert(Gahm::Units::Radian, Gahm::Units::Degree);
  //
  //  Gahm::Date d(2005, 1, 1);
  //  Gahm::ForecastPeriod snap(d, -89.0, 20.0, 4 * kt2ms, 45, 55 * nmi2m,
  //                            45 * kt2ms, 989, 1013.0);
  //
  //  Gahm::StormIsotach iso0(34 * kt2ms, {80 * nmi2m, 40 * nmi2m, 0, 50 *
  //  nmi2m}); snap.addIsotach(iso0); snap.computeRadiusToMaxWind();
  //
  //  double start_x = -100;
  //  double end_x = -70.0;
  //  double start_y = 10.0;
  //  double end_y = 30.0;
  //  double dx = 0.1;
  //
  //  std::ofstream output("uvp.txt");
  //
  //  for (auto x = start_x; x < end_x; x += dx) {
  //    for (auto y = start_y; y < end_y; y += dx) {
  //      auto [u, v, p, iso, quad] = uvp(snap, x, y);
  //      auto uv = std::sqrt(u * u + v * v);
  //      auto s = fmt::format(
  //          "{:0.8f},{:0.8f},{:0.4f},{:0.4f},{:0.4f},{:0.4f},{:d},{:d}\n", x,
  //          y, uv, u, v, p, iso, quad);
  //      output << s;
  //    }
  //  }
  //
  //  output.close();

  std::string f(
      "/Users/zcobell/Documents/Code/gahm/testing/test_files/bal122005.dat");
  Gahm::GahmVortex vortex(f);
  vortex.solve();

  Gahm::WindGrid wg(-100.0, 5.0, -60.0, 40.0, 0.25, 0.25);
  auto x = wg.xpoints();
  auto y = wg.ypoints();

  Gahm::VortexSolution sln(&vortex, x, y);

  auto start = vortex.atcf().data().begin()->date().toSeconds();
  auto end = (vortex.atcf().data().end() - 1)->date().toSeconds();

  for (size_t i = start - 3600; i <= end + 3600; i += 3600) {
    auto d = Gahm::Date::fromSeconds(i);
    auto w = sln.get(d);
    std::cout << w.u().size() << std::endl;
  }

  return 0;
}