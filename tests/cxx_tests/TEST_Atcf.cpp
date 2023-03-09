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
#include "atcf/AtcfFile.h"
#include "atcf/AtcfIsotach.h"
#include "atcf/AtcfQuadrant.h"
#include "atcf/AtcfSnap.h"
#include "catch2/catch_test_macros.hpp"
#include "physical/Units.h"

TEST_CASE("Construct AtcfIsotach", "[AtcfIsotach]") {
  Gahm::Atcf::AtcfIsotach isotach(20.0, {10, 20, 30, 40});
  REQUIRE(isotach.getWindSpeed() == 20.0);
  REQUIRE(isotach.getQuadrants().size() == 4);
  REQUIRE(isotach.getQuadrants()[0].getQuadrantIndex() == 0);
  REQUIRE(isotach.getQuadrants()[2].getIsotachRadius() == 30.0);
  REQUIRE(isotach.getQuadrants()[1].getRadiusToMaxWindSpeed() == 0.0);
}

TEST_CASE("Construct AtcfQuadrant", "[AtcfQuadrant] ") {
  Gahm::Atcf::AtcfQuadrant quadrant;
  REQUIRE(quadrant.getQuadrantIndex() == 0);
  REQUIRE(quadrant.getIsotachRadius() == 0.0);
  REQUIRE(quadrant.getRadiusToMaxWindSpeed() == 0.0);
  REQUIRE(quadrant.getGahmHollandB() == 0.0);
  REQUIRE(quadrant.getVmaxAtBoundaryLayer() == 0.0);
  REQUIRE(quadrant.getIsotachSpeedAtBoundaryLayer() == 0.0);

  quadrant.setQuadrantIndex(1);
  quadrant.setIsotachRadius(10.0);
  quadrant.setRadiusToMaxWindSpeed(20.0);
  quadrant.setGahmHollandB(0.5);
  quadrant.setVmaxAtBoundaryLayer(30.0);
  quadrant.setIsotachSpeedAtBoundaryLayer(40.0);

  REQUIRE(quadrant.getQuadrantIndex() == 1);
  REQUIRE(quadrant.getIsotachRadius() == 10.0);
  REQUIRE(quadrant.getRadiusToMaxWindSpeed() == 20.0);
  REQUIRE(quadrant.getGahmHollandB() == 0.5);
  REQUIRE(quadrant.getVmaxAtBoundaryLayer() == 30.0);
  REQUIRE(quadrant.getIsotachSpeedAtBoundaryLayer() == 40.0);
}

TEST_CASE("Construct AtcfSnap Manually", "[AtcfSnap]") {
  Gahm::Atcf::AtcfSnap snap(Gahm::Atcf::AtcfSnap::BASIN::AL, 980.0, 1013.0,
                            20.0, 40.0, 35.0, Gahm::Datatypes::Date(2018, 9, 1),
                            1, "Test");
  const auto isotach0 = Gahm::Atcf::AtcfIsotach(20.0, {10, 20, 30, 40});
  const auto isotach1 = Gahm::Atcf::AtcfIsotach(40.0, {20, 30, 40, 50});

  REQUIRE(snap.numberOfIsotachs() == 0);
  snap.addIsotach(isotach0);
  REQUIRE(snap.numberOfIsotachs() == 1);
  snap.addIsotach(isotach1);
  REQUIRE(snap.numberOfIsotachs() == 2);
  REQUIRE(snap.basin() == Gahm::Atcf::AtcfSnap::BASIN::AL);
  REQUIRE(snap.centralPressure() == 980.0);
  REQUIRE(snap.backgroundPressure() == 1013.0);
  REQUIRE(snap.date() == Gahm::Datatypes::Date(2018, 9, 1));
  REQUIRE(snap.vmax() == 40.0);
  REQUIRE(snap.radiusToMaxWinds() == 20.0);
  REQUIRE(snap.vmaxBoundaryLayer() == 35.0);
  REQUIRE(snap.stormId() == 1);
  REQUIRE(snap.stormName() == "Test");
  REQUIRE(snap.getIsotachs()[0].getWindSpeed() == 20.0);
  REQUIRE(snap.getIsotachs()[1].getWindSpeed() == 40.0);
}

TEST_CASE("Construct AtcfSnap from string", "[AtcfSnap]") {
  const std::string line =
      "AL, 12, 2005082812,   , BEST,   0, 257N,  877W, 145,  909, HU,  50, "
      "NEQ,  120,  120,   75,  100, 1008,  300,  20, 170,   0,   L,   0,    ,  "
      " 0,   0,    KATRINA, D,";
  auto snap = Gahm::Atcf::AtcfSnap::parseAtcfSnap(line).value();
  REQUIRE(snap.basin() == Gahm::Atcf::AtcfSnap::BASIN::AL);
  REQUIRE(snap.centralPressure() == 909.0 * 100.0);
  REQUIRE(snap.backgroundPressure() == 101300.0);
  REQUIRE(snap.date() == Gahm::Datatypes::Date(2005, 8, 28, 12, 0, 0));
  REQUIRE(snap.vmax() == 145.0 * Gahm::Physical::Units::convert(
                                     Gahm::Physical::Units::Knot,
                                     Gahm::Physical::Units::MetersPerSecond));
  REQUIRE(snap.getIsotachs()[0].getWindSpeed() ==
          50.0 * Gahm::Physical::Units::convert(
                     Gahm::Physical::Units::Knot,
                     Gahm::Physical::Units::MetersPerSecond));
  REQUIRE(snap.stormId() == 12);
  REQUIRE(snap.stormName() == "KATRINA");
}

TEST_CASE("Construct Atcf from file", "[AtcfFile]") {
  const std::string filename = "test_files/bal122005.dat";
  auto atcf = Gahm::Atcf::AtcfFile(filename);
  atcf.read();

  REQUIRE(atcf.size() == 28);
  REQUIRE(atcf[0].stormId() == 12);
}