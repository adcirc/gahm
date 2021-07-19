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
//#define CATCH_CONFIG_MAIN
#include "Gahm.h"
#include "OwiAscii.h"
#include "catch.hpp"

//TEST_CASE("GAHM-Oceanweather", "[gahm-owi]") {
int main() {
  Date start_date(2005, 8, 27, 0, 0, 0);
  Date end_date(2005, 8, 28, 0, 0, 0);
  const unsigned dt = 1800;

  const double llx = -99.0;
  const double lly = 15.0;
  const double urx = -75.0;
  const double ury = 35.0;
  const double dx = 0.1;
  const double dy = 0.1;

  const std::string pressure_file = "katrina_gahm.221";
  const std::string wind_file = "katrina_gahm.222";

  OwiAscii ogrid(start_date, end_date, dt);

  WindGrid domain1(llx, lly, urx, ury, dx, dy);
  ogrid.addDomain(domain1, pressure_file, wind_file);

  domain1.write("grid.txt");

  auto position = domain1.griddata();
  std::cout << std::get<0>(position).size() << std::endl;

  Gahm g("../testing/test_files/bal122005.dat", std::get<0>(position),
         std::get<1>(position));

  size_t ii = 0;
  for (auto d = start_date; d <= end_date; d += dt) {
    std::cout << "Time = " << d << std::endl;
    std::cout << "    Solving GAHM...";
    auto s = g.get(d);
    std::cout << "done!" << std::endl;
    std::cout << "    Writing solution to Oceanweather format...";
    ogrid.write(d, 0, s.p(), s.u(), s.v());
    std::cout << "done!" << std::endl;
    ii++;
    //std::cout << ii << ", " << s.stormParameters()->cycle() << "," << s.stormParameters()->longitude() << ", " << s.stormParameters()->latitude() << std::endl;
  }
}
