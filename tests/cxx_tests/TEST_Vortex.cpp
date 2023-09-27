// GNU General Public License v3.0
//
// This file is part of the GAHM model (https://github.com/adcirc/gahm).
// Copyright (c) 2023 ADCIRC Development Group.
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, version 3.
//
// This program is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program. If not, see <http://www.gnu.org/licenses/>.
//
// Author: Zach Cobell
// Contact: zcobell@thewaterinstitute.org
//

#include <algorithm>
#include <fstream>
#include <iterator>
#include <tuple>

#include "atcf/AtcfFile.h"
#include "catch2/catch_approx.hpp"
#include "catch2/catch_test_macros.hpp"
#include "datatypes/WindGrid.h"
#include "fmt/core.h"
#include "output/OwiOutput.h"
#include "physical/Earth.h"
#include "preprocessor/Preprocessor.h"
#include "vortex/Vortex.h"

TEST_CASE("Quadrant Selection", "[Vortex]") {
  constexpr double deg2rad = M_PI / 180.0;

  // clang-format off
    std::vector<double> azi = {0,   45, 90, 135, 180, 225, 270, 315, 360};
    std::vector<int> quad =   {0,    1,  1,   1,   2,   2,   3,   3,   0};
    std::vector<double> rem = {45, 0.0, 45,  90,  45,  90,  45,  90,  45};
  // clang-format on

  for (size_t i = 0; i < azi.size(); ++i) {
    auto [quad0, rem0] = Gahm::Vortex::getBaseQuadrant(azi[i] * deg2rad);
    std::cout << "Testing that storm azimuth " << azi[i] << " is in quadrant "
              << quad[i] << " with delta angle " << rem[i] << std::endl;
    std::cout << "  Actual: " << quad0 << " " << rem0 * deg2rad << std::endl;
    REQUIRE(quad0 == quad[i]);
    REQUIRE(rem0 == Catch::Approx(rem[i] * deg2rad));
  }
}

TEST_CASE("Vortex", "[vortex]") {
  Gahm::Datatypes::WindGrid wg = Gahm::Datatypes::WindGrid::fromCorners(
      -100.0, 22.0, -78.0, 32.0, 0.1, 0.1);

  const std::string filename = "test_files/bal122005.dat";
  auto atcf = Gahm::Atcf::AtcfFile(filename);
  atcf.read();

  Gahm::Preprocessor prep(&atcf);
  prep.prepareAtcfData();
  prep.solve();

  auto v = Gahm::Vortex(&atcf, wg.points());

  Gahm::Datatypes::Date startDate(2005, 8, 23, 18, 0, 0);
  Gahm::Datatypes::Date endDate(2005, 8, 30, 12, 0, 0);
  Gahm::Datatypes::Date middleDate(2005, 8, 26, 11, 0, 0);

  const auto [track_start_it, track_start_weight] = v.selectTime(startDate);
  const auto [track_end_it, track_end_weight] = v.selectTime(endDate);
  const auto [track_middle_it, track_middle_weight] = v.selectTime(middleDate);

  REQUIRE(track_start_it->date() == atcf.data().begin()->date());
  REQUIRE(track_start_weight == 0.0);

  REQUIRE(track_end_it->date() == std::prev(atcf.data().end())->date());
  REQUIRE(track_end_weight == 1.0);

  REQUIRE(track_middle_it->date() == atcf.data()[10].date());
  REQUIRE(track_middle_weight == Catch::Approx(0.8333333));

  auto check_time = Gahm::Datatypes::Date(2005, 8, 29, 0, 0, 0);
  const auto [track_test_it, tack_test_weight] = v.selectTime(check_time);

  auto check_point = wg.points()[2];
  auto distance = Gahm::Physical::Earth::distance(
      check_point, track_test_it->position().point());
  auto azimuth = Gahm::Physical::Earth::azimuth(
      check_point, track_test_it->position().point());
  REQUIRE(distance == Catch::Approx(1242459.4171680384));
  REQUIRE(azimuth * Gahm::Physical::Constants::rad2deg() ==
          Catch::Approx(246.1927110799));

  auto solution = v.solve(check_time);
  REQUIRE(solution.uvp().size() == wg.points().size());

  const std::vector<size_t> sampling_points = {
      8388, 3140, 6198, 5268, 7132, 5813, 14660, 9560, 21931, 14990, 11451};

  // clang-format off
  const std::vector<Gahm::Datatypes::Uvp> sampling_solution =
      {{-3.545647,  -6.420195, 1009.456382},
       {1.452037,   -1.700460, 1011.704127},
       {-1.023131,  -3.231597, 1010.912514},
       {1.138791,   -4.860846, 1010.530213},
       {5.111922,   -3.006727, 1010.788160},
       {2.586431,   -1.005869, 1011.713863},
       {-8.760438,  13.135478, 1006.263273},
       {-1.926749, -23.747142, 1002.827237},
       {-0.188428,   1.768306, 1011.776616},
       {-6.725148,   2.578584, 1009.484907},
       {-9.240353,  54.034793,  965.549211}};
  // clang-format on

  REQUIRE(sampling_points.size() == sampling_solution.size());

  for (size_t i = 0; i < sampling_points.size(); ++i) {
    auto index = sampling_points[i];
    auto sol = solution.uvp()[index];
    auto expected = sampling_solution[i];
    REQUIRE(sol.u() == Catch::Approx(expected.u()));
    REQUIRE(sol.v() == Catch::Approx(expected.v()));
    REQUIRE(sol.p() == Catch::Approx(expected.p()));
    //    std::cout << "{"
    //              << fmt::format("{:f}, {:f}, {:f}", sol.u(), sol.v(),
    //              sol.p())
    //              << "}," << std::endl;
  }

  //...Used when updating the solution to get the data back out
  std::ofstream out("vortex_solution.txt");
  size_t index = 0;
  auto points = wg.points();
  for (const auto &s : solution.uvp()) {
    auto x = points[index].x();
    auto y = points[index].y();
    auto mag = std::sqrt(s.u() * s.u() + s.v() * s.v());
    out << fmt::format("{:d} {:f} {:f} {:f} {:f} {:f} {:f}\n", index, x, y, mag,
                       s.u(), s.v(), s.p());
    index++;
  }
  out.close();
}