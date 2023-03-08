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
#include "catch2/catch_test_macros.hpp"
#include "datatypes/CircularArray.h"

TEST_CASE("Create Array", "[CircularArray] ") {
  Gahm::Datatypes::CircularArray<int, 10> array(
      std::array{0, 1, 2, 3, 4, 5, 6, 7, 8, 9});
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
  Gahm::Datatypes::CircularArray<int, 10> array({0, 1, 2, 3, 4, 5, 6, 7, 8, 9});
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