#include <fuzzer/FuzzedDataProvider.h>

#include <cstdlib>
#include <string>

#include "atcf/AtcfFile.h"
#include "atcf/AtcfSnap.h"
#include "preprocessor/Preprocessor.h"

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size) {
  FuzzedDataProvider fuzz_data(data, size);
  auto basin = fuzz_data.ConsumeEnum<Gahm::Atcf::AtcfSnap::BASIN>();
  auto rmax = fuzz_data.ConsumeFloatingPoint<double>();
  double cp = fuzz_data.ConsumeFloatingPoint<double>();
  double bp = fuzz_data.ConsumeFloatingPoint<double>();
  double vmax = fuzz_data.ConsumeFloatingPoint<double>();
  double vmax_bl = fuzz_data.ConsumeFloatingPoint<double>();
  Gahm::Datatypes::Date date =
      Gahm::Datatypes::Date::fromSeconds(fuzz_data.ConsumeIntegral<long>());
  int storm_id = fuzz_data.ConsumeIntegral<int>();
  std::string storm_name = fuzz_data.ConsumeRandomLengthString();

  constexpr size_t n_snaps = 8;
  constexpr size_t n_isotachs = 4;

  Gahm::Atcf::AtcfFile atcf;
  for (size_t i = 0; i < n_snaps; ++i) {
    auto snap = Gahm::Atcf::AtcfSnap(basin, cp, bp, rmax, vmax, vmax_bl, date,
                                     storm_id, storm_name);
    for (size_t j = 0; j < n_isotachs; ++j) {
      double isotach_wind_speed = fuzz_data.ConsumeFloatingPoint<double>();
      std::array<double, 4> radii = {fuzz_data.ConsumeFloatingPoint<double>(),
                                     fuzz_data.ConsumeFloatingPoint<double>(),
                                     fuzz_data.ConsumeFloatingPoint<double>(),
                                     fuzz_data.ConsumeFloatingPoint<double>()};
      snap.addIsotach(Gahm::Atcf::AtcfIsotach(isotach_wind_speed, radii));
    }
    atcf.addAtcfSnap(snap);
  }

  auto prep = Gahm::Preprocessor(&atcf);
  prep.prepareAtcfData();

  return 0;
}