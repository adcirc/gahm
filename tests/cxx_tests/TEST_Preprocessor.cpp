// MIT License
//
// Copyright (c) 2023 ADCIRC Development Group
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//
// Author: Zach Cobell
// Contact: zcobell@thewaterinstitute.org
//

#include <array>
#include <memory>

#include "catch2/catch_approx.hpp"
#include "catch2/catch_test_macros.hpp"
#include "gahm/GahmEquations.h"
#include "gahm/GahmSolver.h"
#include "physical/Earth.h"
#include "preprocessor/Preprocessor.h"

TEST_CASE("FillMissingData", "[Preprocessor]") {
  auto atcf =
      std::make_unique<Gahm::Atcf::AtcfFile>("test_files/bal122005.dat");
  auto preprocessor = std::make_unique<Gahm::Preprocessor>(atcf.get());

  REQUIRE((*atcf)[6].getIsotachs()[1].getQuadrants()[1].getIsotachRadius() ==
          0.0);
  REQUIRE((*atcf)[6].getIsotachs()[1].getQuadrants()[0].getIsotachRadius() ==
          Catch::Approx(27779.9));

  preprocessor->prepareAtcfData();

  REQUIRE((*atcf)[6].getIsotachs()[1].getQuadrants()[1].getIsotachRadius() ==
          Catch::Approx(27779.9));
  REQUIRE((*atcf)[6].getIsotachs()[1].getQuadrants()[0].getIsotachRadius() ==
          Catch::Approx(27779.9));
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
  double solution_b = solver->bg();

  REQUIRE(solution_rmax / nmi2m ==
          Catch::Approx(39.5956715406));  // Solution checked in nmi
  REQUIRE(solution_b == Catch::Approx(3.1036266587));

  double v = Gahm::Solver::GahmEquations::GahmFunction(
      solution_rmax, vmax[0], isospd[0], isorad[0],
      Gahm::Physical::Earth::coriolis(latitude), solution_b);
  REQUIRE(v == Catch::Approx(0.0).margin(1e-8));
}

TEST_CASE("GahmAtcf", "[Preprocessor]") {
  auto atcf =
      std::make_unique<Gahm::Atcf::AtcfFile>("test_files/bal122005.dat");
  auto preprocessor = std::make_unique<Gahm::Preprocessor>(atcf.get());
  preprocessor->prepareAtcfData();
  preprocessor->solve();

  auto line =
      (*atcf)[6].to_string(0, Gahm::Datatypes::Date(2005, 8, 15, 0, 0, 0), 0);
  atcf->write("gahm_test.dat");
}
