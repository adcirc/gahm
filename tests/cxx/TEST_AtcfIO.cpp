//
// Created by Zach Cobell on 7/30/24.
//
#include "catch2/catch_approx.hpp"
#include "catch2/catch_test_macros.hpp"
#include <iostream>

using namespace Catch;

#include <string>

#include "storm/AtcfIO.h"

TEST_CASE("AtcfIO", "[AtcfIO]") {
  SECTION("Read ATCF") {
    const std::string filename =
        "/Users/zcobell/Documents/Code/gahm/tests/test_files/bal122005.dat";

    Gahm::Atcf::AtcfIO atcfIO(filename);
    auto atcf = atcfIO.read();
  }
}
