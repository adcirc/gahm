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
      track_test_it->position().point(), check_point);
  auto azimuth = Gahm::Physical::Earth::azimuth(
      track_test_it->position().point(), check_point);
  REQUIRE(distance == Catch::Approx(1242459.4171680384));
  REQUIRE(azimuth * Gahm::Physical::Constants::rad2deg() ==
          Catch::Approx(250.8907884234));

  auto solution = v.solve(check_time);
  REQUIRE(solution.uvp().size() == wg.points().size());

  const std::vector<size_t> sampling_points = {
      8388, 3140, 6198, 5268, 7132, 5813, 14660, 9560, 21931, 14990, 11451};
  const std::vector<Gahm::Datatypes::VortexSolution::t_uvp> sampling_solution =
      {{-4.645162, -8.131252, 1010.020649},
       {1.620713, -1.946252, 1012.055274},
       {-1.104336, -3.307435, 1011.654450},
       {1.248558, -5.732596, 1011.122958},
       {8.716328, -5.224886, 1010.218869},
       {3.686932, -1.459382, 1011.777583},
       {-6.582892, 9.597902, 1010.185487},
       {-3.275348, -33.613207, 1000.841223},
       {-0.076416, 0.625650, 1012.725300},
       {-2.939382, 1.107471, 1012.110355},
       {-7.498839, 71.424834, 955.991428}};

  for (size_t i = 0; i < sampling_points.size(); ++i) {
    auto index = sampling_points[i];
    auto sol = solution.uvp()[index];
    auto expected = sampling_solution[i];
    REQUIRE(sol.u == Catch::Approx(expected.u));
    REQUIRE(sol.v == Catch::Approx(expected.v));
    REQUIRE(sol.p == Catch::Approx(expected.p));
  }

  //...Used when updating the solution to get the data back out
  //  std::ofstream out("vortex_solution.txt");
  //  size_t index = 0;
  //  auto points = wg.points();
  //  for (const auto &s : solution.uvp()) {
  //    auto x = points[index].x();
  //    auto y = points[index].y();
  //    auto mag = std::sqrt(s.u * s.u + s.v * s.v);
  //    out << fmt::format("{:f} {:f} {:f} {:f} {:f} {:f}\n", x, y, mag, s.u,
  //    s.v,
  //                       s.p);
  //    index++;
  //  }
  //  out.close();
}