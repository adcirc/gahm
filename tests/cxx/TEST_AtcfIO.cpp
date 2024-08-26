//
// Created by Zach Cobell on 7/30/24.
//
#include <string>

#include "atcf/AtcfIO.hpp"
#include "catch2/catch_approx.hpp"
#include "catch2/catch_test_macros.hpp"

using namespace Catch;

// NOLINTBEGIN(cppcoreguidelines-macro-usage,
// cppcoreguidelines-avoid-magic-numbers, misc-use-anonymous-namespace,
// readability-function-cognitive-complexity, cert-err58-cpp,
// readability-magic-numbers)

TEST_CASE("AtcfIO", "[AtcfIO]") {
  SECTION("Read ATCF") {
    const std::string filename = "../test_files/bal122005.dat";

    const Gahm::Atcf::AtcfIO atcfIO(filename);
    auto atcf = atcfIO.read();
  }
}

// NOLINTEND(cppcoreguidelines-macro-usage,
// cppcoreguidelines-avoid-magic-numbers, misc-use-anonymous-namespace,
// readability-function-cognitive-complexity, cert-err58-cpp,
// readability-magic-numbers)