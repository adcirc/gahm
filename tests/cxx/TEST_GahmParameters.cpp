//
// Created by Zach Cobell on 7/31/24.
//

#include "atcf/AtcfIO.hpp"
#include "catch2/catch_test_macros.hpp"
#include "catch2/matchers/catch_matchers_floating_point.hpp"

// NOLINTBEGIN(cppcoreguidelines-macro-usage,
// cppcoreguidelines-avoid-magic-numbers, misc-use-anonymous-namespace,
// readability-function-cognitive-complexity, cert-err58-cpp,
// readability-magic-numbers)

using namespace Catch;
using namespace Catch::Matchers;

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

  REQUIRE_THAT(mb2pa, WithinRel(100.0, 0.001));
  REQUIRE_THAT(kt2ms, WithinRel(0.5144456, 0.001));

  const double p_c = 952.0 * mb2pa;
  const double p_bk = 1013.0 * mb2pa;
  const double v_max = 90.0 * kt2ms;
  const double r_max = 20.0 * nmi2m;
  const Gahm::Types::Point eye_location(-76.5, 34.0);
  const Gahm::Types::Point previous_eye_location(-76.0, 33.6);
  const Gahm::Storm::StormTranslation translation(eye_location,
                                                  previous_eye_location, dt);

  REQUIRE_THAT(p_c, WithinRel(95200.0, 0.001));
  REQUIRE_THAT(p_bk, WithinRel(101300.0, 0.001));
  REQUIRE_THAT(v_max, WithinRel(46.300107, 0.001));
  REQUIRE_THAT(r_max, WithinRel(37039.907937, 0.001));
  REQUIRE_THAT(translation.speed(), WithinRel(2.977566, 0.001));
  REQUIRE_THAT(translation.direction(), WithinRel(2.377621, 0.001));
  REQUIRE_THAT(translation.unit_vector().u(), WithinRel(-0.722094, 0.001));
  REQUIRE_THAT(translation.unit_vector().v(), WithinRel(0.691794, 0.001));
  REQUIRE_THAT(translation.velocity().u(), WithinRel(-2.150083, 0.001));
  REQUIRE_THAT(translation.velocity().v(), WithinRel(2.059864, 0.001));

  const Gahm::Atcf::TempIsotach i34(
      34.0 * kt2ms, std::array<double, 4>{170 * nmi2m, 150 * nmi2m, 130 * nmi2m,
                                          100 * nmi2m});
  const Gahm::Atcf::TempIsotach i50(
      50.0 * kt2ms,
      std::array<double, 4>{100 * nmi2m, 80 * nmi2m, 80 * nmi2m, 70 * nmi2m});
  const Gahm::Atcf::TempIsotach i64(
      64.0 * kt2ms,
      std::array<double, 4>{70 * nmi2m, 60 * nmi2m, 50 * nmi2m, 50 * nmi2m});

  const auto quadrants = Gahm::Atcf::AtcfIO::transpose_to_quadrants(
      eye_location.y(), {i34, i50, i64});

  REQUIRE_THAT(quadrants[0].isotach(0).wind_speed(),
               WithinRel(34.0 * kt2ms, 0.001));
  REQUIRE_THAT(quadrants[0].isotach(0).radius(),
               WithinRel(170.0 * nmi2m, 0.001));
  REQUIRE_THAT(quadrants[1].isotach(0).radius(),
               WithinRel(150.0 * nmi2m, 0.001));
  REQUIRE_THAT(quadrants[2].isotach(0).radius(),
               WithinRel(130.0 * nmi2m, 0.001));
  REQUIRE_THAT(quadrants[3].isotach(0).radius(),
               WithinRel(100.0 * nmi2m, 0.001));

  REQUIRE_THAT(quadrants[1].isotach(1).wind_speed(),
               WithinRel(50.0 * kt2ms, 0.001));
  REQUIRE_THAT(quadrants[0].isotach(1).radius(),
               WithinRel(100.0 * nmi2m, 0.001));
  REQUIRE_THAT(quadrants[1].isotach(1).radius(), WithinRel(80.0 * nmi2m, 0.001));
  REQUIRE_THAT(quadrants[2].isotach(1).radius(), WithinRel(80.0 * nmi2m, 0.001));
  REQUIRE_THAT(quadrants[3].isotach(1).radius(), WithinRel(70.0 * nmi2m, 0.001));

  REQUIRE_THAT(quadrants[3].isotach(2).wind_speed(),
               WithinRel(64.0 * kt2ms, 0.001));
  REQUIRE_THAT(quadrants[0].isotach(2).radius(), WithinRel(70.0 * nmi2m, 0.001));
  REQUIRE_THAT(quadrants[1].isotach(2).radius(), WithinRel(60.0 * nmi2m, 0.001));
  REQUIRE_THAT(quadrants[2].isotach(2).radius(), WithinRel(50.0 * nmi2m, 0.001));
  REQUIRE_THAT(quadrants[3].isotach(2).radius(), WithinRel(50.0 * nmi2m, 0.001));

  Gahm::Atcf::AtcfPeriod snap(snap_date, p_c, p_bk, v_max, r_max, eye_location,
                              quadrants);
  snap.set_translation(translation);

  REQUIRE_THAT(snap.background_pressure(), WithinRel(101300.0, 0.001));
  REQUIRE_THAT(snap.central_pressure(), WithinRel(95200.0, 0.001));
  REQUIRE(snap.datetime().to_string() == "2018-09-14T00:00:00");
  REQUIRE_THAT(snap.v_max(), WithinRel(46.300107, 0.001));
  REQUIRE_THAT(snap.r_max(), WithinRel(37039.907937, 0.001));
  REQUIRE_THAT(snap.eye_location().x(), WithinRel(-76.5, 0.001));
  REQUIRE_THAT(snap.eye_location().y(), WithinRel(34.0, 0.001));
  REQUIRE(snap.translation() == translation);

  const auto params_q1_i1 = Gahm::Solver::GahmParameters(
      snap.translation(), snap.eye_location(),
      snap.quadrant(0).unit_vector_tbl(), snap.central_pressure(),
      snap.background_pressure(), snap.v_max(),
      snap.quadrant(0).isotach(0).wind_speed(),
      snap.quadrant(0).isotach(0).radius());
  REQUIRE_THAT(params_q1_i1.radius_to_max_winds(), WithinRel(47483.261218, 0.001));
  REQUIRE_THAT(params_q1_i1.gahm_b(), WithinRel(1.059443, 0.001));
  REQUIRE_THAT(params_q1_i1.gahm_phi(), WithinRel(1.07886, 0.001));
  REQUIRE_THAT(params_q1_i1.holland_b(), WithinRel(0.968063, 0.001));

  const auto params_q1_i2 = Gahm::Solver::GahmParameters(
      snap.translation(), snap.eye_location(),
      snap.quadrant(0).unit_vector_tbl(), snap.central_pressure(),
      snap.background_pressure(), snap.v_max(),
      snap.quadrant(0).isotach(1).wind_speed(),
      snap.quadrant(0).isotach(1).radius());
  REQUIRE_THAT(params_q1_i2.radius_to_max_winds(), WithinRel(40125.878661, 0.001));
  REQUIRE_THAT(params_q1_i2.gahm_b(), WithinRel(1.0449821, 0.001));
  REQUIRE_THAT(params_q1_i2.gahm_phi(), WithinRel(1.068449, 0.001));

  const auto params_q1_i3 = Gahm::Solver::GahmParameters(
      snap.translation(), snap.eye_location(),
      snap.quadrant(0).unit_vector_tbl(), snap.central_pressure(),
      snap.background_pressure(), snap.v_max(),
      snap.quadrant(0).isotach(2).wind_speed(),
      snap.quadrant(0).isotach(2).radius());
  REQUIRE_THAT(params_q1_i3.radius_to_max_winds(), WithinRel(41448.183351, 0.001));
  REQUIRE_THAT(params_q1_i3.gahm_b(), WithinRel(1.047575, 0.001));
  REQUIRE_THAT(params_q1_i3.gahm_phi(), WithinRel(1.070364, 0.001));

  const auto params_q2_i1 = Gahm::Solver::GahmParameters(
      snap.translation(), snap.eye_location(),
      snap.quadrant(1).unit_vector_tbl(), snap.central_pressure(),
      snap.background_pressure(), snap.v_max(),
      snap.quadrant(1).isotach(0).wind_speed(),
      snap.quadrant(1).isotach(0).radius());
  REQUIRE_THAT(params_q2_i1.radius_to_max_winds(), WithinRel(40736.114234, 0.001));
  REQUIRE_THAT(params_q2_i1.gahm_b(), WithinRel(1.046178, 0.001));
  REQUIRE_THAT(params_q2_i1.gahm_phi(), WithinRel(1.069335, 0.001));

  const auto params_q2_i2 = Gahm::Solver::GahmParameters(
      snap.translation(), snap.eye_location(),
      snap.quadrant(1).unit_vector_tbl(), snap.central_pressure(),
      snap.background_pressure(), snap.v_max(),
      snap.quadrant(1).isotach(1).wind_speed(),
      snap.quadrant(1).isotach(1).radius());
  REQUIRE_THAT(params_q2_i2.radius_to_max_winds(), WithinRel(31495.124443, 0.001));
  REQUIRE_THAT(params_q2_i2.gahm_b(), WithinRel(1.028125, 0.001));
  REQUIRE_THAT(params_q2_i2.gahm_phi(), WithinRel(1.055461, 0.001));

  const auto params_q2_i3 = Gahm::Solver::GahmParameters(
      snap.translation(), snap.eye_location(),
      snap.quadrant(1).unit_vector_tbl(), snap.central_pressure(),
      snap.background_pressure(), snap.v_max(),
      snap.quadrant(1).isotach(2).wind_speed(),
      snap.quadrant(1).isotach(2).radius());
  REQUIRE_THAT(params_q2_i3.radius_to_max_winds(), WithinRel(37192.779139, 0.001));
  REQUIRE_THAT(params_q2_i3.gahm_b(), WithinRel(1.039239, 0.001));
  REQUIRE_THAT(params_q2_i3.gahm_phi(), WithinRel(1.064132, 0.001));

  const auto params_q3_i1 = Gahm::Solver::GahmParameters(
      snap.translation(), snap.eye_location(),
      snap.quadrant(2).unit_vector_tbl(), snap.central_pressure(),
      snap.background_pressure(), snap.v_max(),
      snap.quadrant(2).isotach(0).wind_speed(),
      snap.quadrant(2).isotach(0).radius());
  REQUIRE_THAT(params_q3_i1.radius_to_max_winds(), WithinRel(38515.124120, 0.001));
  REQUIRE_THAT(params_q3_i1.gahm_b(), WithinRel(1.041826, 0.001));
  REQUIRE_THAT(params_q3_i1.gahm_phi(), WithinRel(1.06609, 0.001));

  const auto params_q3_i2 = Gahm::Solver::GahmParameters(
      snap.translation(), snap.eye_location(),
      snap.quadrant(2).unit_vector_tbl(), snap.central_pressure(),
      snap.background_pressure(), snap.v_max(),
      snap.quadrant(2).isotach(1).wind_speed(),
      snap.quadrant(2).isotach(1).radius());
  REQUIRE_THAT(params_q3_i2.radius_to_max_winds(), WithinRel(39236.206189, 0.001));
  REQUIRE_THAT(params_q3_i2.gahm_b(), WithinRel(1.043238, 0.001));
  REQUIRE_THAT(params_q3_i2.gahm_phi(), WithinRel(1.067150, 0.001));

  const auto params_q3_i3 = Gahm::Solver::GahmParameters(
      snap.translation(), snap.eye_location(),
      snap.quadrant(2).unit_vector_tbl(), snap.central_pressure(),
      snap.background_pressure(), snap.v_max(),
      snap.quadrant(2).isotach(2).wind_speed(),
      snap.quadrant(2).isotach(2).radius());
  REQUIRE_THAT(params_q3_i3.radius_to_max_winds(),
               WithinRel(39344.975592, 0.001));
  REQUIRE_THAT(params_q3_i3.gahm_b(), WithinRel(1.043451, 0.001));
  REQUIRE_THAT(params_q3_i3.gahm_phi(), WithinRel(1.067309, 0.001));

  const auto params_q4_i1 = Gahm::Solver::GahmParameters(
      snap.translation(), snap.eye_location(),
      snap.quadrant(3).unit_vector_tbl(), snap.central_pressure(),
      snap.background_pressure(), snap.v_max(),
      snap.quadrant(3).isotach(0).wind_speed(),
      snap.quadrant(3).isotach(0).radius());
  REQUIRE_THAT(params_q4_i1.radius_to_max_winds(),
               WithinRel(23723.416089, 0.001));
  REQUIRE_THAT(params_q4_i1.gahm_b(), WithinRel(1.013067, 0.001));
  REQUIRE_THAT(params_q4_i1.gahm_phi(), WithinRel(1.0430015, 0.001));

  const auto params_q4_i2 = Gahm::Solver::GahmParameters(
      snap.translation(), snap.eye_location(),
      snap.quadrant(3).unit_vector_tbl(), snap.central_pressure(),
      snap.background_pressure(), snap.v_max(),
      snap.quadrant(3).isotach(1).wind_speed(),
      snap.quadrant(3).isotach(1).radius());
  REQUIRE_THAT(params_q4_i2.radius_to_max_winds(), WithinRel(29931.737717, 0.001));
  REQUIRE_THAT(params_q4_i2.gahm_b(), WithinRel(1.025086, 0.001));
  REQUIRE_THAT(params_q4_i2.gahm_phi(), WithinRel(1.053014, 0.001));

  const auto params_q4_i3 = Gahm::Solver::GahmParameters(
      snap.translation(), snap.eye_location(),
      snap.quadrant(3).unit_vector_tbl(), snap.central_pressure(),
      snap.background_pressure(), snap.v_max(),
      snap.quadrant(3).isotach(2).wind_speed(),
      snap.quadrant(3).isotach(2).radius());
  REQUIRE_THAT(params_q4_i3.radius_to_max_winds(), WithinRel(34916.078328, 0.001));
  REQUIRE_THAT(params_q4_i3.gahm_b(), WithinRel(1.034791, 0.001));
  REQUIRE_THAT(params_q4_i3.gahm_phi(), WithinRel(1.060713, 0.001));

  snap.quadrant(0).isotach(0).compute_gahm_parameters(
      snap.translation(), snap.eye_location(),
      snap.quadrant(0).unit_vector_tbl(), snap.central_pressure(),
      snap.background_pressure(), snap.v_max());
  REQUIRE_THAT(
      snap.quadrant(0).isotach(0).gahm_parameters().radius_to_max_winds(),
      WithinRel(47483.261218, 0.001));
  REQUIRE_THAT(snap.quadrant(0).isotach(0).gahm_parameters().gahm_b(),
               WithinRel(1.059443, 0.001));
  REQUIRE_THAT(snap.quadrant(0).isotach(0).gahm_parameters().gahm_phi(),
               WithinRel(1.07886, 0.001));

  snap.compute_gahm_parameters();
  REQUIRE_THAT(
      snap.quadrant(1).isotach(0).gahm_parameters().radius_to_max_winds(),
      WithinRel(40736.114234, 0.001));
  REQUIRE_THAT(snap.quadrant(1).isotach(0).gahm_parameters().gahm_b(),
               WithinRel(1.046178, 0.001));
  REQUIRE_THAT(snap.quadrant(1).isotach(0).gahm_parameters().gahm_phi(),
               WithinRel(1.069335, 0.001));
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

  const Gahm::Atcf::TempIsotach i34(
      34.0 * kt2ms,
      std::array<double, 4>{75 * nmi2m, 90 * nmi2m, 90 * nmi2m, 50 * nmi2m});
  const Gahm::Atcf::TempIsotach i50(
      50.0 * kt2ms,
      std::array<double, 4>{50 * nmi2m, 60 * nmi2m, 0 * nmi2m, 0 * nmi2m});
  const Gahm::Atcf::TempIsotach i64(
      64.0 * kt2ms,
      std::array<double, 4>{0 * nmi2m, 0 * nmi2m, 0 * nmi2m, 0 * nmi2m});

  const auto quadrants = Gahm::Atcf::AtcfIO::transpose_to_quadrants(
      eye_location.y(), {i34, i50, i64});

  Gahm::Atcf::AtcfPeriod snap(snap_date, p_c, p_bk, v_max, r_max, eye_location,
                              quadrants);
  snap.set_translation(translation);

  REQUIRE_THAT(snap.background_pressure(), WithinRel(101300.0, 0.001));
  REQUIRE_THAT(snap.central_pressure(), WithinRel(96100.0, 0.001));
  REQUIRE(snap.datetime().to_string() == "2005-08-30T00:00:00");
  REQUIRE_THAT(snap.v_max(), WithinRel(25.722281, 0.001));
  REQUIRE_THAT(snap.r_max(), WithinRel(55559.861905, 0.001));
  REQUIRE_THAT(snap.eye_location().x(), WithinRel(-89.1, 0.001));
  REQUIRE_THAT(snap.eye_location().y(), WithinRel(32.6, 0.001));

  REQUIRE_THAT(snap.quadrant(0).isotach(0).wind_speed(),
               WithinRel(34.0 * kt2ms, 0.001));
  REQUIRE_THAT(snap.quadrant(0).isotach(0).radius(),
               WithinRel(138899.654764, 0.001));
  REQUIRE_THAT(snap.quadrant(0).isotach(1).wind_speed(),
               WithinRel(50.0 * kt2ms, 0.001));
  REQUIRE_THAT(snap.quadrant(0).isotach(1).radius(),
               WithinRel(92599.769842, 0.001));
  REQUIRE_THAT(snap.quadrant(0).isotach(2).wind_speed(),
               WithinRel(64.0 * kt2ms, 0.001));

  REQUIRE_THAT(snap.quadrant(1).isotach(0).radius(),
               WithinRel(166679.585716, 0.001));
  REQUIRE_THAT(snap.quadrant(1).isotach(1).radius(),
               WithinRel(111119.723811, 0.001));

  REQUIRE_THAT(snap.quadrant(2).isotach(0).radius(),
               WithinRel(166679.585716, 0.001));

  snap.compute_gahm_parameters();

  REQUIRE_THAT(
      snap.quadrant(0).isotach(0).gahm_parameters().radius_to_max_winds(),
      WithinRel(28908.370764, 0.001));
  REQUIRE_THAT(snap.quadrant(0).isotach(0).gahm_parameters().gahm_b(),
               WithinRel(0.276342, 0.001));
  REQUIRE_THAT(snap.quadrant(0).isotach(0).gahm_parameters().gahm_phi(),
               WithinRel(1.3820045, 0.001));
  REQUIRE_THAT(snap.quadrant(0).isotach(1).gahm_parameters().holland_b(),
               WithinRel(0.3252617, 0.001));

  REQUIRE_THAT(
      snap.quadrant(0).isotach(1).gahm_parameters().radius_to_max_winds(),
      WithinRel(92599.768364, 0.001));
  REQUIRE_THAT(snap.quadrant(0).isotach(1).gahm_parameters().gahm_b(),
               WithinRel(0.4738021, 0.001));
  REQUIRE_THAT(snap.quadrant(0).isotach(1).gahm_parameters().gahm_phi(),
               WithinRel(1.511737, 0.001));

  REQUIRE_THAT(
      snap.quadrant(1).isotach(0).gahm_parameters().radius_to_max_winds(),
      WithinRel(9751.519382, 0.001));
  REQUIRE_THAT(snap.quadrant(1).isotach(0).gahm_parameters().gahm_b(),
               WithinRel(0.245111, 0.001));
  REQUIRE_THAT(snap.quadrant(1).isotach(0).gahm_parameters().gahm_phi(),
               WithinRel(1.156206, 0.001));
  REQUIRE_THAT(snap.quadrant(1).isotach(0).gahm_parameters().holland_b(),
               WithinRel(0.23313339, 0.001));

  REQUIRE_THAT(
      snap.quadrant(1).isotach(1).gahm_parameters().radius_to_max_winds(),
      WithinRel(111119.723770, 0.001));
  REQUIRE_THAT(snap.quadrant(1).isotach(1).gahm_parameters().gahm_b(),
               WithinRel(0.41762, 0.001));
  REQUIRE_THAT(snap.quadrant(1).isotach(1).gahm_parameters().gahm_phi(),
               WithinRel(1.740439, 0.001));
  REQUIRE_THAT(snap.quadrant(1).isotach(1).gahm_parameters().holland_b(),
               WithinRel(0.239447, 0.001));

  REQUIRE_THAT(
      snap.quadrant(2).isotach(0).gahm_parameters().radius_to_max_winds(),
      WithinRel(166679.583757, 0.001));
  REQUIRE_THAT(snap.quadrant(2).isotach(0).gahm_parameters().gahm_b(),
               WithinRel(0.466707, 0.001));
  REQUIRE_THAT(snap.quadrant(2).isotach(0).gahm_parameters().gahm_phi(),
               WithinRel(1.893838, 0.001));
  REQUIRE_THAT(snap.quadrant(2).isotach(0).gahm_parameters().holland_b(),
               WithinRel(0.210740, 0.001));

  REQUIRE_THAT(
      snap.quadrant(3).isotach(0).gahm_parameters().radius_to_max_winds(),
      WithinRel(92599.769089, 0.001));
  REQUIRE_THAT(snap.quadrant(3).isotach(0).gahm_parameters().gahm_b(),
               WithinRel(0.4824338, 0.001));
  REQUIRE_THAT(snap.quadrant(3).isotach(0).gahm_parameters().gahm_phi(),
               WithinRel(1.497667, 0.001));
  REQUIRE_THAT(snap.quadrant(3).isotach(0).gahm_parameters().holland_b(),
               WithinRel(0.333795, 0.001));
}

// NOLINTEND(cppcoreguidelines-macro-usage,
// cppcoreguidelines-avoid-magic-numbers, misc-use-anonymous-namespace,
// readability-function-cognitive-complexity, cert-err58-cpp,
// readability-magic-numbers)