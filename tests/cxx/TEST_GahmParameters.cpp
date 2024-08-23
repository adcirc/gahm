//
// Created by Zach Cobell on 7/31/24.
//

#include <array>

#include "atcf/AtcfIO.h"
#include "catch2/catch_approx.hpp"
#include "catch2/catch_test_macros.hpp"

// NOLINTBEGIN(cppcoreguidelines-macro-usage,
// cppcoreguidelines-avoid-magic-numbers, misc-use-anonymous-namespace,
// readability-function-cognitive-complexity, cert-err58-cpp,
// readability-magic-numbers)

using namespace Catch;

TEST_CASE("GahmParameters", "[GahmParameters]") {
  const Gahm::Types::Datetime previous_date("2018091318");
  const Gahm::Types::Datetime snap_date("2018091400");
  const auto dt =
      snap_date.seconds_since_epoch() - previous_date.seconds_since_epoch();

  constexpr long expected_dt = 6 * 3600;
  REQUIRE(dt == expected_dt);

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
  REQUIRE(translation.speed() == Approx(2.977566));
  REQUIRE(translation.direction() == Approx(2.37762));
  REQUIRE(translation.unit_vector().u() == Approx(-0.72209));
  REQUIRE(translation.unit_vector().v() == Approx(0.691794));
  REQUIRE(translation.velocity().u() == Approx(-2.15008));
  REQUIRE(translation.velocity().v() == Approx(2.05986));

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
  REQUIRE(params_q1_i1.radius_to_max_winds() == Approx(47483.2612));
  REQUIRE(params_q1_i1.gahm_b() == Approx(1.05944));
  REQUIRE(params_q1_i1.gahm_phi() == Approx(1.07886));
  REQUIRE(params_q1_i1.holland_b() == Approx(0.96806));

  const auto params_q1_i2 = Gahm::Solver::GahmParameters(
      snap.translation(), snap.eye_location(),
      snap.quadrant(0).unit_vector_tbl(), snap.central_pressure(),
      snap.background_pressure(), snap.v_max(),
      snap.quadrant(0).isotach(1).wind_speed(),
      snap.quadrant(0).isotach(1).radius());
  REQUIRE(params_q1_i2.radius_to_max_winds() == Approx(40125.8786));
  REQUIRE(params_q1_i2.gahm_b() == Approx(1.04498));
  REQUIRE(params_q1_i2.gahm_phi() == Approx(1.06844));

  const auto params_q1_i3 = Gahm::Solver::GahmParameters(
      snap.translation(), snap.eye_location(),
      snap.quadrant(0).unit_vector_tbl(), snap.central_pressure(),
      snap.background_pressure(), snap.v_max(),
      snap.quadrant(0).isotach(2).wind_speed(),
      snap.quadrant(0).isotach(2).radius());
  REQUIRE(params_q1_i3.radius_to_max_winds() == Approx(41448.183));
  REQUIRE(params_q1_i3.gahm_b() == Approx(1.04757));
  REQUIRE(params_q1_i3.gahm_phi() == Approx(1.070364));

  const auto params_q2_i1 = Gahm::Solver::GahmParameters(
      snap.translation(), snap.eye_location(),
      snap.quadrant(1).unit_vector_tbl(), snap.central_pressure(),
      snap.background_pressure(), snap.v_max(),
      snap.quadrant(1).isotach(0).wind_speed(),
      snap.quadrant(1).isotach(0).radius());
  REQUIRE(params_q2_i1.radius_to_max_winds() == Approx(40736.1142));
  REQUIRE(params_q2_i1.gahm_b() == Approx(1.04617));
  REQUIRE(params_q2_i1.gahm_phi() == Approx(1.06933));

  const auto params_q2_i2 = Gahm::Solver::GahmParameters(
      snap.translation(), snap.eye_location(),
      snap.quadrant(1).unit_vector_tbl(), snap.central_pressure(),
      snap.background_pressure(), snap.v_max(),
      snap.quadrant(1).isotach(1).wind_speed(),
      snap.quadrant(1).isotach(1).radius());
  REQUIRE(params_q2_i2.radius_to_max_winds() == Approx(31495.1244));
  REQUIRE(params_q2_i2.gahm_b() == Approx(1.028125));
  REQUIRE(params_q2_i2.gahm_phi() == Approx(1.05546));

  const auto params_q2_i3 = Gahm::Solver::GahmParameters(
      snap.translation(), snap.eye_location(),
      snap.quadrant(1).unit_vector_tbl(), snap.central_pressure(),
      snap.background_pressure(), snap.v_max(),
      snap.quadrant(1).isotach(2).wind_speed(),
      snap.quadrant(1).isotach(2).radius());
  REQUIRE(params_q2_i3.radius_to_max_winds() == Approx(37192.779));
  REQUIRE(params_q2_i3.gahm_b() == Approx(1.039239));
  REQUIRE(params_q2_i3.gahm_phi() == Approx(1.064132));

  const auto params_q3_i1 = Gahm::Solver::GahmParameters(
      snap.translation(), snap.eye_location(),
      snap.quadrant(2).unit_vector_tbl(), snap.central_pressure(),
      snap.background_pressure(), snap.v_max(),
      snap.quadrant(2).isotach(0).wind_speed(),
      snap.quadrant(2).isotach(0).radius());
  REQUIRE(params_q3_i1.radius_to_max_winds() == Approx(38515.124));
  REQUIRE(params_q3_i1.gahm_b() == Approx(1.041826));
  REQUIRE(params_q3_i1.gahm_phi() == Approx(1.06609));

  const auto params_q3_i2 = Gahm::Solver::GahmParameters(
      snap.translation(), snap.eye_location(),
      snap.quadrant(2).unit_vector_tbl(), snap.central_pressure(),
      snap.background_pressure(), snap.v_max(),
      snap.quadrant(2).isotach(1).wind_speed(),
      snap.quadrant(2).isotach(1).radius());
  REQUIRE(params_q3_i2.radius_to_max_winds() == Approx(39236.206));
  REQUIRE(params_q3_i2.gahm_b() == Approx(1.043238));
  REQUIRE(params_q3_i2.gahm_phi() == Approx(1.067150));

  const auto params_q3_i3 = Gahm::Solver::GahmParameters(
      snap.translation(), snap.eye_location(),
      snap.quadrant(2).unit_vector_tbl(), snap.central_pressure(),
      snap.background_pressure(), snap.v_max(),
      snap.quadrant(2).isotach(2).wind_speed(),
      snap.quadrant(2).isotach(2).radius());
  REQUIRE(params_q3_i3.radius_to_max_winds() == Approx(39344.97559));
  REQUIRE(params_q3_i3.gahm_b() == Approx(1.043451));
  REQUIRE(params_q3_i3.gahm_phi() == Approx(1.067309));

  const auto params_q4_i1 = Gahm::Solver::GahmParameters(
      snap.translation(), snap.eye_location(),
      snap.quadrant(3).unit_vector_tbl(), snap.central_pressure(),
      snap.background_pressure(), snap.v_max(),
      snap.quadrant(3).isotach(0).wind_speed(),
      snap.quadrant(3).isotach(0).radius());
  REQUIRE(params_q4_i1.radius_to_max_winds() == Approx(23723.41608));
  REQUIRE(params_q4_i1.gahm_b() == Approx(1.01306));
  REQUIRE(params_q4_i1.gahm_phi() == Approx(1.0430015));

  const auto params_q4_i2 = Gahm::Solver::GahmParameters(
      snap.translation(), snap.eye_location(),
      snap.quadrant(3).unit_vector_tbl(), snap.central_pressure(),
      snap.background_pressure(), snap.v_max(),
      snap.quadrant(3).isotach(1).wind_speed(),
      snap.quadrant(3).isotach(1).radius());
  REQUIRE(params_q4_i2.radius_to_max_winds() == Approx(29931.7377));
  REQUIRE(params_q4_i2.gahm_b() == Approx(1.025086));
  REQUIRE(params_q4_i2.gahm_phi() == Approx(1.05301));

  const auto params_q4_i3 = Gahm::Solver::GahmParameters(
      snap.translation(), snap.eye_location(),
      snap.quadrant(3).unit_vector_tbl(), snap.central_pressure(),
      snap.background_pressure(), snap.v_max(),
      snap.quadrant(3).isotach(2).wind_speed(),
      snap.quadrant(3).isotach(2).radius());
  REQUIRE(params_q4_i3.radius_to_max_winds() == Approx(34916.0783));
  REQUIRE(params_q4_i3.gahm_b() == Approx(1.03479));
  REQUIRE(params_q4_i3.gahm_phi() == Approx(1.060713));

  snap.quadrant(0).isotach(0).compute_gahm_parameters(
      snap.translation(), snap.eye_location(),
      snap.quadrant(0).unit_vector_tbl(), snap.central_pressure(),
      snap.background_pressure(), snap.v_max());
  REQUIRE(snap.quadrant(0).isotach(0).gahm_parameters().radius_to_max_winds() ==
          Approx(47483.2612));
  REQUIRE(snap.quadrant(0).isotach(0).gahm_parameters().gahm_b() ==
          Approx(1.059443));
  REQUIRE(snap.quadrant(0).isotach(0).gahm_parameters().gahm_phi() ==
          Approx(1.07886));

  snap.compute_gahm_parameters();
  REQUIRE(snap.quadrant(1).isotach(0).gahm_parameters().radius_to_max_winds() ==
          Approx(40736.1142));
  REQUIRE(snap.quadrant(1).isotach(0).gahm_parameters().gahm_b() ==
          Approx(1.04617));
  REQUIRE(snap.quadrant(1).isotach(0).gahm_parameters().gahm_phi() ==
          Approx(1.06933));
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

  const auto quadrants = Gahm::Atcf::AtcfIO::transpose_to_quadrants(
      eye_location.y(), {i34, i50, i64});

  Gahm::Atcf::AtcfPeriod snap(snap_date, p_c, p_bk, v_max, r_max, eye_location,
                              quadrants);
  snap.set_translation(translation);

  REQUIRE(snap.background_pressure() == Approx(101300.0));
  REQUIRE(snap.central_pressure() == Approx(96100.0));
  REQUIRE(snap.datetime().to_string() == "2005-08-30T00:00:00");
  REQUIRE(snap.v_max() == Approx(25.7222));
  REQUIRE(snap.r_max() == Approx(55559.86));
  REQUIRE(snap.eye_location().x() == Approx(-89.1));
  REQUIRE(snap.eye_location().y() == Approx(32.6));

  REQUIRE(snap.quadrant(0).isotach(0).wind_speed() == Approx(34.0 * kt2ms));
  REQUIRE(snap.quadrant(0).isotach(0).radius() == Approx(138899.654));
  REQUIRE(snap.quadrant(0).isotach(1).wind_speed() == Approx(50.0 * kt2ms));
  REQUIRE(snap.quadrant(0).isotach(1).radius() == Approx(92599.769));
  REQUIRE(snap.quadrant(0).isotach(2).wind_speed() == Approx(64.0 * kt2ms));

  REQUIRE(snap.quadrant(1).isotach(0).radius() == Approx(166679.5857));
  REQUIRE(snap.quadrant(1).isotach(1).radius() == Approx(111119.723));

  REQUIRE(snap.quadrant(2).isotach(0).radius() == Approx(166679.585));

  snap.compute_gahm_parameters();

  REQUIRE(snap.quadrant(0).isotach(0).gahm_parameters().radius_to_max_winds() ==
          Approx(28908.3707));
  REQUIRE(snap.quadrant(0).isotach(0).gahm_parameters().gahm_b() ==
          Approx(0.27634));
  REQUIRE(snap.quadrant(0).isotach(0).gahm_parameters().gahm_phi() ==
          Approx(1.3820045));
  REQUIRE(snap.quadrant(0).isotach(1).gahm_parameters().holland_b() ==
          Approx(0.3252617));

  REQUIRE(snap.quadrant(0).isotach(1).gahm_parameters().radius_to_max_winds() ==
          Approx(92599.7689));
  REQUIRE(snap.quadrant(0).isotach(1).gahm_parameters().gahm_b() ==
          Approx(0.4738021));
  REQUIRE(snap.quadrant(0).isotach(1).gahm_parameters().gahm_phi() ==
          Approx(1.511737));

  REQUIRE(snap.quadrant(1).isotach(0).gahm_parameters().radius_to_max_winds() ==
          Approx(9751.519));
  REQUIRE(snap.quadrant(1).isotach(0).gahm_parameters().gahm_b() ==
          Approx(0.245111));
  REQUIRE(snap.quadrant(1).isotach(0).gahm_parameters().gahm_phi() ==
          Approx(1.1562));
  REQUIRE(snap.quadrant(1).isotach(0).gahm_parameters().holland_b() ==
          Approx(0.23313339));

  REQUIRE(snap.quadrant(1).isotach(1).gahm_parameters().radius_to_max_winds() ==
          Approx(111119.723));
  REQUIRE(snap.quadrant(1).isotach(1).gahm_parameters().gahm_b() ==
          Approx(0.41762));
  REQUIRE(snap.quadrant(1).isotach(1).gahm_parameters().gahm_phi() ==
          Approx(1.740439));
  REQUIRE(snap.quadrant(1).isotach(1).gahm_parameters().holland_b() ==
          Approx(0.239447));

  REQUIRE(snap.quadrant(2).isotach(0).gahm_parameters().radius_to_max_winds() ==
          Approx(166679.5847));
  REQUIRE(snap.quadrant(2).isotach(0).gahm_parameters().gahm_b() ==
          Approx(0.466707));
  REQUIRE(snap.quadrant(2).isotach(0).gahm_parameters().gahm_phi() ==
          Approx(1.89383));
  REQUIRE(snap.quadrant(2).isotach(0).gahm_parameters().holland_b() ==
          Approx(0.210740));

  REQUIRE(snap.quadrant(3).isotach(0).gahm_parameters().radius_to_max_winds() ==
          Approx(92599.7698));
  REQUIRE(snap.quadrant(3).isotach(0).gahm_parameters().gahm_b() ==
          Approx(0.4824338));
  REQUIRE(snap.quadrant(3).isotach(0).gahm_parameters().gahm_phi() ==
          Approx(1.49766));
  REQUIRE(snap.quadrant(3).isotach(0).gahm_parameters().holland_b() ==
          Approx(0.333795));
}

// NOLINTEND(cppcoreguidelines-macro-usage,
// cppcoreguidelines-avoid-magic-numbers, misc-use-anonymous-namespace,
// readability-function-cognitive-complexity, cert-err58-cpp,
// readability-magic-numbers)