//
// Created by Zach Cobell on 3/14/23.
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
#include "physical/Earth.h"
#include "preprocessor/Preprocessor.h"
#include "vortex/Vortex.h"

//TEST_CASE("Quadrant Selection", "[Vortex]") {
//  constexpr double deg2rad = M_PI / 180.0;
//
//  // clang-format off
//   std::vector<double> azi = {0,    45,  90, 135, 180, 225, 270, 315, 360};
//   std::vector<int> quad =   {0,     0,   1,   1,   2,   2,   3,   3,   0};
//   std::vector<double> rem = {0.0, 0.5, 0.0, 0.5, 0.0, 0.5, 0.0, 0.5, 0.0};
//  // clang-format on
//
//  for (size_t i = 0; i < azi.size(); ++i) {
//    auto [quad0, rem0] = Gahm::Vortex::getBaseQuadrant(azi[i] * deg2rad);
//    std::cout << "Testing that " << azi[i] << " is in quadrant " << quad[i]
//              << " with quadrant weight " << rem[i] << std::endl;
//    REQUIRE(quad0 == quad[i]);
//    REQUIRE(rem0 == Catch::Approx(rem[i]));
//  }
//}

// TEST_CASE("Isotach Selection", "[Vortex]") {
//   const std::string filename = "test_files/bal122005.dat";
//   auto atcf = Gahm::Atcf::AtcfFile(filename);
//   atcf.read();
//
//   Gahm::Preprocessor prep(&atcf);
//   prep.prepareAtcfData();
//   prep.solve();
//
//   Gahm::Datatypes::Point p(-82.05, 25.2);
//   auto pc = Gahm::Datatypes::PointCloud();
//   pc.addPoint(p);
//   auto v = Gahm::Vortex::Vortex(&atcf, pc);
//
//   auto [it, time_weight] =
//       v.selectTimeIndex(Gahm::Datatypes::Date(2005, 8, 26, 12, 0, 0));
//   auto snap = *it;
//
//
//   auto da =
//       Gahm::Vortex::Vortex::distanceAndAzimuth(p, snap.position().point());
//   auto diso = Gahm::Vortex::Vortex::findQuadrantAndIsotach(snap, da);
//
//   REQUIRE(diso.quadrant == 1);
//   REQUIRE(diso.isotach == 0);
//
//   std::cout << diso.isotach << " " << da.distance << std::endl;
//   auto q= snap.getIsotachs()[0].getQuadrant(1);
//   std::cout << q << std::endl;
//
//   REQUIRE(1==0);
//
// }

TEST_CASE("Vortex", "[vortex]") {
  Gahm::Datatypes::WindGrid wg = Gahm::Datatypes::WindGrid::fromCorners(
      -87.0, 22.0, -78.0, 29.0, 0.01, 0.01);

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

  auto check_time = Gahm::Datatypes::Date(2005, 8, 26, 12, 0, 0);
  const auto [track_test_it, tack_test_weight] = v.selectTime(check_time);

  auto check_point = wg.points()[2];
  auto point_position = Gahm::Vortex::getPointPosition(
      check_point, track_test_it->position(), *track_test_it);
  std::cout
      << fmt::format(
             "Checking distance and azimuth between {:f}, {:f} and {:f}, {:f}",
             check_point.x(), check_point.y(),
             track_test_it->position().point().x(),
             track_test_it->position().point().y())
      << std::endl;
  REQUIRE(point_position.distance == Catch::Approx(691645.9377152371));
  REQUIRE(point_position.azimuth * Gahm::Physical::Constants::rad2deg() ==
          Catch::Approx(55.9489916919));

  auto solution = v.solve(check_time);

  REQUIRE(solution.uvp().size() == wg.points().size());

  std::ofstream out("vortex_solution.txt");
  size_t index = 0;
  auto points = wg.points();
  for (const auto &s : solution.uvp()) {
    auto x = points[index].x();
    auto y = points[index].y();
    auto mag = std::sqrt(s.u * s.u + s.v * s.v);
    out << fmt::format("{:f} {:f} {:f} {:f} {:f} {:f}\n", x, y, mag, s.u, s.v,
                       s.p);
    index++;
  }
  out.close();
}