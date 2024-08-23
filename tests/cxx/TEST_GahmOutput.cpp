//
// Created by Zach Cobell on 8/22/24.
//

#include <fstream>

#include "atcf/AtcfIO.h"
#include "atcf/AtcfPeriod.h"
#include "catch2/catch_approx.hpp"
#include "catch2/catch_test_macros.hpp"
#include "datatypes/Datetime.h"
#include "output/PointOutput.h"
#include "output/RadialProfile.h"
#include "physical/Units.h"
#include "plotting/AtcfPeriodPlot.h"

// NOLINTBEGIN(cppcoreguidelines-macro-usage,
// cppcoreguidelines-avoid-magic-numbers, misc-use-anonymous-namespace,
// readability-function-cognitive-complexity, cert-err58-cpp,
// readability-magic-numbers)

using namespace Catch;

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
  const Gahm::Atcf::AtcfIO::TempIsotach i34(
      34.0 * kt2ms, std::array<double, 4>{170 * nmi2m, 150 * nmi2m, 130 * nmi2m,
                                          100 * nmi2m});
  const Gahm::Atcf::AtcfIO::TempIsotach i50(
      50.0 * kt2ms,
      std::array<double, 4>{100 * nmi2m, 80 * nmi2m, 80 * nmi2m, 70 * nmi2m});
  const Gahm::Atcf::AtcfIO::TempIsotach i64(
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
      control_file << p.distance << "," << p.pressure << ","
                   << p.wind_vector.u() << "," << p.wind_vector.v() << "\n";
    }
    control_file.close();

    control_file.open(
        "../tests/test_files/control_solutions/test_sln_nw_profile.csv");
    for (const auto &p : profile_nw.data) {
      control_file << p.distance << "," << p.pressure << ","
                   << p.wind_vector.u() << "," << p.wind_vector.v() << "\n";
    }
    control_file.close();

    control_file.open(
        "../tests/test_files/control_solutions/test_sln_sw_profile.csv");
    for (const auto &p : profile_sw.data) {
      control_file << p.distance << "," << p.pressure << ","
                   << p.wind_vector.u() << "," << p.wind_vector.v() << "\n";
    }
    control_file.close();

    control_file.open(
        "../tests/test_files/control_solutions/test_sln_se_profile.csv");
    for (const auto &p : profile_se.data) {
      control_file << p.distance << "," << p.pressure << ","
                   << p.wind_vector.u() << "," << p.wind_vector.v() << "\n";
    }
    control_file.close();
#endif

    SECTION("NW Profile") {
      const auto control_profile = read_control_profile(
          "../test_files/control_solutions/test_sln_nw_profile.csv");
      REQUIRE(profile_nw.data.size() == control_profile.data.size());
      for (size_t i = 0; i < profile_nw.data.size(); ++i) {
        const auto &p = profile_nw.data[i];
        const auto &cp = control_profile.data[i];
        REQUIRE(p.distance == Approx(cp.distance));
        REQUIRE(p.pressure == Approx(cp.pressure));
        REQUIRE(p.wind_vector.u() == Approx(cp.wind_vector.u()));
        REQUIRE(p.wind_vector.v() == Approx(cp.wind_vector.v()));
      }
    }

    SECTION("NE Profile") {
      const auto control_profile = read_control_profile(
          "../test_files/control_solutions/test_sln_ne_profile.csv");
      REQUIRE(profile_ne.data.size() == control_profile.data.size());
      for (size_t i = 0; i < profile_ne.data.size(); ++i) {
        const auto &p = profile_ne.data[i];
        const auto &cp = control_profile.data[i];
        REQUIRE(p.distance == Approx(cp.distance));
        REQUIRE(p.pressure == Approx(cp.pressure));
        REQUIRE(p.wind_vector.u() == Approx(cp.wind_vector.u()));
        REQUIRE(p.wind_vector.v() == Approx(cp.wind_vector.v()));
      }
    }

    SECTION("SW Profile") {
      const auto control_profile = read_control_profile(
          "../test_files/control_solutions/test_sln_sw_profile.csv");
      REQUIRE(profile_sw.data.size() == control_profile.data.size());
      for (size_t i = 0; i < profile_sw.data.size(); ++i) {
        const auto &p = profile_sw.data[i];
        const auto &cp = control_profile.data[i];
        REQUIRE(p.distance == Approx(cp.distance));
        REQUIRE(p.pressure == Approx(cp.pressure));
        REQUIRE(p.wind_vector.u() == Approx(cp.wind_vector.u()));
        REQUIRE(p.wind_vector.v() == Approx(cp.wind_vector.v()));
      }
    }

    SECTION("SE Profile") {
      const auto control_profile = read_control_profile(
          "../test_files/control_solutions/test_sln_se_profile.csv");
      REQUIRE(profile_se.data.size() == control_profile.data.size());
      for (size_t i = 0; i < profile_se.data.size(); ++i) {
        const auto &p = profile_se.data[i];
        const auto &cp = control_profile.data[i];
        REQUIRE(p.distance == Approx(cp.distance));
        REQUIRE(p.pressure == Approx(cp.pressure));
        REQUIRE(p.wind_vector.u() == Approx(cp.wind_vector.u()));
        REQUIRE(p.wind_vector.v() == Approx(cp.wind_vector.v()));
      }
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
    // Write the solution to file
    std::ofstream control_file(
        "../tests/test_files/control_solutions/test_sln_point_output.csv");
    auto pts = grid.points();
    auto x_vec = pts.x();
    auto y_vec = pts.y();
    for (size_t i = 0; i < solution.data.size(); i++) {
      control_file << x_vec[i] << "," << y_vec[i] << ","
                   << solution.data[i].pressure << ","
                   << solution.data[i].wind_vector.u() << ","
                   << solution.data[i].wind_vector.v() << "\n";
    }
#endif

    const auto control_solution = read_control_point_solution(
        "../test_files/control_solutions/test_sln_point_output.csv");

    for (size_t i = 0; i < solution.data.size(); ++i) {
      const auto &p = solution.data[i];
      const auto &cp = control_solution.data[i];
      REQUIRE(p.pressure == Approx(cp.pressure));
      REQUIRE(p.wind_vector.u() == Approx(cp.wind_vector.u()));
      REQUIRE(p.wind_vector.v() == Approx(cp.wind_vector.v()));
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

    REQUIRE(interp.eye_location().x() == Approx(-76.85));
    REQUIRE(interp.eye_location().y() == Approx(34.1));
    REQUIRE(interp.central_pressure() / 100.0 == Approx(952.0));
    REQUIRE(interp.background_pressure() / 100.0 == Approx(1013.0));
    REQUIRE(interp.v_max() * ms2kt == Approx(90.0));
    REQUIRE(interp.r_max() / nmi2m == Approx(20.0));

    const auto quadrants = interp.quadrants();
    REQUIRE(quadrants.size() == 4);
    REQUIRE(quadrants[0].isotach(0).wind_speed() == Approx(34.0 / ms2kt));
    REQUIRE(quadrants[0].isotach(0).radius() / nmi2m == Approx(170.0));
    REQUIRE(quadrants[0].isotach(1).wind_speed() == Approx(50.0 / ms2kt));
    REQUIRE(quadrants[0].isotach(1).radius() / nmi2m == Approx(100.0));
    REQUIRE(quadrants[0].isotach(2).wind_speed() == Approx(64.0 / ms2kt));
    REQUIRE(quadrants[0].isotach(2).radius() / nmi2m == Approx(70.0));

    REQUIRE(quadrants[1].isotach(0).wind_speed() == Approx(34.0 / ms2kt));
    REQUIRE(quadrants[1].isotach(0).radius() / nmi2m == Approx(150.0));
    REQUIRE(quadrants[1].isotach(1).wind_speed() == Approx(50.0 / ms2kt));
    REQUIRE(quadrants[1].isotach(1).radius() / nmi2m == Approx(80.0));
    REQUIRE(quadrants[1].isotach(2).wind_speed() == Approx(64.0 / ms2kt));
    REQUIRE(quadrants[1].isotach(2).radius() / nmi2m == Approx(60.0));

    REQUIRE(quadrants[2].isotach(0).wind_speed() == Approx(34.0 / ms2kt));
    REQUIRE(quadrants[2].isotach(0).radius() / nmi2m == Approx(130.0));
    REQUIRE(quadrants[2].isotach(1).wind_speed() == Approx(50.0 / ms2kt));
    REQUIRE(quadrants[2].isotach(1).radius() / nmi2m == Approx(80.0));
    REQUIRE(quadrants[2].isotach(2).wind_speed() == Approx(64.0 / ms2kt));
    REQUIRE(quadrants[2].isotach(2).radius() / nmi2m == Approx(55.0));

    REQUIRE(quadrants[3].isotach(0).wind_speed() == Approx(34.0 / ms2kt));
    REQUIRE(quadrants[3].isotach(0).radius() / nmi2m == Approx(100.0));
    REQUIRE(quadrants[3].isotach(1).wind_speed() == Approx(50.0 / ms2kt));
    REQUIRE(quadrants[3].isotach(1).radius() / nmi2m == Approx(70.0));
    REQUIRE(quadrants[3].isotach(2).wind_speed() == Approx(64.0 / ms2kt));
    REQUIRE(quadrants[3].isotach(2).radius() / nmi2m == Approx(50.0));

    // Lastly, check the radius to max winds
    REQUIRE(quadrants[0].isotach(0).gahm_parameters().radius_to_max_winds() /
                nmi2m ==
            Approx(25.2020));
    REQUIRE(quadrants[0].isotach(1).gahm_parameters().radius_to_max_winds() /
                nmi2m ==
            Approx(21.2783));
    REQUIRE(quadrants[0].isotach(2).gahm_parameters().radius_to_max_winds() /
                nmi2m ==
            Approx(21.9693));

    REQUIRE(quadrants[1].isotach(0).gahm_parameters().radius_to_max_winds() /
                nmi2m ==
            Approx(23.0634));
    REQUIRE(quadrants[1].isotach(1).gahm_parameters().radius_to_max_winds() /
                nmi2m ==
            Approx(18.1137));
    REQUIRE(quadrants[1].isotach(2).gahm_parameters().radius_to_max_winds() /
                nmi2m ==
            Approx(21.7528));

    REQUIRE(quadrants[2].isotach(0).gahm_parameters().radius_to_max_winds() /
                nmi2m ==
            Approx(21.1364));
    REQUIRE(quadrants[2].isotach(1).gahm_parameters().radius_to_max_winds() /
                nmi2m ==
            Approx(21.7089));
    REQUIRE(quadrants[2].isotach(2).gahm_parameters().radius_to_max_winds() /
                nmi2m ==
            Approx(24.6346));

    REQUIRE(quadrants[3].isotach(0).gahm_parameters().radius_to_max_winds() /
                nmi2m ==
            Approx(12.0107));
    REQUIRE(quadrants[3].isotach(1).gahm_parameters().radius_to_max_winds() /
                nmi2m ==
            Approx(15.0321));
    REQUIRE(quadrants[3].isotach(2).gahm_parameters().radius_to_max_winds() /
                nmi2m ==
            Approx(17.3128));
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