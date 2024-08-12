//
// Created by Zach Cobell on 7/30/24.
//
#include "catch2/catch_approx.hpp"
#include "catch2/catch_test_macros.hpp"
#include "physical/Constants.h"

using namespace Catch;

TEST_CASE("Physical Constants", "[PhysicalConstants]") {
  SECTION("Pi") {
    REQUIRE(Gahm::Physical::Constants::pi() == Approx(3.14159265358979323846));
  }

  SECTION("Two Pi") {
    REQUIRE(Gahm::Physical::Constants::twoPi() ==
            Approx(6.28318530717958647692));
  }

  SECTION("Half Pi") {
    REQUIRE(Gahm::Physical::Constants::halfPi() ==
            Approx(1.57079632679489661923));
  }

  SECTION("Quarter Pi") {
    REQUIRE(Gahm::Physical::Constants::quarterPi() ==
            Approx(0.78539816339744830962));
  }

  SECTION("Degrees to Radians") {
    REQUIRE(Gahm::Physical::Constants::deg2rad() ==
            Approx(0.01745329251994329577));
  }

  SECTION("Radians to Degrees") {
    REQUIRE(Gahm::Physical::Constants::rad2deg() ==
            Approx(57.29577951308232087680));
  }

  SECTION("Background Pressure") {
    REQUIRE(Gahm::Physical::Constants::backgroundPressure() == Approx(1013.00));
  }

  SECTION("Top of Boundary Layer to 10m") {
    REQUIRE(Gahm::Physical::Constants::topOfBoundaryLayerToTenMeter() ==
            Approx(0.9));
  }

  SECTION("10m to Top of Boundary Layer") {
    REQUIRE(Gahm::Physical::Constants::tenMeterToTopOfBoundaryLayer() ==
            Approx(1.11111111111111111111));
  }
}