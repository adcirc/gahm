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

#include <fcntl.h>
#include <openssl/evp.h>
#include <unistd.h>

#include <cstdio>

#include "atcf/AtcfFile.h"
#include "catch2/catch_approx.hpp"
#include "catch2/catch_test_macros.hpp"
#include "datatypes/WindGrid.h"
#include "output/OwiOutput.h"
#include "preprocessor/Preprocessor.h"
#include "vortex/Vortex.h"

std::string compute_file_md5(const std::string& filename) {
  int fd = open(filename.c_str(), O_RDONLY);
  if (fd == -1) {
    return "";
  }

  EVP_MD_CTX* mdctx;
  const EVP_MD* md;
  unsigned char md_value[EVP_MAX_MD_SIZE];
  unsigned int md_len, i;

  OpenSSL_add_all_digests();

  md = EVP_get_digestbyname("MD5");
  if (!md) {
    return "";
  }

  mdctx = EVP_MD_CTX_new();
  EVP_DigestInit_ex(mdctx, md, nullptr);
  char buf[1024];
  ssize_t bytes;
  while ((bytes = read(fd, buf, 1024)) != 0) {
    EVP_DigestUpdate(mdctx, buf, bytes);
  }
  EVP_DigestFinal_ex(mdctx, md_value, &md_len);
  EVP_MD_CTX_free(mdctx);

  char mdString[33];
  for (i = 0; i < md_len; i++) {
    snprintf(&mdString[i * 2], 3, "%02x", (unsigned int)md_value[i]);
  }

  return mdString;
}

TEST_CASE("Oceanweather Output", "[Output]") {
  Gahm::Datatypes::WindGrid wg = Gahm::Datatypes::WindGrid::fromCorners(
      -100.0, 22.0, -78.0, 32.0, 0.1, 0.1);

  const std::string filename = "test_files/bal122005.dat";
  auto atcf = Gahm::Atcf::AtcfFile(filename);
  atcf.read();

  Gahm::Preprocessor prep(&atcf);
  prep.prepareAtcfData();
  prep.solve();

  auto start_time = Gahm::Datatypes::Date(2005, 8, 25, 0, 0, 0);
  auto end_time = Gahm::Datatypes::Date(2005, 8, 26, 0, 0, 0);
  auto dt = 3600;

  auto v = Gahm::Vortex(&atcf, wg.points());
  auto output =
      Gahm::Output::OwiOutput(start_time, end_time, "test_owi_katrina", wg);
  output.open();

  for (auto time = start_time.toSeconds(); time < end_time.toSeconds();
       time += dt) {
    auto date = Gahm::Datatypes::Date(time);
    std::cout << "Solving vortex for time = " << date << std::endl;
    auto solution = v.solve(date);
    output.write(date, solution);
  }
  output.close();

  //...Compute md5 hashes of output files as verification that both the solution
  // and the output formats are correct
  //  auto md5_pressure = compute_file_md5("test_owi_katrina.pre");
  //  auto md5_wind = compute_file_md5("test_owi_katrina.wnd");
  //  REQUIRE(md5_pressure == "89368aa40cbc88c3e6537d0bd90d4152");
  //  REQUIRE(md5_wind == "7a349ca2e7d4c4221351ecbdead3d858");
}
