//
// Created by Zach Cobell on 7/30/24.
//
#include "catch2/catch_test_macros.hpp"
#include "catch2/matchers/catch_matchers_floating_point.hpp"
#include "physical/Constants.hpp"

// NOLINTBEGIN(cppcoreguidelines-macro-usage,
// cppcoreguidelines-avoid-magic-numbers, misc-use-anonymous-namespace,
// readability-function-cognitive-complexity, cert-err58-cpp,
// readability-magic-numbers)

using namespace Catch;
using namespace Catch::Matchers;

TEST_CASE("Physical Constants", "[PhysicalConstants]") {
  SECTION("Pi") {
    REQUIRE_THAT(Gahm::Physical::Constants::pi(),
                 WithinAbs(3.14159265358979323846, 1e-10));
    REQUIRE_THAT(Gahm::Physical::Constants::twoPi(),
                 WithinAbs(6.28318530717958647692, 1e-10));
    REQUIRE_THAT(Gahm::Physical::Constants::halfPi(),
                 WithinAbs(1.57079632679489661923, 1e-10));
    REQUIRE_THAT(Gahm::Physical::Constants::quarterPi(),
                 WithinAbs(0.78539816339744830962, 1e-10));
  }

  SECTION("Degrees to Radians") {
    REQUIRE_THAT(Gahm::Physical::Constants::deg2rad(),
                 WithinAbs(0.01745329251994329577, 1e-10));
    REQUIRE_THAT(Gahm::Physical::Constants::rad2deg(),
                 WithinAbs(57.29577951308232087680, 1e-10));
  }

  SECTION("Atmospheric Constants") {
    REQUIRE_THAT(Gahm::Physical::Constants::backgroundPressure(),
                 WithinAbs(1013.00, 1e-10));
    REQUIRE_THAT(Gahm::Physical::Constants::topOfBoundaryLayerToTenMeter(),
                 WithinAbs(0.9, 1e-10));
    REQUIRE_THAT(Gahm::Physical::Constants::tenMeterToTopOfBoundaryLayer(),
                 WithinAbs(1.11111111111111111111, 1e-10));
  }
}

// NOLINTEND(cppcoreguidelines-macro-usage,
// cppcoreguidelines-avoid-magic-numbers, misc-use-anonymous-namespace,
// readability-function-cognitive-complexity, cert-err58-cpp,
// readability-magic-numbers)