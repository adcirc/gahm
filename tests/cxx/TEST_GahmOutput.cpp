//
// Created by Zach Cobell on 8/22/24.
//

#include <fstream>

#include "atcf/AtcfIO.hpp"
#include "atcf/AtcfPeriod.hpp"
#include "catch2/catch_test_macros.hpp"
#include "catch2/matchers/catch_matchers_floating_point.hpp"
#include "datatypes/Datetime.hpp"
#include "fmt/core.h"
#include "output/PointOutput.hpp"
#include "output/RadialProfile.hpp"
#include "physical/Units.hpp"
#include "plotting/AtcfPeriodPlot.hpp"

// NOLINTBEGIN(cppcoreguidelines-macro-usage,
// cppcoreguidelines-avoid-magic-numbers, misc-use-anonymous-namespace,
// readability-function-cognitive-complexity, cert-err58-cpp,
// readability-magic-numbers)

using namespace Catch;
using namespace Catch::Matchers;

auto generate_test_snap_1() -> Gahm::Atcf::AtcfPeriod {
  constexpr auto mb2pa = Gahm::Physical::Units::convert(
      Gahm::Physical::Units::Millibar, Gahm::Physical::Units::Pascal);
  constexpr auto kt2ms = Gahm::Physical::Units::convert(
      Gahm::Physical::Units::Knot, Gahm::Physical::Units::MetersPerSecond);
  constexpr auto nmi2m = Gahm::Physical::Units::convert(
      Gahm::Physical::Units::NauticalMile, Gahm::Physical::Units::Meter);

  const Gahm::Types::Datetime previous_date("2018091318");
  const Gahm::Types::Datetime snap_date("2018091400");
  const auto dt =
      snap_date.seconds_since_epoch() - previous_date.seconds_since_epoch();
  const double p_c = 952.0 * mb2pa;
  const double p_bk = 1013.0 * mb2pa;
  const double v_max = 90.0 * kt2ms;
  const double r_max = 20.0 * nmi2m;
  const Gahm::Types::Point eye_location(-76.5, 34.0);
  const Gahm::Types::Point previous_eye_location(-76.0, 33.6);
  const Gahm::Storm::StormTranslation translation(eye_location,
                                                  previous_eye_location, dt);
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

  Gahm::Atcf::AtcfPeriod snap(snap_date, p_c, p_bk, v_max, r_max, eye_location,
                              quadrants);
  snap.set_translation(translation);
  snap.compute_gahm_parameters();

  return snap;
}

auto read_control_profile(const std::string &filename)
    -> Gahm::Output::RadialProfile::Profile {
  std::ifstream control_file(filename);

  // Allow us to run from an alternate directory if needed
  if (!control_file.good()) {
    const auto filename_alternate = "../tests/" + filename.substr(3);
    control_file.open(filename_alternate);
  }

  // Check if the file exists
  REQUIRE(control_file.good());

  Gahm::Output::RadialProfile::Profile control_profile;
  std::string line;
  while (std::getline(control_file, line)) {
    std::istringstream line_stream(line);
    std::string distance_str;
    std::string pressure_str;
    std::string wind_speed_str;
    std::string wind_dir_str;
    std::getline(line_stream, distance_str, ',');
    std::getline(line_stream, pressure_str, ',');
    std::getline(line_stream, wind_speed_str, ',');
    std::getline(line_stream, wind_dir_str, ',');
    const double distance = std::stod(distance_str);
    const double pressure = std::stod(pressure_str);
    const double wind_speed = std::stod(wind_speed_str);
    const double wind_dir = std::stod(wind_dir_str);
    const Gahm::Types::Vec wind_vector(wind_speed, wind_dir);
    control_profile.data.push_back(Gahm::Output::RadialProfile::ProfilePoint{
        distance, pressure, wind_vector});
  }

  return control_profile;
}

auto read_control_point_solution(const std::string &filename) {
  std::ifstream control_file(filename);

  // Allow us to run from an alternate directory if needed
  if (!control_file.good()) {
    const auto filename_alternate = "../tests/" + filename.substr(3);
    control_file.open(filename_alternate);
  }

  REQUIRE(control_file.good());

  Gahm::Output::PointOutput::PointSolution control_solution;
  std::string line;
  while (std::getline(control_file, line)) {
    std::istringstream line_stream(line);
    std::string x_str;
    std::string y_str;
    std::string pressure_str;
    std::string u_str;
    std::string v_str;
    std::getline(line_stream, x_str, ',');
    std::getline(line_stream, y_str, ',');
    std::getline(line_stream, pressure_str, ',');
    std::getline(line_stream, u_str, ',');
    std::getline(line_stream, v_str, ',');
    // const double x = std::stod(x_str);
    // const double y = std::stod(y_str);
    const double pressure = std::stod(pressure_str);
    const double u = std::stod(u_str);
    const double v = std::stod(v_str);
    const Gahm::Types::Vec wind_vector(u, v);
    control_solution.data.push_back(
        Gahm::Output::PointOutput::SolutionPoint{pressure, wind_vector});
  }

  return control_solution;
}

auto generate_test_snap_2() -> Gahm::Atcf::AtcfPeriod {
  constexpr auto mb2pa = Gahm::Physical::Units::convert(
      Gahm::Physical::Units::Millibar, Gahm::Physical::Units::Pascal);
  constexpr auto kt2ms = Gahm::Physical::Units::convert(
      Gahm::Physical::Units::Knot, Gahm::Physical::Units::MetersPerSecond);
  constexpr auto nmi2m = Gahm::Physical::Units::convert(
      Gahm::Physical::Units::NauticalMile, Gahm::Physical::Units::Meter);

  const Gahm::Types::Datetime previous_date("2018091400");
  const Gahm::Types::Datetime snap_date("2018091406");
  const auto dt =
      snap_date.seconds_since_epoch() - previous_date.seconds_since_epoch();
  const double p_c = 952.0 * mb2pa;
  const double p_bk = 1013.0 * mb2pa;
  const double v_max = 90.0 * kt2ms;
  const double r_max = 20.0 * nmi2m;
  const Gahm::Types::Point eye_location(-77.2, 34.2);
  const Gahm::Types::Point previous_eye_location(-76.5, 34.0);
  const Gahm::Storm::StormTranslation translation(eye_location,
                                                  previous_eye_location, dt);
  const Gahm::Atcf::TempIsotach i34(
      34.0 * kt2ms, std::array<double, 4>{170 * nmi2m, 150 * nmi2m, 130 * nmi2m,
                                          100 * nmi2m});
  const Gahm::Atcf::TempIsotach i50(
      50.0 * kt2ms,
      std::array<double, 4>{100 * nmi2m, 80 * nmi2m, 80 * nmi2m, 70 * nmi2m});
  const Gahm::Atcf::TempIsotach i64(
      64.0 * kt2ms,
      std::array<double, 4>{70 * nmi2m, 60 * nmi2m, 60 * nmi2m, 50 * nmi2m});

  const auto quadrants = Gahm::Atcf::AtcfIO::transpose_to_quadrants(
      eye_location.y(), {i34, i50, i64});

  Gahm::Atcf::AtcfPeriod snap(snap_date, p_c, p_bk, v_max, r_max, eye_location,
                              quadrants);
  snap.set_translation(translation);
  snap.compute_gahm_parameters();

  return snap;
}

auto profile_comparison(
    const Gahm::Output::RadialProfile::Profile &control_profile,
    const Gahm::Output::RadialProfile::Profile &test_profile) {
  for (size_t i = 0; i < test_profile.data.size(); ++i) {
    const auto &p = test_profile.data[i];
    const auto &cp = control_profile.data[i];
    REQUIRE_THAT(p.distance, WithinRel(cp.distance, 0.001));
    REQUIRE_THAT(p.pressure, WithinRel(cp.pressure, 0.001));

    // We compare against the relative and absolute tolerance to account for
    // the wind speed because they may be close to zero
    REQUIRE_THAT(p.wind_vector.u(), WithinRel(cp.wind_vector.u(), 0.001) ||
                                        WithinAbs(cp.wind_vector.u(), 1e-6));
    REQUIRE_THAT(p.wind_vector.v(), WithinRel(cp.wind_vector.v(), 0.001) ||
                                        WithinAbs(cp.wind_vector.v(), 1e-6));
  }
}

TEST_CASE("OutputData", "[output]") {
  const auto snap = generate_test_snap_1();

  SECTION("Radial Profile") {
    const auto profile_ne = Gahm::Output::RadialProfile::get_profile(
        snap, Gahm::Types::QuadCode::QuadrantCode::NE, 0.0, 500000, 1000.0);
    const auto profile_nw = Gahm::Output::RadialProfile::get_profile(
        snap, Gahm::Types::QuadCode::QuadrantCode::NW, 0.0, 500000, 1000.0);
    const auto profile_sw = Gahm::Output::RadialProfile::get_profile(
        snap, Gahm::Types::QuadCode::QuadrantCode::SW, 0.0, 500000, 1000.0);
    const auto profile_se = Gahm::Output::RadialProfile::get_profile(
        snap, Gahm::Types::QuadCode::QuadrantCode::SE, 0.0, 500000, 1000.0);

#ifdef REWRITE_SOLUTION_FILES
    std::ofstream control_file;
    control_file.open(
        "../tests/test_files/control_solutions/test_sln_ne_profile.csv");
    for (const auto &p : profile_ne.data) {
      control_file << fmt::format("{:.8f},{:.8f},{:.8f},{:.8f}\n", p.distance,
                                  p.pressure, p.wind_vector.u(),
                                  p.wind_vector.v());
    }
    control_file.close();

    control_file.open(
        "../tests/test_files/control_solutions/test_sln_nw_profile.csv");
    for (const auto &p : profile_nw.data) {
      control_file << fmt::format("{:.8f},{:.8f},{:.8f},{:.8f}\n", p.distance,
                                  p.pressure, p.wind_vector.u(),
                                  p.wind_vector.v());
    }
    control_file.close();

    control_file.open(
        "../tests/test_files/control_solutions/test_sln_sw_profile.csv");
    for (const auto &p : profile_sw.data) {
      control_file << fmt::format("{:.8f},{:.8f},{:.8f},{:.8f}\n", p.distance,
                                  p.pressure, p.wind_vector.u(),
                                  p.wind_vector.v());
    }
    control_file.close();

    control_file.open(
        "../tests/test_files/control_solutions/test_sln_se_profile.csv");
    for (const auto &p : profile_se.data) {
      control_file << fmt::format("{:.8f},{:.8f},{:.8f},{:.8f}\n", p.distance,
                                  p.pressure, p.wind_vector.u(),
                                  p.wind_vector.v());
    }
    control_file.close();
#endif

    SECTION("NW Profile") {
      const auto control_profile = read_control_profile(
          "../test_files/control_solutions/test_sln_nw_profile.csv");
      REQUIRE(profile_nw.data.size() == control_profile.data.size());
      profile_comparison(control_profile, profile_nw);
    }

    SECTION("NE Profile") {
      const auto control_profile = read_control_profile(
          "../test_files/control_solutions/test_sln_ne_profile.csv");
      REQUIRE(profile_ne.data.size() == control_profile.data.size());
      profile_comparison(control_profile, profile_ne);
    }

    SECTION("SW Profile") {
      const auto control_profile = read_control_profile(
          "../test_files/control_solutions/test_sln_sw_profile.csv");
      REQUIRE(profile_sw.data.size() == control_profile.data.size());
      profile_comparison(control_profile, profile_sw);
    }

    SECTION("SE Profile") {
      const auto control_profile = read_control_profile(
          "../test_files/control_solutions/test_sln_se_profile.csv");
      REQUIRE(profile_se.data.size() == control_profile.data.size());
      profile_comparison(control_profile, profile_se);
    }
  }
}

TEST_CASE("Point Output", "[output]") {
  const auto snap = generate_test_snap_1();

  SECTION("Point Output") {
    const auto x_init = snap.eye_location().x() - 2.0;
    const auto y_init = snap.eye_location().y() - 2.0;
    const auto x_end = snap.eye_location().x() + 2.0;
    const auto y_end = snap.eye_location().y() + 2.0;
    const auto grid = Gahm::Types::Grid::fromCorners(
        Gahm::Types::Point(x_init, y_init), Gahm::Types::Point(x_end, y_end),
        0.01, 0.01);

    const auto solution = Gahm::Output::PointOutput::get_points(snap, grid);

    REQUIRE(solution.data.size() == grid.size());

#ifdef REWRITE_SOLUTION_FILES
    // Write the solution to file using 6 decimal places
    std::ofstream control_file(
        "../tests/test_files/control_solutions/test_sln_point_output.csv");

    auto pts = grid.points();
    auto x_vec = pts.x();
    auto y_vec = pts.y();
    for (size_t i = 0; i < solution.data.size(); i++) {
      control_file << fmt::format("{:.8f},{:.8f},{:.8f},{:.8f},{:.8f}\n",
                                  x_vec[i], y_vec[i], solution.data[i].pressure,
                                  solution.data[i].wind_vector.u(),
                                  solution.data[i].wind_vector.v());
    }
    control_file.close();
#endif

    const auto control_solution = read_control_point_solution(
        "../test_files/control_solutions/test_sln_point_output.csv");

    for (size_t i = 0; i < solution.data.size(); ++i) {
      const auto &p = solution.data[i];
      const auto &cp = control_solution.data[i];
      REQUIRE_THAT(p.pressure, WithinRel(cp.pressure, 0.001));
      REQUIRE_THAT(p.wind_vector.u(), WithinRel(cp.wind_vector.u(), 0.001) ||
                                          WithinAbs(cp.wind_vector.u(), 1e-8));
      REQUIRE_THAT(p.wind_vector.v(), WithinRel(cp.wind_vector.v(), 0.001) ||
                                          WithinAbs(cp.wind_vector.v(), 1e-8));
    }
  }
}

TEST_CASE("Point Output Multi-snap", "[output]") {
  constexpr auto ms2kt = Gahm::Physical::Units::convert(
      Gahm::Physical::Units::MetersPerSecond, Gahm::Physical::Units::Knot);
  constexpr auto nmi2m = Gahm::Physical::Units::convert(
      Gahm::Physical::Units::NauticalMile, Gahm::Physical::Units::Meter);

  const auto snap_1 = generate_test_snap_1();
  const auto snap_2 = generate_test_snap_2();

  SECTION("Snap Interpolation") {
    const auto interp_date = Gahm::Types::Datetime("2018091403");
    const auto interp_snap =
        Gahm::Atcf::AtcfPeriod::interpolate(snap_1, snap_2, interp_date);

    REQUIRE(interp_snap.has_value());

    const auto interp = interp_snap.value();

    REQUIRE_THAT(interp.eye_location().x(), WithinRel(-76.85, 0.001));
    REQUIRE_THAT(interp.eye_location().y(), WithinRel(34.1, 0.001));
    REQUIRE_THAT(interp.central_pressure() / 100.0, WithinRel(952.0, 0.001));
    REQUIRE_THAT(interp.background_pressure() / 100.0,
                 WithinRel(1013.0, 0.001));
    REQUIRE_THAT(interp.v_max() * ms2kt, WithinRel(90.0, 0.001));
    REQUIRE_THAT(interp.r_max() / nmi2m, WithinRel(20.0, 0.001));

    const auto quadrants = interp.quadrants();
    REQUIRE(quadrants.size() == 4);
    REQUIRE_THAT(quadrants[0].isotach(0).wind_speed(),
                 WithinRel(34.0 / ms2kt, 0.001));
    REQUIRE_THAT(quadrants[0].isotach(0).radius() / nmi2m,
                 WithinRel(170.0, 0.001));
    REQUIRE_THAT(quadrants[0].isotach(1).wind_speed(),
                 WithinRel(50.0 / ms2kt, 0.001));
    REQUIRE_THAT(quadrants[0].isotach(1).radius() / nmi2m,
                 WithinRel(100.0, 0.001));
    REQUIRE_THAT(quadrants[0].isotach(2).wind_speed(),
                 WithinRel(64.0 / ms2kt, 0.001));
    REQUIRE_THAT(quadrants[0].isotach(2).radius() / nmi2m,
                 WithinRel(70.0, 0.001));

    REQUIRE_THAT(quadrants[1].isotach(0).wind_speed(),
                 WithinRel(34.0 / ms2kt, 0.001));
    REQUIRE_THAT(quadrants[1].isotach(0).radius() / nmi2m,
                 WithinRel(150.0, 0.001));
    REQUIRE_THAT(quadrants[1].isotach(1).wind_speed(),
                 WithinRel(50.0 / ms2kt, 0.001));
    REQUIRE_THAT(quadrants[1].isotach(1).radius() / nmi2m,
                 WithinRel(80.0, 0.001));
    REQUIRE_THAT(quadrants[1].isotach(2).wind_speed(),
                 WithinRel(64.0 / ms2kt, 0.001));
    REQUIRE_THAT(quadrants[1].isotach(2).radius() / nmi2m,
                 WithinRel(60.0, 0.001));

    REQUIRE_THAT(quadrants[2].isotach(0).wind_speed(),
                 WithinRel(34.0 / ms2kt, 0.001));
    REQUIRE_THAT(quadrants[2].isotach(0).radius() / nmi2m,
                 WithinRel(130.0, 0.001));
    REQUIRE_THAT(quadrants[2].isotach(1).wind_speed(),
                 WithinRel(50.0 / ms2kt, 0.001));
    REQUIRE_THAT(quadrants[2].isotach(1).radius() / nmi2m,
                 WithinRel(80.0, 0.001));
    REQUIRE_THAT(quadrants[2].isotach(2).wind_speed(),
                 WithinRel(64.0 / ms2kt, 0.001));
    REQUIRE_THAT(quadrants[2].isotach(2).radius() / nmi2m,
                 WithinRel(55.0, 0.001));

    REQUIRE_THAT(quadrants[3].isotach(0).wind_speed(),
                 WithinRel(34.0 / ms2kt, 0.001));
    REQUIRE_THAT(quadrants[3].isotach(0).radius() / nmi2m,
                 WithinRel(100.0, 0.001));
    REQUIRE_THAT(quadrants[3].isotach(1).wind_speed(),
                 WithinRel(50.0 / ms2kt, 0.001));
    REQUIRE_THAT(quadrants[3].isotach(1).radius() / nmi2m,
                 WithinRel(70.0, 0.001));
    REQUIRE_THAT(quadrants[3].isotach(2).wind_speed(),
                 WithinRel(64.0 / ms2kt, 0.001));
    REQUIRE_THAT(quadrants[3].isotach(2).radius() / nmi2m,
                 WithinRel(50.0, 0.001));

    // Lastly, check the radius to max winds
    REQUIRE_THAT(
        quadrants[0].isotach(0).gahm_parameters().radius_to_max_winds() / nmi2m,
        WithinRel(25.202015, 0.001));
    REQUIRE_THAT(
        quadrants[0].isotach(1).gahm_parameters().radius_to_max_winds() / nmi2m,
        WithinRel(21.278301, 0.001));
    REQUIRE_THAT(
        quadrants[0].isotach(2).gahm_parameters().radius_to_max_winds() / nmi2m,
        WithinRel(21.969396, 0.001));

    REQUIRE_THAT(
        quadrants[1].isotach(0).gahm_parameters().radius_to_max_winds() / nmi2m,
        WithinRel(23.063446, 0.001));
    REQUIRE_THAT(
        quadrants[1].isotach(1).gahm_parameters().radius_to_max_winds() / nmi2m,
        WithinRel(18.113743, 0.001));
    REQUIRE_THAT(
        quadrants[1].isotach(2).gahm_parameters().radius_to_max_winds() / nmi2m,
        WithinRel(21.752882, 0.001));

    REQUIRE_THAT(
        quadrants[2].isotach(0).gahm_parameters().radius_to_max_winds() / nmi2m,
        WithinRel(21.136460, 0.001));
    REQUIRE_THAT(
        quadrants[2].isotach(1).gahm_parameters().radius_to_max_winds() / nmi2m,
        WithinRel(21.708920, 0.001));
    REQUIRE_THAT(
        quadrants[2].isotach(2).gahm_parameters().radius_to_max_winds() / nmi2m,
        WithinRel(24.634652, 0.001));

    REQUIRE_THAT(
        quadrants[3].isotach(0).gahm_parameters().radius_to_max_winds() / nmi2m,
        WithinRel(12.010748, 0.001));
    REQUIRE_THAT(
        quadrants[3].isotach(1).gahm_parameters().radius_to_max_winds() / nmi2m,
        WithinRel(15.032159, 0.001));
    REQUIRE_THAT(
        quadrants[3].isotach(2).gahm_parameters().radius_to_max_winds() / nmi2m,
        WithinRel(17.312822, 0.001));
  }

  SECTION("Point Output") {
    const auto x_init =
        ((snap_1.eye_location().x() + snap_2.eye_location().x()) / 2.0) - 2.0;
    const auto y_init =
        ((snap_1.eye_location().y() + snap_2.eye_location().y()) / 2.0) - 2.0;
    const auto x_end =
        ((snap_1.eye_location().x() + snap_2.eye_location().x()) / 2.0) + 2.0;
    const auto y_end =
        ((snap_1.eye_location().y() + snap_2.eye_location().y()) / 2.0) + 2.0;
    const auto grid = Gahm::Types::Grid::fromCorners(
        Gahm::Types::Point(x_init, y_init), Gahm::Types::Point(x_end, y_end),
        0.01, 0.01);

    const Gahm::Types::Datetime interp_date("2018091403");

    const auto solution = Gahm::Output::PointOutput::get_points(
        snap_1, snap_2, interp_date, grid);

    REQUIRE(solution.data.size() == grid.size());
  }
}

// NOLINTEND(cppcoreguidelines-macro-usage,
// cppcoreguidelines-avoid-magic-numbers, misc-use-anonymous-namespace,
// readability-function-cognitive-complexity, cert-err58-cpp,
// readability-magic-numbers)