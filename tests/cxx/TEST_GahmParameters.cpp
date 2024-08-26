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

  REQUIRE_THAT(mb2pa, WithinAbs(100.0, 1e-6));
  REQUIRE_THAT(kt2ms, WithinAbs(0.5144456, 1e-6));

  const double p_c = 952.0 * mb2pa;
  const double p_bk = 1013.0 * mb2pa;
  const double v_max = 90.0 * kt2ms;
  const double r_max = 20.0 * nmi2m;
  const Gahm::Types::Point eye_location(-76.5, 34.0);
  const Gahm::Types::Point previous_eye_location(-76.0, 33.6);
  const Gahm::Storm::StormTranslation translation(eye_location,
                                                  previous_eye_location, dt);

  REQUIRE_THAT(p_c, WithinAbs(95200.0, 1e-6));
  REQUIRE_THAT(p_bk, WithinAbs(101300.0, 1e-6));
  REQUIRE_THAT(v_max, WithinAbs(46.300107, 1e-6));
  REQUIRE_THAT(r_max, WithinAbs(37039.907937, 1e-6));
  REQUIRE_THAT(translation.speed(), WithinAbs(2.977566, 1e-6));
  REQUIRE_THAT(translation.direction(), WithinAbs(2.377621, 1e-6));
  REQUIRE_THAT(translation.unit_vector().u(), WithinAbs(-0.722094, 1e-6));
  REQUIRE_THAT(translation.unit_vector().v(), WithinAbs(0.691794, 1e-6));
  REQUIRE_THAT(translation.velocity().u(), WithinAbs(-2.150083, 1e-6));
  REQUIRE_THAT(translation.velocity().v(), WithinAbs(2.059864, 1e-6));

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
               WithinAbs(34.0 * kt2ms, 1e-6));
  REQUIRE_THAT(quadrants[0].isotach(0).radius(),
               WithinAbs(170.0 * nmi2m, 1e-6));
  REQUIRE_THAT(quadrants[1].isotach(0).radius(),
               WithinAbs(150.0 * nmi2m, 1e-6));
  REQUIRE_THAT(quadrants[2].isotach(0).radius(),
               WithinAbs(130.0 * nmi2m, 1e-6));
  REQUIRE_THAT(quadrants[3].isotach(0).radius(),
               WithinAbs(100.0 * nmi2m, 1e-6));

  REQUIRE_THAT(quadrants[1].isotach(1).wind_speed(),
               WithinAbs(50.0 * kt2ms, 1e-6));
  REQUIRE_THAT(quadrants[0].isotach(1).radius(),
               WithinAbs(100.0 * nmi2m, 1e-6));
  REQUIRE_THAT(quadrants[1].isotach(1).radius(), WithinAbs(80.0 * nmi2m, 1e-6));
  REQUIRE_THAT(quadrants[2].isotach(1).radius(), WithinAbs(80.0 * nmi2m, 1e-6));
  REQUIRE_THAT(quadrants[3].isotach(1).radius(), WithinAbs(70.0 * nmi2m, 1e-6));

  REQUIRE_THAT(quadrants[3].isotach(2).wind_speed(),
               WithinAbs(64.0 * kt2ms, 1e-6));
  REQUIRE_THAT(quadrants[0].isotach(2).radius(), WithinAbs(70.0 * nmi2m, 1e-6));
  REQUIRE_THAT(quadrants[1].isotach(2).radius(), WithinAbs(60.0 * nmi2m, 1e-6));
  REQUIRE_THAT(quadrants[2].isotach(2).radius(), WithinAbs(50.0 * nmi2m, 1e-6));
  REQUIRE_THAT(quadrants[3].isotach(2).radius(), WithinAbs(50.0 * nmi2m, 1e-6));

  Gahm::Atcf::AtcfPeriod snap(snap_date, p_c, p_bk, v_max, r_max, eye_location,
                              quadrants);
  snap.set_translation(translation);

  REQUIRE_THAT(snap.background_pressure(), WithinAbs(101300.0, 1e-6));
  REQUIRE_THAT(snap.central_pressure(), WithinAbs(95200.0, 1e-6));
  REQUIRE(snap.datetime().to_string() == "2018-09-14T00:00:00");
  REQUIRE_THAT(snap.v_max(), WithinAbs(46.300107, 1e-6));
  REQUIRE_THAT(snap.r_max(), WithinAbs(37039.907937, 1e-6));
  REQUIRE_THAT(snap.eye_location().x(), WithinAbs(-76.5, 1e-6));
  REQUIRE_THAT(snap.eye_location().y(), WithinAbs(34.0, 1e-6));
  REQUIRE(snap.translation() == translation);

  const auto params_q1_i1 = Gahm::Solver::GahmParameters(
      snap.translation(), snap.eye_location(),
      snap.quadrant(0).unit_vector_tbl(), snap.central_pressure(),
      snap.background_pressure(), snap.v_max(),
      snap.quadrant(0).isotach(0).wind_speed(),
      snap.quadrant(0).isotach(0).radius());
  REQUIRE_THAT(params_q1_i1.radius_to_max_winds(), WithinAbs(47483.261218, 1e-6));
  REQUIRE_THAT(params_q1_i1.gahm_b(), WithinAbs(1.059443, 1e-6));
  REQUIRE_THAT(params_q1_i1.gahm_phi(), WithinAbs(1.07886, 1e-6));
  REQUIRE_THAT(params_q1_i1.holland_b(), WithinAbs(0.968063, 1e-6));

  const auto params_q1_i2 = Gahm::Solver::GahmParameters(
      snap.translation(), snap.eye_location(),
      snap.quadrant(0).unit_vector_tbl(), snap.central_pressure(),
      snap.background_pressure(), snap.v_max(),
      snap.quadrant(0).isotach(1).wind_speed(),
      snap.quadrant(0).isotach(1).radius());
  REQUIRE_THAT(params_q1_i2.radius_to_max_winds(), WithinAbs(40125.878661, 1e-6));
  REQUIRE_THAT(params_q1_i2.gahm_b(), WithinAbs(1.0449821, 1e-6));
  REQUIRE_THAT(params_q1_i2.gahm_phi(), WithinAbs(1.068449, 1e-6));

  const auto params_q1_i3 = Gahm::Solver::GahmParameters(
      snap.translation(), snap.eye_location(),
      snap.quadrant(0).unit_vector_tbl(), snap.central_pressure(),
      snap.background_pressure(), snap.v_max(),
      snap.quadrant(0).isotach(2).wind_speed(),
      snap.quadrant(0).isotach(2).radius());
  REQUIRE_THAT(params_q1_i3.radius_to_max_winds(), WithinAbs(41448.183351, 1e-6));
  REQUIRE_THAT(params_q1_i3.gahm_b(), WithinAbs(1.047575, 1e-6));
  REQUIRE_THAT(params_q1_i3.gahm_phi(), WithinAbs(1.070364, 1e-6));

  const auto params_q2_i1 = Gahm::Solver::GahmParameters(
      snap.translation(), snap.eye_location(),
      snap.quadrant(1).unit_vector_tbl(), snap.central_pressure(),
      snap.background_pressure(), snap.v_max(),
      snap.quadrant(1).isotach(0).wind_speed(),
      snap.quadrant(1).isotach(0).radius());
  REQUIRE_THAT(params_q2_i1.radius_to_max_winds(), WithinAbs(40736.114234, 1e-6));
  REQUIRE_THAT(params_q2_i1.gahm_b(), WithinAbs(1.046178, 1e-6));
  REQUIRE_THAT(params_q2_i1.gahm_phi(), WithinAbs(1.069335, 1e-6));

  const auto params_q2_i2 = Gahm::Solver::GahmParameters(
      snap.translation(), snap.eye_location(),
      snap.quadrant(1).unit_vector_tbl(), snap.central_pressure(),
      snap.background_pressure(), snap.v_max(),
      snap.quadrant(1).isotach(1).wind_speed(),
      snap.quadrant(1).isotach(1).radius());
  REQUIRE_THAT(params_q2_i2.radius_to_max_winds(), WithinAbs(31495.124443, 1e-6));
  REQUIRE_THAT(params_q2_i2.gahm_b(), WithinAbs(1.028125, 1e-6));
  REQUIRE_THAT(params_q2_i2.gahm_phi(), WithinAbs(1.055461, 1e-6));

  const auto params_q2_i3 = Gahm::Solver::GahmParameters(
      snap.translation(), snap.eye_location(),
      snap.quadrant(1).unit_vector_tbl(), snap.central_pressure(),
      snap.background_pressure(), snap.v_max(),
      snap.quadrant(1).isotach(2).wind_speed(),
      snap.quadrant(1).isotach(2).radius());
  REQUIRE_THAT(params_q2_i3.radius_to_max_winds(), WithinAbs(37192.779139, 1e-6));
  REQUIRE_THAT(params_q2_i3.gahm_b(), WithinAbs(1.039239, 1e-6));
  REQUIRE_THAT(params_q2_i3.gahm_phi(), WithinAbs(1.064132, 1e-6));

  const auto params_q3_i1 = Gahm::Solver::GahmParameters(
      snap.translation(), snap.eye_location(),
      snap.quadrant(2).unit_vector_tbl(), snap.central_pressure(),
      snap.background_pressure(), snap.v_max(),
      snap.quadrant(2).isotach(0).wind_speed(),
      snap.quadrant(2).isotach(0).radius());
  REQUIRE_THAT(params_q3_i1.radius_to_max_winds(), WithinAbs(38515.124120, 1e-6));
  REQUIRE_THAT(params_q3_i1.gahm_b(), WithinAbs(1.041826, 1e-6));
  REQUIRE_THAT(params_q3_i1.gahm_phi(), WithinAbs(1.06609, 1e-6));

  const auto params_q3_i2 = Gahm::Solver::GahmParameters(
      snap.translation(), snap.eye_location(),
      snap.quadrant(2).unit_vector_tbl(), snap.central_pressure(),
      snap.background_pressure(), snap.v_max(),
      snap.quadrant(2).isotach(1).wind_speed(),
      snap.quadrant(2).isotach(1).radius());
  REQUIRE_THAT(params_q3_i2.radius_to_max_winds(), WithinAbs(39236.206189, 1e-6));
  REQUIRE_THAT(params_q3_i2.gahm_b(), WithinAbs(1.043238, 1e-6));
  REQUIRE_THAT(params_q3_i2.gahm_phi(), WithinAbs(1.067150, 1e-6));

  const auto params_q3_i3 = Gahm::Solver::GahmParameters(
      snap.translation(), snap.eye_location(),
      snap.quadrant(2).unit_vector_tbl(), snap.central_pressure(),
      snap.background_pressure(), snap.v_max(),
      snap.quadrant(2).isotach(2).wind_speed(),
      snap.quadrant(2).isotach(2).radius());
  REQUIRE_THAT(params_q3_i3.radius_to_max_winds(),
               WithinAbs(39344.975592, 1e-6));
  REQUIRE_THAT(params_q3_i3.gahm_b(), WithinAbs(1.043451, 1e-6));
  REQUIRE_THAT(params_q3_i3.gahm_phi(), WithinAbs(1.067309, 1e-6));

  const auto params_q4_i1 = Gahm::Solver::GahmParameters(
      snap.translation(), snap.eye_location(),
      snap.quadrant(3).unit_vector_tbl(), snap.central_pressure(),
      snap.background_pressure(), snap.v_max(),
      snap.quadrant(3).isotach(0).wind_speed(),
      snap.quadrant(3).isotach(0).radius());
  REQUIRE_THAT(params_q4_i1.radius_to_max_winds(),
               WithinAbs(23723.416089, 1e-6));
  REQUIRE_THAT(params_q4_i1.gahm_b(), WithinAbs(1.013067, 1e-6));
  REQUIRE_THAT(params_q4_i1.gahm_phi(), WithinAbs(1.0430015, 1e-6));

  const auto params_q4_i2 = Gahm::Solver::GahmParameters(
      snap.translation(), snap.eye_location(),
      snap.quadrant(3).unit_vector_tbl(), snap.central_pressure(),
      snap.background_pressure(), snap.v_max(),
      snap.quadrant(3).isotach(1).wind_speed(),
      snap.quadrant(3).isotach(1).radius());
  REQUIRE_THAT(params_q4_i2.radius_to_max_winds(), WithinAbs(29931.737717, 1e-6));
  REQUIRE_THAT(params_q4_i2.gahm_b(), WithinAbs(1.025086, 1e-6));
  REQUIRE_THAT(params_q4_i2.gahm_phi(), WithinAbs(1.053014, 1e-6));

  const auto params_q4_i3 = Gahm::Solver::GahmParameters(
      snap.translation(), snap.eye_location(),
      snap.quadrant(3).unit_vector_tbl(), snap.central_pressure(),
      snap.background_pressure(), snap.v_max(),
      snap.quadrant(3).isotach(2).wind_speed(),
      snap.quadrant(3).isotach(2).radius());
  REQUIRE_THAT(params_q4_i3.radius_to_max_winds(), WithinAbs(34916.078328, 1e-6));
  REQUIRE_THAT(params_q4_i3.gahm_b(), WithinAbs(1.034791, 1e-6));
  REQUIRE_THAT(params_q4_i3.gahm_phi(), WithinAbs(1.060713, 1e-6));

  snap.quadrant(0).isotach(0).compute_gahm_parameters(
      snap.translation(), snap.eye_location(),
      snap.quadrant(0).unit_vector_tbl(), snap.central_pressure(),
      snap.background_pressure(), snap.v_max());
  REQUIRE_THAT(
      snap.quadrant(0).isotach(0).gahm_parameters().radius_to_max_winds(),
      WithinAbs(47483.261218, 1e-6));
  REQUIRE_THAT(snap.quadrant(0).isotach(0).gahm_parameters().gahm_b(),
               WithinAbs(1.059443, 1e-6));
  REQUIRE_THAT(snap.quadrant(0).isotach(0).gahm_parameters().gahm_phi(),
               WithinAbs(1.07886, 1e-6));

  snap.compute_gahm_parameters();
  REQUIRE_THAT(
      snap.quadrant(1).isotach(0).gahm_parameters().radius_to_max_winds(),
      WithinAbs(40736.114234, 1e-6));
  REQUIRE_THAT(snap.quadrant(1).isotach(0).gahm_parameters().gahm_b(),
               WithinAbs(1.046178, 1e-6));
  REQUIRE_THAT(snap.quadrant(1).isotach(0).gahm_parameters().gahm_phi(),
               WithinAbs(1.069335, 1e-6));
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

  REQUIRE_THAT(snap.background_pressure(), WithinAbs(101300.0, 1e-6));
  REQUIRE_THAT(snap.central_pressure(), WithinAbs(96100.0, 1e-6));
  REQUIRE(snap.datetime().to_string() == "2005-08-30T00:00:00");
  REQUIRE_THAT(snap.v_max(), WithinAbs(25.722281, 1e-6));
  REQUIRE_THAT(snap.r_max(), WithinAbs(55559.861905, 1e-6));
  REQUIRE_THAT(snap.eye_location().x(), WithinAbs(-89.1, 1e-6));
  REQUIRE_THAT(snap.eye_location().y(), WithinAbs(32.6, 1e-6));

  REQUIRE_THAT(snap.quadrant(0).isotach(0).wind_speed(),
               WithinAbs(34.0 * kt2ms, 1e-6));
  REQUIRE_THAT(snap.quadrant(0).isotach(0).radius(),
               WithinAbs(138899.654764, 1e-6));
  REQUIRE_THAT(snap.quadrant(0).isotach(1).wind_speed(),
               WithinAbs(50.0 * kt2ms, 1e-6));
  REQUIRE_THAT(snap.quadrant(0).isotach(1).radius(),
               WithinAbs(92599.769842, 1e-6));
  REQUIRE_THAT(snap.quadrant(0).isotach(2).wind_speed(),
               WithinAbs(64.0 * kt2ms, 1e-6));

  REQUIRE_THAT(snap.quadrant(1).isotach(0).radius(),
               WithinAbs(166679.585716, 1e-6));
  REQUIRE_THAT(snap.quadrant(1).isotach(1).radius(),
               WithinAbs(111119.723811, 1e-6));

  REQUIRE_THAT(snap.quadrant(2).isotach(0).radius(),
               WithinAbs(166679.585716, 1e-6));

  snap.compute_gahm_parameters();

  REQUIRE_THAT(
      snap.quadrant(0).isotach(0).gahm_parameters().radius_to_max_winds(),
      WithinAbs(28908.370764, 1e-6));
  REQUIRE_THAT(snap.quadrant(0).isotach(0).gahm_parameters().gahm_b(),
               WithinAbs(0.276342, 1e-6));
  REQUIRE_THAT(snap.quadrant(0).isotach(0).gahm_parameters().gahm_phi(),
               WithinAbs(1.3820045, 1e-6));
  REQUIRE_THAT(snap.quadrant(0).isotach(1).gahm_parameters().holland_b(),
               WithinAbs(0.3252617, 1e-6));

  REQUIRE_THAT(
      snap.quadrant(0).isotach(1).gahm_parameters().radius_to_max_winds(),
      WithinAbs(92599.768364, 1e-6));
  REQUIRE_THAT(snap.quadrant(0).isotach(1).gahm_parameters().gahm_b(),
               WithinAbs(0.4738021, 1e-6));
  REQUIRE_THAT(snap.quadrant(0).isotach(1).gahm_parameters().gahm_phi(),
               WithinAbs(1.511737, 1e-6));

  REQUIRE_THAT(
      snap.quadrant(1).isotach(0).gahm_parameters().radius_to_max_winds(),
      WithinAbs(9751.519382, 1e-6));
  REQUIRE_THAT(snap.quadrant(1).isotach(0).gahm_parameters().gahm_b(),
               WithinAbs(0.245111, 1e-6));
  REQUIRE_THAT(snap.quadrant(1).isotach(0).gahm_parameters().gahm_phi(),
               WithinAbs(1.156206, 1e-6));
  REQUIRE_THAT(snap.quadrant(1).isotach(0).gahm_parameters().holland_b(),
               WithinAbs(0.23313339, 1e-6));

  REQUIRE_THAT(
      snap.quadrant(1).isotach(1).gahm_parameters().radius_to_max_winds(),
      WithinAbs(111119.723770, 1e-6));
  REQUIRE_THAT(snap.quadrant(1).isotach(1).gahm_parameters().gahm_b(),
               WithinAbs(0.41762, 1e-6));
  REQUIRE_THAT(snap.quadrant(1).isotach(1).gahm_parameters().gahm_phi(),
               WithinAbs(1.740439, 1e-6));
  REQUIRE_THAT(snap.quadrant(1).isotach(1).gahm_parameters().holland_b(),
               WithinAbs(0.239447, 1e-6));

  REQUIRE_THAT(
      snap.quadrant(2).isotach(0).gahm_parameters().radius_to_max_winds(),
      WithinAbs(166679.583757, 1e-6));
  REQUIRE_THAT(snap.quadrant(2).isotach(0).gahm_parameters().gahm_b(),
               WithinAbs(0.466707, 1e-6));
  REQUIRE_THAT(snap.quadrant(2).isotach(0).gahm_parameters().gahm_phi(),
               WithinAbs(1.893838, 1e-6));
  REQUIRE_THAT(snap.quadrant(2).isotach(0).gahm_parameters().holland_b(),
               WithinAbs(0.210740, 1e-6));

  REQUIRE_THAT(
      snap.quadrant(3).isotach(0).gahm_parameters().radius_to_max_winds(),
      WithinAbs(92599.769089, 1e-6));
  REQUIRE_THAT(snap.quadrant(3).isotach(0).gahm_parameters().gahm_b(),
               WithinAbs(0.4824338, 1e-6));
  REQUIRE_THAT(snap.quadrant(3).isotach(0).gahm_parameters().gahm_phi(),
               WithinAbs(1.497667, 1e-6));
  REQUIRE_THAT(snap.quadrant(3).isotach(0).gahm_parameters().holland_b(),
               WithinAbs(0.333795, 1e-6));
}

// NOLINTEND(cppcoreguidelines-macro-usage,
// cppcoreguidelines-avoid-magic-numbers, misc-use-anonymous-namespace,
// readability-function-cognitive-complexity, cert-err58-cpp,
// readability-magic-numbers)