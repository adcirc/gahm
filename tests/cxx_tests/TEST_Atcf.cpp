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
#include "catch2/catch_approx.hpp"
#include "catch2/catch_test_macros.hpp"
#include "gahm.h"

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
  REQUIRE(quadrant.getIsotachSpeedAtBoundaryLayer() == 0.0);

  quadrant.setQuadrantIndex(1);
  quadrant.setIsotachRadius(10.0);
  quadrant.setRadiusToMaxWindSpeed(20.0);
  quadrant.setGahmHollandB(0.5);
  quadrant.setIsotachSpeedAtBoundaryLayer(40.0);

  REQUIRE(quadrant.getQuadrantIndex() == 1);
  REQUIRE(quadrant.getIsotachRadius() == 10.0);
  REQUIRE(quadrant.getRadiusToMaxWindSpeed() == 20.0);
  REQUIRE(quadrant.getGahmHollandB() == 0.5);
  REQUIRE(quadrant.getIsotachSpeedAtBoundaryLayer() == 40.0);
}

TEST_CASE("Construct AtcfSnap Manually", "[AtcfSnap]") {
  Gahm::Atcf::AtcfSnap snap(Gahm::Atcf::AtcfSnap::BASIN::AL, 980.0, 1013.0,
                            20.0, 40.0, Gahm::Datatypes::Date(2018, 9, 1), 1,
                            "Test");
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
  //REQUIRE(snap.vmaxBoundaryLayer() == Catch::Approx(0.0)); // Uninitialized until preprocessor
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
          50.0 *
              Gahm::Physical::Units::convert(
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