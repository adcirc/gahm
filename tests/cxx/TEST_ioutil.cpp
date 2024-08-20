//
// Created by Zach Cobell on 7/30/24.
//

#include <string>

#include "catch2/catch_approx.hpp"
#include "catch2/catch_test_macros.hpp"
#include "datatypes/Datetime.h"
#include "util/ioutils.h"

// NOLINTBEGIN(cppcoreguidelines-macro-usage,
// cppcoreguidelines-avoid-magic-numbers, misc-use-anonymous-namespace,
// readability-function-cognitive-complexity, cert-err58-cpp,
// readability-magic-numbers)

TEST_CASE("Date", "[date]") {
  const std::string date1 = "2024010406";
  int hours = 0;

  const auto parsed_date1 = Gahm::Types::Datetime(date1).to_struct();
  REQUIRE(parsed_date1.year == 2024);
  REQUIRE(parsed_date1.month == 1);
  REQUIRE(parsed_date1.day == 4);
  REQUIRE(parsed_date1.hour == 6);
  REQUIRE(parsed_date1.minute == 0);
  REQUIRE(parsed_date1.second == 0);

  hours = 6;
  const auto parsed_date2 = Gahm::Types::Datetime(date1, hours).to_struct();
  REQUIRE(parsed_date2.year == 2024);
  REQUIRE(parsed_date2.month == 1);
  REQUIRE(parsed_date2.day == 4);
  REQUIRE(parsed_date2.hour == 12);
  REQUIRE(parsed_date2.minute == 0);
  REQUIRE(parsed_date2.second == 0);

  hours = -6;
  const auto parsed_date3 = Gahm::Types::Datetime(date1, hours).to_struct();
  REQUIRE(parsed_date3.year == 2024);
  REQUIRE(parsed_date3.month == 1);
  REQUIRE(parsed_date3.day == 4);
  REQUIRE(parsed_date3.hour == 0);
  REQUIRE(parsed_date3.minute == 0);
  REQUIRE(parsed_date3.second == 0);

  hours = 36;
  const auto parsed_date4 = Gahm::Types::Datetime(date1, hours).to_struct();
  REQUIRE(parsed_date4.year == 2024);
  REQUIRE(parsed_date4.month == 1);
  REQUIRE(parsed_date4.day == 5);
  REQUIRE(parsed_date4.hour == 18);
  REQUIRE(parsed_date4.minute == 0);
  REQUIRE(parsed_date4.second == 0);

  hours = 7624;
  const auto parsed_date5 = Gahm::Types::Datetime(date1, hours).to_struct();
  REQUIRE(parsed_date5.year == 2024);
  REQUIRE(parsed_date5.month == 11);
  REQUIRE(parsed_date5.day == 16);
  REQUIRE(parsed_date5.hour == 22);
  REQUIRE(parsed_date5.minute == 0);
  REQUIRE(parsed_date5.second == 0);
}

TEST_CASE("Split String", "[split_string]") {
  const std::string str = "1,2,3,4,5,6,7,8,9,10";
  auto split = Gahm::Util::IO::split_string(str);
  REQUIRE(split.size() == 10);
  REQUIRE(split[0] == "1");
  REQUIRE(split[1] == "2");
  REQUIRE(split[2] == "3");
  REQUIRE(split[3] == "4");
  REQUIRE(split[4] == "5");
  REQUIRE(split[5] == "6");
  REQUIRE(split[6] == "7");
  REQUIRE(split[7] == "8");
  REQUIRE(split[8] == "9");
  REQUIRE(split[9] == "10");

  const std::string str3 = "1,2,3,4,5,6,7,8,9,10,";
  auto split3 = Gahm::Util::IO::split_string(str3);
  REQUIRE(split3.size() == 11);
  REQUIRE(split3[0] == "1");
  REQUIRE(split3[1] == "2");
  REQUIRE(split3[2] == "3");
  REQUIRE(split3[3] == "4");
  REQUIRE(split3[4] == "5");
  REQUIRE(split3[5] == "6");
  REQUIRE(split3[6] == "7");
  REQUIRE(split3[7] == "8");
  REQUIRE(split3[8] == "9");
  REQUIRE(split3[9] == "10");
  REQUIRE(split3[10] == "");

  const std::string str4 = "1,2,3,,4,5  ,6,7,8,9,10";
  auto split4 = Gahm::Util::IO::split_string(str4);
  REQUIRE(split4.size() == 11);
  REQUIRE(split4[0] == "1");
  REQUIRE(split4[1] == "2");
  REQUIRE(split4[2] == "3");
  REQUIRE(split4[3] == "");
  REQUIRE(split4[4] == "4");
  REQUIRE(split4[5] == "5");
  REQUIRE(split4[6] == "6");
  REQUIRE(split4[7] == "7");
  REQUIRE(split4[8] == "8");
  REQUIRE(split4[9] == "9");
  REQUIRE(split4[10] == "10");
}

// NOLINTEND(cppcoreguidelines-macro-usage,
// cppcoreguidelines-avoid-magic-numbers, misc-use-anonymous-namespace,
// readability-function-cognitive-complexity, cert-err58-cpp,
// readability-magic-numbers)