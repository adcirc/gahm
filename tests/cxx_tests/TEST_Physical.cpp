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
#include "physical/Constants.h"
#include "physical/Earth.h"
#include "physical/Units.h"

TEST_CASE("Check Physical Constants", "[Constants]") {
  REQUIRE(Gahm::Physical::Constants::g() == 9.80665);
  REQUIRE(Gahm::Physical::Constants::rhoWater() == 1000.0);
  REQUIRE(Gahm::Physical::Constants::oneToten() == 0.8928);
  REQUIRE(Gahm::Physical::Constants::backgroundPressure() == 1013.00);
  REQUIRE(Gahm::Physical::Constants::windReduction() == 0.9);
  REQUIRE(Gahm::Physical::Constants::rhoAir() == 1.293);
}

TEST_CASE("Check Earth Calculations", "[Earth]") {
  REQUIRE(Gahm::Physical::Earth::omega() == 7.292115e-5);
  REQUIRE(Gahm::Physical::Earth::radius(0.0) ==
          Catch::Approx(Gahm::Physical::Earth::equatorialRadius()));
  REQUIRE(Gahm::Physical::Earth::radius(45.0) ==
          Catch::Approx(6367489.5368007272));
  REQUIRE(Gahm::Physical::Earth::radius(90.0) ==
          Catch::Approx(Gahm::Physical::Earth::polarRadius()));
  REQUIRE(Gahm::Physical::Earth::radius(23.5, 35.0) ==
          Catch::Approx(6373063.9457547655));
  REQUIRE(Gahm::Physical::Earth::coriolis(25.0) == Catch::Approx(0.0000616356));
}

TEST_CASE("Unit Conversions", "[UnitConversion]") {
  REQUIRE(
      Gahm::Physical::Units::convert(Gahm::Physical::Units::Knot,
                                     Gahm::Physical::Units::MetersPerSecond) ==
      Catch::Approx(0.51444444444444444));
  REQUIRE(Gahm::Physical::Units::convert(Gahm::Physical::Units::Knot,
                                         Gahm::Physical::Units::Knot) ==
          Catch::Approx(1.0));
  REQUIRE(Gahm::Physical::Units::convert(Gahm::Physical::Units::Knot,
                                         Gahm::Physical::Units::MilesPerHour) ==
          Catch::Approx(1.1507794480235434));
  REQUIRE(Gahm::Physical::Units::convert(
              Gahm::Physical::Units::Knot,
              Gahm::Physical::Units::KilometersPerHour) ==
          Catch::Approx(1.852));
  REQUIRE(Gahm::Physical::Units::convert(Gahm::Physical::Units::Meter,
                                         Gahm::Physical::Units::Meter) ==
          Catch::Approx(1.0));
  REQUIRE(Gahm::Physical::Units::convert(Gahm::Physical::Units::Meter,
                                         Gahm::Physical::Units::Kilometer) ==
          Catch::Approx(0.001));
  REQUIRE(Gahm::Physical::Units::convert(Gahm::Physical::Units::Meter,
                                         Gahm::Physical::Units::Mile) ==
          Catch::Approx(0.000621371192237334));
  REQUIRE(Gahm::Physical::Units::convert(Gahm::Physical::Units::NauticalMile,
                                         Gahm::Physical::Units::Meter) ==
          Catch::Approx(1852.0));
  REQUIRE(Gahm::Physical::Units::convert(Gahm::Physical::Units::Meter,
                                         Gahm::Physical::Units::NauticalMile) ==
          Catch::Approx(1.0 / 1852.0));
}