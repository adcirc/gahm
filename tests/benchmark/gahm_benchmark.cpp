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

#include <random>

#include "atcf/AtcfFile.h"
#include "benchmark/benchmark.h"
#include "datatypes/Date.h"
#include "datatypes/WindGrid.h"
#include "preprocessor/Preprocessor.h"
#include "vortex/Vortex.h"

/*
 * Get a random time between two dates using a random number generator
 * @param mt Random number generator
 * @param time_start Start time
 * @param time_end End time
 * @return Random time between start and end
 */
Gahm::Datatypes::Date get_random_time(std::mt19937 &mt,
                                      const Gahm::Datatypes::Date &time_start,
                                      const Gahm::Datatypes::Date &time_end) {
  auto time_start_sec = time_start.toSeconds();
  auto time_end_sec = time_end.toSeconds();
  auto time_diff = time_end_sec - time_start_sec;
  auto time_rand = time_start_sec + (mt() % time_diff);
  auto time_out = time_start;
  time_out.addSeconds(time_rand);
  return time_out;
}

/**
 * Benchmark the vortex solver for a random time
 * @param state Benchmark state
 */
static void BM_Vortex(benchmark::State &state) {
  // Random number generator
  std::mt19937 mt(
      std::chrono::high_resolution_clock::now().time_since_epoch().count());

  // Read ATCF file
  std::string atcf_file = "../tests/test_files/bal122005.dat";
  auto atcf = Gahm::Atcf::AtcfFile(atcf_file, true);
  atcf.read();

  // Prepare ATCF data
  auto preprocessor = Gahm::Preprocessor(&atcf);
  preprocessor.prepareAtcfData();
  preprocessor.solve();

  // Create vortex solver and wind grid
  auto wind_grid =
      Gahm::Datatypes::WindGrid::fromCorners(-100, 5, -70, 35, 0.1, 0.1);
  auto vortex = Gahm::Vortex(&atcf, wind_grid.points());

  // Prep the counts of nodes processed and the time range
  size_t nodes_processed = 0;
  auto nodes_per_it = wind_grid.points().size();
  auto time_start = atcf[0].date();
  auto time_end = atcf[atcf.size() - 1].date();

  // Benchmark the vortex solver
  for (auto _ : state) {
    auto time = get_random_time(mt, time_start, time_end);
    auto v = vortex.solve(time);
    benchmark::DoNotOptimize(v);
    nodes_processed += nodes_per_it;
  }

  // Set the benchmark counters for nodes processed and rate
  state.counters["NodeTime"] = benchmark::Counter(
      static_cast<double>(nodes_processed), benchmark::Counter::kIsRate);
  state.counters["NodeRate"] = benchmark::Counter(
      static_cast<double>(nodes_processed),
      benchmark::Counter::kIsRate | benchmark::Counter::kInvert);
}

/**
 * Benchmark the random time generator
 * @param state Benchmark state
 */
static void BM_getRandomTime(benchmark::State &state) {
  std::mt19937 mt(
      std::chrono::high_resolution_clock::now().time_since_epoch().count());

  auto time_start = Gahm::Datatypes::Date(2018, 1, 1, 0, 0, 0);
  auto time_end = Gahm::Datatypes::Date(2018, 1, 1, 12, 0, 0);

  for (auto _ : state) {
    auto time = get_random_time(mt, time_start, time_end);
    benchmark::DoNotOptimize(time);
  }
}

BENCHMARK(BM_Vortex);
// BENCHMARK(BM_getRandomTime);
BENCHMARK_MAIN();