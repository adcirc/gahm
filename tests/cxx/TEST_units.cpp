//
// Created by Zach Cobell on 7/30/24.
//
#include <cmath>

#include "catch2/catch_approx.hpp"
#include "catch2/catch_test_macros.hpp"
#include "physical/Units.hpp"

// NOLINTBEGIN(cppcoreguidelines-macro-usage,
// cppcoreguidelines-avoid-magic-numbers, misc-use-anonymous-namespace,
// readability-function-cognitive-complexity, cert-err58-cpp,
// readability-magic-numbers)

using namespace Catch;

TEST_CASE("Units", "[Units]") {
  SECTION("Length") {
    REQUIRE(Gahm::Physical::Units::Meter() == Approx(1.0));
    REQUIRE(Gahm::Physical::Units::Kilometer() == Approx(1.0 / 1000.0));
    REQUIRE(Gahm::Physical::Units::Foot() == Approx(3.28084));
    REQUIRE(Gahm::Physical::Units::Mile() == Approx(1.0 / 1609.34));
    REQUIRE(Gahm::Physical::Units::NauticalMile() ==
            Approx(1.0 / 1851.995396854));
  }

  SECTION("Angle") {
    REQUIRE(Gahm::Physical::Units::Radian() == Approx(1.0));
    REQUIRE(Gahm::Physical::Units::Degree() == Approx(180.0 / M_PI));
  }

  SECTION("Speed") {
    REQUIRE(Gahm::Physical::Units::MetersPerSecond() == Approx(1.0));
    REQUIRE(Gahm::Physical::Units::MilesPerHour() == Approx(2.23694));
    REQUIRE(Gahm::Physical::Units::Knot() == Approx(1.94384));
    REQUIRE(Gahm::Physical::Units::KilometersPerHour() == Approx(3.6));
  }
}

TEST_CASE("Unit Conversions", "[Unit Conversions]") {
  SECTION("Length") {
    REQUIRE(Gahm::Physical::Units::convert(Gahm::Physical::Units::Meter,
                                           Gahm::Physical::Units::Kilometer) ==
            Approx(0.001));
    REQUIRE(Gahm::Physical::Units::convert(Gahm::Physical::Units::Meter,
                                           Gahm::Physical::Units::Foot) ==
            Approx(3.28084));
    REQUIRE(Gahm::Physical::Units::convert(Gahm::Physical::Units::Meter,
                                           Gahm::Physical::Units::Mile) ==
            Approx(1.0 / 1609.34));
    REQUIRE(
        Gahm::Physical::Units::convert(Gahm::Physical::Units::Meter,
                                       Gahm::Physical::Units::NauticalMile) ==
        Approx(1.0 / 1851.995396854));
  }

  SECTION("Angle") {
    REQUIRE(Gahm::Physical::Units::convert(Gahm::Physical::Units::Radian,
                                           Gahm::Physical::Units::Degree) ==
            Approx(180.0 / M_PI));
  }

  SECTION("Speed") {
    REQUIRE(Gahm::Physical::Units::convert(
                Gahm::Physical::Units::MetersPerSecond,
                Gahm::Physical::Units::MilesPerHour) == Approx(2.23694));
    REQUIRE(Gahm::Physical::Units::convert(
                Gahm::Physical::Units::MetersPerSecond,
                Gahm::Physical::Units::Knot) == Approx(1.94384));
    REQUIRE(Gahm::Physical::Units::convert(
                Gahm::Physical::Units::MetersPerSecond,
                Gahm::Physical::Units::KilometersPerHour) == Approx(3.6));
  }

  SECTION("Pressure") {
    REQUIRE(Gahm::Physical::Units::convert(Gahm::Physical::Units::Millibar,
                                           Gahm::Physical::Units::Millibar) ==
            Approx(1.0));
  }
}

// NOLINTEND(cppcoreguidelines-macro-usage,
// cppcoreguidelines-avoid-magic-numbers, misc-use-anonymous-namespace,
// readability-function-cognitive-complexity, cert-err58-cpp,
// readability-magic-numbers)