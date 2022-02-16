// MIT License
//
// Copyright (c) 2020 ADCIRC Development Group
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
#define CATCH_CONFIG_MAIN
#include "Physical.h"
#include "catch.hpp"

TEST_CASE("Physical - Radius Earth", "[RadiusEarth]") {
  auto radiusEarthAt35 = Gahm::Physical::radiusEarth(35.0);
  REQUIRE(radiusEarthAt35 == Approx(6371141.2260278296));

  auto radiusEarthAt352 = Gahm::Physical::radiusEarth(30.0, 40.0);
  REQUIRE(radiusEarthAt35 == Approx(radiusEarthAt352));
}

TEST_CASE("Physical - Distance", "[distance]") {
  auto cdis = Gahm::Physical::cartesian_distance(1.0, 2.0, 3.0, 4.0);
  REQUIRE(cdis == Approx(2.828427));

  auto gdis = Gahm::Physical::geodesic_distance(-90.0, 20.0, -74.0, 40.0);
  REQUIRE(gdis == Approx(2696551.5700955666));

  auto sdis = Gahm::Physical::sphericalDx(-90.0, 20.0, -74.0, 40.0);
  REQUIRE(std::get<0>(sdis) == Approx(1539944.325839015));
  REQUIRE(std::get<1>(sdis) == Approx(2224535.3744850801));
  REQUIRE(std::get<2>(sdis) == gdis);
}

TEST_CASE("Physical - Azimuth", "[azimuth]") {
  auto azi = Gahm::Physical::azimuthEarth(-90.0, 20.0, -74.0, 40.0);
  REQUIRE(azi == Approx(-0.5401034501));
}

TEST_CASE("Physical - Coriolis", "[coriolis]") {
  auto c = Gahm::Physical::coriolis(30.0);
  REQUIRE(c == Approx(0.0000763628));
}