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
#include <memory>

#include "Atcf.h"
#include "Preprocessor.h"
#include "catch.hpp"

TEST_CASE("Atcf - I/O GAHM", "[atcf-io]") {
  auto assume = std::make_shared<Gahm::Assumptions>();
  Gahm::Atcf a("test_files/bal082018.dat", assume);
  a.read();

  Gahm::Preprocessor p(&a);
  int ierr = p.run();

  a.write("test_output.22");

  REQUIRE(a.nRecords() == 42);

  REQUIRE(a.record(8)->centralPressure() == Approx(979.530231));

  assume->log(Gahm::Assumption::MINOR);
}
