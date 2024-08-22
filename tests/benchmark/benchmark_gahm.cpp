//
// Created by Zach Cobell on 8/22/24.
//

#include <array>
#include <cstddef>

#include "atcf/AtcfIO.h"
#include "atcf/AtcfPeriod.h"
#include "benchmark/benchmark.h"
#include "datatypes/Datetime.h"
#include "datatypes/Grid.h"
#include "datatypes/Point.h"
#include "datatypes/PointCloud.h"
#include "output/PointOutput.h"
#include "physical/Units.h"
#include "storm/StormTranslation.h"

namespace {

auto generate_test_snap() -> Gahm::Atcf::AtcfPeriod {
  constexpr auto mb2pa = Gahm::Physical::Units::convert(
      Gahm::Physical::Units::Millibar, Gahm::Physical::Units::Pascal);
  constexpr auto kt2ms = Gahm::Physical::Units::convert(
      Gahm::Physical::Units::Knot, Gahm::Physical::Units::MetersPerSecond);
  constexpr auto nmi2m = Gahm::Physical::Units::convert(
      Gahm::Physical::Units::NauticalMile, Gahm::Physical::Units::Meter);

  const Gahm::Types::Datetime previous_date("2018091318");
  const Gahm::Types::Datetime snap_date("2018091400");
  const auto dt =
      snap_date.seconds_since_epoch() - previous_date.seconds_since_epoch();
  const double p_c = 952.0 * mb2pa;
  const double p_bk = 1013.0 * mb2pa;
  const double v_max = 90.0 * kt2ms;
  const double r_max = 20.0 * nmi2m;
  const Gahm::Types::Point eye_location(-76.5, 34.0);
  const Gahm::Types::Point previous_eye_location(-76.0, 33.6);
  const Gahm::Storm::StormTranslation translation(eye_location,
                                                  previous_eye_location, dt);
  const Gahm::Atcf::AtcfIO::TempIsotach i34(
      34.0 * kt2ms, std::array<double, 4>{170 * nmi2m, 150 * nmi2m, 130 * nmi2m,
                                          100 * nmi2m});
  const Gahm::Atcf::AtcfIO::TempIsotach i50(
      50.0 * kt2ms,
      std::array<double, 4>{100 * nmi2m, 80 * nmi2m, 80 * nmi2m, 70 * nmi2m});
  const Gahm::Atcf::AtcfIO::TempIsotach i64(
      64.0 * kt2ms,
      std::array<double, 4>{70 * nmi2m, 60 * nmi2m, 50 * nmi2m, 50 * nmi2m});

  const auto quadrants = Gahm::Atcf::AtcfIO::transpose_to_quadrants(
      eye_location.y(), {i34, i50, i64});

  Gahm::Atcf::AtcfPeriod snap(snap_date, p_c, p_bk, v_max, r_max, eye_location,
                              quadrants);
  snap.set_translation(translation);
  snap.compute_gahm_parameters();

  return snap;
}
}  // namespace

static void BM_BenchmarkGahmSolution(benchmark::State& state) {
  const auto snap = generate_test_snap();
  const auto x_init = snap.eye_location().x() - 2.0;
  const auto y_init = snap.eye_location().y() - 2.0;
  const auto x_end = snap.eye_location().x() + 2.0;
  const auto y_end = snap.eye_location().y() + 2.0;
  const auto grid = Gahm::Types::Grid::fromCorners(
      Gahm::Types::Point(x_init, y_init), Gahm::Types::Point(x_end, y_end),
      0.01, 0.01);
  const auto pts = grid.points();

  size_t nodes_processed = 0;
  for (auto _ : state) {
    auto solution = Gahm::Output::PointOutput::get_points(snap, pts);
    benchmark::DoNotOptimize(solution);
    nodes_processed += pts.size();
  }

  // Set the benchmark counters for nodes processed and rate
  state.counters["NodeTime"] = benchmark::Counter(
      static_cast<double>(nodes_processed), benchmark::Counter::kIsRate);
  state.counters["NodeRate"] = benchmark::Counter(
      static_cast<double>(nodes_processed),
      benchmark::Counter::kIsRate | benchmark::Counter::kInvert);
}

BENCHMARK_MAIN();
BENCHMARK(BM_BenchmarkGahmSolution);