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
#include <array>
#include <memory>

#include "catch2/catch_approx.hpp"
#include "catch2/catch_test_macros.hpp"
#include "gahm.h"

TEST_CASE("FillMissingData", "[Preprocessor]") {
  auto atcf =
      std::make_unique<Gahm::Atcf::AtcfFile>("test_files/bal122005.dat");
  atcf->read();
  auto preprocessor = std::make_unique<Gahm::Preprocessor>(atcf.get(), false);

  REQUIRE((*atcf)[6].getIsotachs()[1].getQuadrants()[1].getIsotachRadius() ==
          0.0);
  REQUIRE((*atcf)[6].getIsotachs()[1].getQuadrants()[0].getIsotachRadius() ==
          Catch::Approx(27779.9));

  preprocessor->prepareAtcfData();

  REQUIRE((*atcf)[6].getIsotachs()[1].getQuadrants()[1].getIsotachRadius() ==
          Catch::Approx(111119.72381124));
  REQUIRE((*atcf)[6].getIsotachs()[1].getQuadrants()[0].getIsotachRadius() ==
          Catch::Approx(111119.72381124));
}

TEST_CASE("GahmSolver", "[Preprocessor]") {
  constexpr double kt2ms = Gahm::Physical::Units::convert(
      Gahm::Physical::Units::Knot, Gahm::Physical::Units::MetersPerSecond);
  constexpr double nmi2m = Gahm::Physical::Units::convert(
      Gahm::Physical::Units::NauticalMile, Gahm::Physical::Units::Meter);

  // clang-format off
  std::array<double, 4> vmax = {100*kt2ms,100*kt2ms,100*kt2ms,100*kt2ms};
  std::array<double, 4> isorad = {65*nmi2m,65*nmi2m,65*nmi2m,65*nmi2m};
  std::array<double, 4> isospd = {64*kt2ms,64*kt2ms,64*kt2ms,64*kt2ms};
  // clang-format on

  double p0 = 97900.0;
  double pinf = 101300.0;
  double latitude = 40.0;

  auto solver = std::make_unique<Gahm::Solver::GahmSolver>(
      isorad[0], isospd[0], vmax[0], p0, pinf, latitude);
  solver->solve();
  double solution_rmax = solver->rmax();
  double solution_b = solver->gahm_b();

  REQUIRE(solution_rmax / nmi2m ==
          Catch::Approx(39.5956715406));  // Solution checked in nmi
  REQUIRE(solution_b == Catch::Approx(3.1036266587));

  double v = Gahm::Solver::GahmEquations::GahmFunction(
      solution_rmax, vmax[0], isospd[0], isorad[0],
      Gahm::Physical::Earth::coriolis(latitude), solution_b);

  //...This is the proof that the root is correctly found by the solver. Note
  // that by increasing the number of iterations in the GahmSolver class,
  // the solution is more accurate, but 1e-8 is good enough for anything we are
  // going to do.
  REQUIRE(v == Catch::Approx(0.0).margin(1e-8));
}

TEST_CASE("GahmAtcf", "[Preprocessor]") {
  auto atcf =
      std::make_unique<Gahm::Atcf::AtcfFile>("test_files/bal122005.dat");
  atcf->read();
  auto preprocessor = std::make_unique<Gahm::Preprocessor>(atcf.get());
  preprocessor->prepareAtcfData();
  preprocessor->solve();

  auto line =
      (*atcf)[6].to_string(0, Gahm::Datatypes::Date(2005, 8, 15, 0, 0, 0), 0);
  atcf->write("gahm_test.dat");
}
