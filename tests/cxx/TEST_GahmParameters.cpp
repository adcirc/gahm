//
// Created by Zach Cobell on 7/31/24.
//

#include <array>
#include <iostream>

#include "atcf/AtcfIO.h"
#include "atcf/AtcfPeriod.h"
#include "catch2/catch_approx.hpp"
#include "catch2/catch_test_macros.hpp"
#include "datatypes/Datetime.h"
#include "datatypes/Point.h"
#include "gahm/GahmParameters.h"
#include "physical/Units.h"
#include "storm/StormTranslation.h"

using namespace Catch;

TEST_CASE("GahmParameters", "[GahmParameters]") {
  const Gahm::Types::Datetime previous_date("2018091318");
  const Gahm::Types::Datetime snap_date("2018091400");
  const auto dt =
      snap_date.seconds_since_epoch() - previous_date.seconds_since_epoch();

  REQUIRE(dt == 6 * 3600);

  constexpr auto mb2pa = Gahm::Physical::Units::convert(
      Gahm::Physical::Units::Millibar, Gahm::Physical::Units::Pascal);
  constexpr auto kt2ms = Gahm::Physical::Units::convert(
      Gahm::Physical::Units::Knot, Gahm::Physical::Units::MetersPerSecond);
  constexpr auto nmi2m = Gahm::Physical::Units::convert(
      Gahm::Physical::Units::NauticalMile, Gahm::Physical::Units::Meter);

  REQUIRE(mb2pa == Approx(100.0));
  REQUIRE(kt2ms == Approx(0.51444));

  const double p_c = 952.0 * mb2pa;
  const double p_bk = 1013.0 * mb2pa;
  const double v_max = 90.0 * kt2ms;
  const double r_max = 20.0 * nmi2m;
  const Gahm::Types::Point eye_location(-76.5, 34.0);
  const Gahm::Types::Point previous_eye_location(-76.0, 33.6);
  const Gahm::Storm::StormTranslation translation(eye_location,
                                                  previous_eye_location, dt);

  REQUIRE(p_c == Approx(95200.0));
  REQUIRE(p_bk == Approx(101300.0));
  REQUIRE(v_max == Approx(46.300107));
  REQUIRE(r_max == Approx(37039.907));
  REQUIRE(translation.speed() == Approx(2.96927));
  REQUIRE(translation.direction() == Approx(2.37762));
  REQUIRE(translation.unit_vector().u() == Approx(-0.72209));
  REQUIRE(translation.unit_vector().v() == Approx(0.691794));
  REQUIRE(translation.velocity().u() == Approx(-2.14409));
  REQUIRE(translation.velocity().v() == Approx(2.05413));

  const Gahm::Atcf::AtcfIO::TempIsotach i34(
      34.0 * kt2ms, std::array<double, 4>{170 * nmi2m, 150 * nmi2m, 130 * nmi2m,
                                          100 * nmi2m});
  const Gahm::Atcf::AtcfIO::TempIsotach i50(
      50.0 * kt2ms,
      std::array<double, 4>{100 * nmi2m, 80 * nmi2m, 80 * nmi2m, 70 * nmi2m});
  const Gahm::Atcf::AtcfIO::TempIsotach i64(
      64.0 * kt2ms,
      std::array<double, 4>{70 * nmi2m, 60 * nmi2m, 50 * nmi2m, 50 * nmi2m});

  const auto quadrants = Gahm::Atcf::AtcfIO::transpose_to_quadrants(
      eye_location.y(), {i34, i50, i64});

  REQUIRE(quadrants[0].isotach(0).wind_speed() == Approx(34.0 * kt2ms));
  REQUIRE(quadrants[0].isotach(0).radius() == Approx(170.0 * nmi2m));
  REQUIRE(quadrants[1].isotach(0).radius() == Approx(150.0 * nmi2m));
  REQUIRE(quadrants[2].isotach(0).radius() == Approx(130.0 * nmi2m));
  REQUIRE(quadrants[3].isotach(0).radius() == Approx(100.0 * nmi2m));

  REQUIRE(quadrants[1].isotach(1).wind_speed() == Approx(50.0 * kt2ms));
  REQUIRE(quadrants[0].isotach(1).radius() == Approx(100.0 * nmi2m));
  REQUIRE(quadrants[1].isotach(1).radius() == Approx(80.0 * nmi2m));
  REQUIRE(quadrants[2].isotach(1).radius() == Approx(80.0 * nmi2m));
  REQUIRE(quadrants[3].isotach(1).radius() == Approx(70.0 * nmi2m));

  REQUIRE(quadrants[3].isotach(2).wind_speed() == Approx(64.0 * kt2ms));
  REQUIRE(quadrants[0].isotach(2).radius() == Approx(70.0 * nmi2m));
  REQUIRE(quadrants[1].isotach(2).radius() == Approx(60.0 * nmi2m));
  REQUIRE(quadrants[2].isotach(2).radius() == Approx(50.0 * nmi2m));
  REQUIRE(quadrants[3].isotach(2).radius() == Approx(50.0 * nmi2m));

  Gahm::Atcf::AtcfPeriod snap(snap_date, p_c, p_bk, v_max, r_max, eye_location,
                              quadrants);
  snap.set_translation(translation);

  REQUIRE(snap.background_pressure() == Approx(101300.0));
  REQUIRE(snap.central_pressure() == Approx(95200.0));
  REQUIRE(snap.datetime().to_string() == "2018-09-14T00:00:00");
  REQUIRE(snap.v_max() == Approx(46.300107));
  REQUIRE(snap.r_max() == Approx(37039.907));
  REQUIRE(snap.eye_location().x() == Approx(-76.5));
  REQUIRE(snap.eye_location().y() == Approx(34.0));
  REQUIRE(snap.translation() == translation);

  const auto params_q1_i1 = Gahm::Solver::GahmParameters(
      snap.translation(), snap.eye_location(),
      snap.quadrant(0).unit_vector_tbl(), snap.central_pressure(),
      snap.background_pressure(), snap.v_max(),
      snap.quadrant(0).isotach(0).wind_speed(),
      snap.quadrant(0).isotach(0).radius());
  REQUIRE(params_q1_i1.radius_to_max_winds == Approx(47515.74));
  REQUIRE(params_q1_i1.gahm_b == Approx(1.05994));
  REQUIRE(params_q1_i1.gahm_phi == Approx(1.07886));
  REQUIRE(params_q1_i1.holland_b == Approx(0.96848));

  const auto params_q1_i2 = Gahm::Solver::GahmParameters(
      snap.translation(), snap.eye_location(),
      snap.quadrant(0).unit_vector_tbl(), snap.central_pressure(),
      snap.background_pressure(), snap.v_max(),
      snap.quadrant(0).isotach(1).wind_speed(),
      snap.quadrant(0).isotach(1).radius());
  REQUIRE(params_q1_i2.radius_to_max_winds == Approx(40147.84));
  REQUIRE(params_q1_i2.gahm_b == Approx(1.04546));
  REQUIRE(params_q1_i2.gahm_phi == Approx(1.06844));

  const auto params_q1_i3 = Gahm::Solver::GahmParameters(
      snap.translation(), snap.eye_location(),
      snap.quadrant(0).unit_vector_tbl(), snap.central_pressure(),
      snap.background_pressure(), snap.v_max(),
      snap.quadrant(0).isotach(2).wind_speed(),
      snap.quadrant(0).isotach(2).radius());
  REQUIRE(params_q1_i3.radius_to_max_winds == Approx(41464.0));
  REQUIRE(params_q1_i3.gahm_b == Approx(1.04804));
  REQUIRE(params_q1_i3.gahm_phi == Approx(1.07034));

  const auto params_q2_i1 = Gahm::Solver::GahmParameters(
      snap.translation(), snap.eye_location(),
      snap.quadrant(1).unit_vector_tbl(), snap.central_pressure(),
      snap.background_pressure(), snap.v_max(),
      snap.quadrant(1).isotach(0).wind_speed(),
      snap.quadrant(1).isotach(0).radius());
  REQUIRE(params_q2_i1.radius_to_max_winds == Approx(40758.26));
  REQUIRE(params_q2_i1.gahm_b == Approx(1.04666));
  REQUIRE(params_q2_i1.gahm_phi == Approx(1.06933));

  const auto params_q2_i2 = Gahm::Solver::GahmParameters(
      snap.translation(), snap.eye_location(),
      snap.quadrant(1).unit_vector_tbl(), snap.central_pressure(),
      snap.background_pressure(), snap.v_max(),
      snap.quadrant(1).isotach(1).wind_speed(),
      snap.quadrant(1).isotach(1).radius());
  REQUIRE(params_q2_i2.radius_to_max_winds == Approx(31506.76));
  REQUIRE(params_q2_i2.gahm_b == Approx(1.02858));
  REQUIRE(params_q2_i2.gahm_phi == Approx(1.05544));

  const auto params_q2_i3 = Gahm::Solver::GahmParameters(
      snap.translation(), snap.eye_location(),
      snap.quadrant(1).unit_vector_tbl(), snap.central_pressure(),
      snap.background_pressure(), snap.v_max(),
      snap.quadrant(1).isotach(2).wind_speed(),
      snap.quadrant(1).isotach(2).radius());
  REQUIRE(params_q2_i3.radius_to_max_winds == Approx(37198.17));
  REQUIRE(params_q2_i3.gahm_b == Approx(1.03968));
  REQUIRE(params_q2_i3.gahm_phi == Approx(1.0641));

  const auto params_q3_i1 = Gahm::Solver::GahmParameters(
      snap.translation(), snap.eye_location(),
      snap.quadrant(2).unit_vector_tbl(), snap.central_pressure(),
      snap.background_pressure(), snap.v_max(),
      snap.quadrant(2).isotach(0).wind_speed(),
      snap.quadrant(2).isotach(0).radius());
  REQUIRE(params_q3_i1.radius_to_max_winds == Approx(38512.87));
  REQUIRE(params_q3_i1.gahm_b == Approx(1.04226));
  REQUIRE(params_q3_i1.gahm_phi == Approx(1.06605));

  const auto params_q3_i2 = Gahm::Solver::GahmParameters(
      snap.translation(), snap.eye_location(),
      snap.quadrant(2).unit_vector_tbl(), snap.central_pressure(),
      snap.background_pressure(), snap.v_max(),
      snap.quadrant(2).isotach(1).wind_speed(),
      snap.quadrant(2).isotach(1).radius());
  REQUIRE(params_q3_i2.radius_to_max_winds == Approx(39219.20));
  REQUIRE(params_q3_i2.gahm_b == Approx(1.04364));
  REQUIRE(params_q3_i2.gahm_phi == Approx(1.06708));

  const auto params_q3_i3 = Gahm::Solver::GahmParameters(
      snap.translation(), snap.eye_location(),
      snap.quadrant(2).unit_vector_tbl(), snap.central_pressure(),
      snap.background_pressure(), snap.v_max(),
      snap.quadrant(2).isotach(2).wind_speed(),
      snap.quadrant(2).isotach(2).radius());
  REQUIRE(params_q3_i3.radius_to_max_winds == Approx(39309.115));
  REQUIRE(params_q3_i3.gahm_b == Approx(1.04382));
  REQUIRE(params_q3_i3.gahm_phi == Approx(1.06722));

  const auto params_q4_i1 = Gahm::Solver::GahmParameters(
      snap.translation(), snap.eye_location(),
      snap.quadrant(3).unit_vector_tbl(), snap.central_pressure(),
      snap.background_pressure(), snap.v_max(),
      snap.quadrant(3).isotach(0).wind_speed(),
      snap.quadrant(3).isotach(0).radius());
  REQUIRE(params_q4_i1.radius_to_max_winds == Approx(23729.948));
  REQUIRE(params_q4_i1.gahm_b == Approx(1.01351));
  REQUIRE(params_q4_i1.gahm_phi == Approx(1.04299));

  const auto params_q4_i2 = Gahm::Solver::GahmParameters(
      snap.translation(), snap.eye_location(),
      snap.quadrant(3).unit_vector_tbl(), snap.central_pressure(),
      snap.background_pressure(), snap.v_max(),
      snap.quadrant(3).isotach(1).wind_speed(),
      snap.quadrant(3).isotach(1).radius());
  REQUIRE(params_q4_i2.radius_to_max_winds == Approx(29929.9));
  REQUIRE(params_q4_i2.gahm_b == Approx(1.02551));
  REQUIRE(params_q4_i2.gahm_phi == Approx(1.05298));

  const auto params_q4_i3 = Gahm::Solver::GahmParameters(
      snap.translation(), snap.eye_location(),
      snap.quadrant(3).unit_vector_tbl(), snap.central_pressure(),
      snap.background_pressure(), snap.v_max(),
      snap.quadrant(3).isotach(2).wind_speed(),
      snap.quadrant(3).isotach(2).radius());
  REQUIRE(params_q4_i3.radius_to_max_winds == Approx(34901.8));
  REQUIRE(params_q4_i3.gahm_b == Approx(1.0352));
  REQUIRE(params_q4_i3.gahm_phi == Approx(1.06065));

  snap.quadrant(0).isotach(0).compute_gahm_parameters(
      snap.translation(), snap.eye_location(),
      snap.quadrant(0).unit_vector_tbl(), snap.central_pressure(),
      snap.background_pressure(), snap.v_max());
  REQUIRE(snap.quadrant(0).isotach(0).radius_to_max_winds() ==
          Approx(47515.74));
  REQUIRE(snap.quadrant(0).isotach(0).gahm_b() == Approx(1.05994));
  REQUIRE(snap.quadrant(0).isotach(0).gahm_phi() == Approx(1.07886));

  snap.compute_gahm_parameters();
  REQUIRE(snap.quadrant(1).isotach(0).radius_to_max_winds() ==
          Approx(40758.26));
  REQUIRE(snap.quadrant(1).isotach(0).gahm_b() == Approx(1.04666));
  REQUIRE(snap.quadrant(1).isotach(0).gahm_phi() == Approx(1.06933));

  snap.to_gnuplot("test.gnuplot");


}

TEST_CASE("GahmParameters-AdjVMax", "[GahmAdjVMax]") {
  constexpr auto mb2pa = Gahm::Physical::Units::convert(
      Gahm::Physical::Units::Millibar, Gahm::Physical::Units::Pascal);
  constexpr auto kt2ms = Gahm::Physical::Units::convert(
      Gahm::Physical::Units::Knot, Gahm::Physical::Units::MetersPerSecond);
  constexpr auto nmi2m = Gahm::Physical::Units::convert(
      Gahm::Physical::Units::NauticalMile, Gahm::Physical::Units::Meter);

  const Gahm::Types::Datetime previous_date("2005082918");
  const Gahm::Types::Datetime snap_date("2005083000");
  const auto dt =
      snap_date.seconds_since_epoch() - previous_date.seconds_since_epoch();

  const double p_c = 961.0 * mb2pa;
  const double p_bk = 1013.0 * mb2pa;
  const double v_max = 50.0 * kt2ms;
  const double r_max = 30.0 * nmi2m;
  const Gahm::Types::Point eye_location(-89.1, 32.6);
  const Gahm::Types::Point previous_eye_location(-89.6, 31.1);
  const Gahm::Storm::StormTranslation translation(eye_location,
                                                  previous_eye_location, dt);

  const Gahm::Atcf::AtcfIO::TempIsotach i34(
      34.0 * kt2ms,
      std::array<double, 4>{75 * nmi2m, 90 * nmi2m, 90 * nmi2m, 50 * nmi2m});
  const Gahm::Atcf::AtcfIO::TempIsotach i50(
      50.0 * kt2ms,
      std::array<double, 4>{50 * nmi2m, 60 * nmi2m, 0 * nmi2m, 0 * nmi2m});
  const Gahm::Atcf::AtcfIO::TempIsotach i64(
      64.0 * kt2ms,
      std::array<double, 4>{0 * nmi2m, 0 * nmi2m, 0 * nmi2m, 0 * nmi2m});
//  const auto i34_s = Gahm::Atcf::AtcfIO::sanitize_temp_isotach(r_max, i34);
//  const auto i50_s = Gahm::Atcf::AtcfIO::sanitize_temp_isotach(r_max, i50);
//  const auto i64_s = Gahm::Atcf::AtcfIO::sanitize_temp_isotach(r_max, i64);

  const auto quadrants = Gahm::Atcf::AtcfIO::transpose_to_quadrants(
      eye_location.y(), {i34, i50, i64});

  Gahm::Atcf::AtcfPeriod snap(snap_date, p_c, p_bk, v_max, r_max, eye_location,
                              quadrants);
  snap.set_translation(translation);

//  REQUIRE(snap.background_pressure() == Approx(101300.0));
//  REQUIRE(snap.central_pressure() == Approx(96100.0));
//  REQUIRE(snap.datetime().to_string() == "2005-08-30T00:00:00");
//  REQUIRE(snap.v_max() == Approx(25.7222));
//  REQUIRE(snap.r_max() == Approx(55559.86));
//  REQUIRE(snap.eye_location().x() == Approx(-89.1));
//  REQUIRE(snap.eye_location().y() == Approx(32.6));
//
//  REQUIRE(snap.quadrant(0).isotach(0).wind_speed() == Approx(34.0 * kt2ms));
//  REQUIRE(snap.quadrant(0).isotach(0).radius() == Approx(138899.654));
//  REQUIRE(snap.quadrant(0).isotach(1).wind_speed() == Approx(50.0 * kt2ms));
//  REQUIRE(snap.quadrant(0).isotach(1).radius() == Approx(92599.769));
//  REQUIRE(snap.quadrant(0).isotach(2).wind_speed() == Approx(64.0 * kt2ms));
//  REQUIRE(snap.quadrant(0).isotach(2).radius() == Approx(55559.861));
//
//  REQUIRE(snap.quadrant(1).isotach(0).radius() == Approx(166679.5857));
//  REQUIRE(snap.quadrant(1).isotach(1).radius() == Approx(111119.723));
//  REQUIRE(snap.quadrant(1).isotach(2).radius() == Approx(55559.861));
//
//  REQUIRE(snap.quadrant(2).isotach(0).radius() == Approx(166679.585));
//  REQUIRE(snap.quadrant(2).isotach(1).radius() == Approx(101859.746));
//  REQUIRE(snap.quadrant(2).isotach(2).radius() == Approx(55559.861));
//
//  snap.compute_gahm_parameters();
//
//  REQUIRE(snap.quadrant(0).isotach(0).radius_to_max_winds() ==
//          Approx(44984.693));
//  REQUIRE(snap.quadrant(0).isotach(0).gahm_b() == Approx(0.24803));
//  REQUIRE(snap.quadrant(0).isotach(0).gahm_phi() == Approx(1.71068));
//  REQUIRE(snap.quadrant(0).isotach(1).holland_b() == Approx(0.279856));
//
//  REQUIRE(snap.quadrant(0).isotach(1).radius_to_max_winds() ==
//          Approx(92599.7689));
//  REQUIRE(snap.quadrant(0).isotach(1).gahm_b() == Approx(0.4284731));
//  REQUIRE(snap.quadrant(0).isotach(1).gahm_phi() == Approx(1.598721));
//
//  REQUIRE(snap.quadrant(0).isotach(2).radius_to_max_winds() ==
//          Approx(55559.861));
//  REQUIRE(snap.quadrant(0).isotach(2).gahm_b() == Approx(0.60354));
//  REQUIRE(snap.quadrant(0).isotach(2).gahm_phi() == Approx(1.21984));
//  REQUIRE(snap.quadrant(0).isotach(2).holland_b() == Approx(0.512517));
//
//  REQUIRE(snap.quadrant(1).isotach(0).radius_to_max_winds() ==
//          Approx(8355.423));
//  REQUIRE(snap.quadrant(1).isotach(0).gahm_b() == Approx(0.1820367));
//  REQUIRE(snap.quadrant(1).isotach(0).gahm_phi() == Approx(1.21));
//  REQUIRE(snap.quadrant(1).isotach(0).holland_b() == Approx(0.171717));
//
//  REQUIRE(snap.quadrant(1).isotach(1).radius_to_max_winds() ==
//          Approx(111119.723));
//  REQUIRE(snap.quadrant(1).isotach(1).gahm_b() == Approx(0.35938));
//  REQUIRE(snap.quadrant(1).isotach(1).gahm_phi() == Approx(1.95013));
//  REQUIRE(snap.quadrant(1).isotach(1).holland_b() == Approx(0.1784695));
//
//  REQUIRE(snap.quadrant(1).isotach(2).radius_to_max_winds() ==
//          Approx(55559.861));
//  REQUIRE(snap.quadrant(1).isotach(2).gahm_b() == Approx(0.449457));
//  REQUIRE(snap.quadrant(1).isotach(2).gahm_phi() == Approx(1.342447));
//  REQUIRE(snap.quadrant(1).isotach(2).holland_b() == Approx(0.362473));
//
//  REQUIRE(snap.quadrant(2).isotach(0).radius_to_max_winds() ==
//          Approx(166679.5847));
//  REQUIRE(snap.quadrant(2).isotach(0).gahm_b() == Approx(0.45998845));
//  REQUIRE(snap.quadrant(2).isotach(0).gahm_phi() == Approx(1.91513));
//  REQUIRE(snap.quadrant(2).isotach(0).holland_b() == Approx(0.204278));
//
//  REQUIRE(snap.quadrant(2).isotach(1).radius_to_max_winds() ==
//          Approx(101859.746));
//  REQUIRE(snap.quadrant(2).isotach(1).gahm_b() == Approx(0.624441));
//  REQUIRE(snap.quadrant(2).isotach(1).gahm_phi() == Approx(1.3671));
//  REQUIRE(snap.quadrant(2).isotach(1).holland_b() == Approx(0.45581));
//
//  REQUIRE(snap.quadrant(2).isotach(2).radius_to_max_winds() ==
//          Approx(55559.861));
//  REQUIRE(snap.quadrant(2).isotach(2).gahm_b() == Approx(0.84439));
//  REQUIRE(snap.quadrant(2).isotach(2).gahm_phi() == Approx(1.133505));
//  REQUIRE(snap.quadrant(2).isotach(2).holland_b() == Approx(0.74309));
//
//  REQUIRE(snap.quadrant(3).isotach(0).radius_to_max_winds() ==
//          Approx(92599.7698));
//  REQUIRE(snap.quadrant(3).isotach(0).gahm_b() == Approx(0.55929));
//  REQUIRE(snap.quadrant(3).isotach(0).gahm_phi() == Approx(1.39716));
//  REQUIRE(snap.quadrant(3).isotach(0).holland_b() == Approx(0.40861));
//
//  REQUIRE(snap.quadrant(3).isotach(1).radius_to_max_winds() ==
//          Approx(101859.749));
//  REQUIRE(snap.quadrant(3).isotach(1).gahm_b() == Approx(0.902245));
//  REQUIRE(snap.quadrant(3).isotach(1).gahm_phi() == Approx(1.21271));
//  REQUIRE(snap.quadrant(3).isotach(1).holland_b() == Approx(0.714752));
//
//  REQUIRE(snap.quadrant(3).isotach(2).radius_to_max_winds() ==
//          Approx(55559.8618));
//  REQUIRE(snap.quadrant(3).isotach(2).gahm_b() == Approx(1.16673));
//  REQUIRE(snap.quadrant(3).isotach(2).gahm_phi() == Approx(1.082738));
//  REQUIRE(snap.quadrant(3).isotach(2).holland_b() == Approx(1.05068));

  snap.compute_gahm_parameters();
//  snap.to_gnuplot("test.gnuplot");

}