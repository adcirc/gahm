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
#include "catch2/catch_test_macros.hpp"
#include "gahm.h"

TEST_CASE("Create Array", "[CircularArray] ") {
  std::array<int, 10> a{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  Gahm::Datatypes::CircularArray<int, 10> array(a);
  REQUIRE(array.size() == 10);
  REQUIRE(array[0] == 0);
  REQUIRE(array[1] == 1);
  REQUIRE(array[2] == 2);
  REQUIRE(array[9] == 9);
  REQUIRE(array[10] == 0);
  REQUIRE(array[11] == 1);
  REQUIRE(array[-1] == 9);
  REQUIRE(array[-2] == 8);
  REQUIRE(array[-10] == 0);
}

TEST_CASE("Create Array with initializer list", "[CircularArray] ") {
  std::array<int, 10> a{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  Gahm::Datatypes::CircularArray<int, 10> array(a);
  REQUIRE(array.size() == 10);
  REQUIRE(array[0] == 0);
  REQUIRE(array[1] == 1);
  REQUIRE(array[2] == 2);
  REQUIRE(array[9] == 9);
  REQUIRE(array[10] == 0);
  REQUIRE(array[11] == 1);
  REQUIRE(array[-1] == 9);
  REQUIRE(array[-2] == 8);
  REQUIRE(array[-10] == 0);
}