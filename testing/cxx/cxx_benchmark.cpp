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
#include "Gahm.h"
#include "WindGrid.h"
#include "Logging.h"
#include "benchmark/benchmark.h"

static bool printed_points_to_screen = false;

static void benchmark_gahm_katrina(benchmark::State &state) {
  
  Date start_date(2005, 8, 27, 0, 0, 0);
  Date end_date(2005, 8, 30, 0, 0, 0);
  const unsigned dt = 900;

  const double llx = -99.0;
  const double lly = 15.0;
  const double urx = -75.0;
  const double ury = 35.0;
  const double dx = 0.1;
  const double dy = 0.1;

  WindGrid domain1(llx, lly, urx, ury, dx, dy);
  auto position = domain1.griddata();

  auto xpoints = std::get<0>(position);
  auto ypoints = std::get<1>(position);

  Gahm g("../testing/test_files/bal122005.dat", xpoints,ypoints);

  if( !printed_points_to_screen ){
      Logging::log("Running GAHM using " + std::to_string(xpoints.size()) + " points."); 
      printed_points_to_screen = true;
  }

  auto d = start_date;

  for (auto _ : state) {
    auto solution = g.get(d);
    benchmark::DoNotOptimize(solution);
    d+=dt;
  }
}

BENCHMARK(benchmark_gahm_katrina);

BENCHMARK_MAIN();
