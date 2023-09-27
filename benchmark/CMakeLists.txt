# Externally provided libraries
include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG v1.10.x)

FetchContent_Declare(
  googlebenchmark
  GIT_REPOSITORY https://github.com/google/benchmark.git
  GIT_TAG main) # need main for benchmark::benchmark

FetchContent_MakeAvailable(googletest googlebenchmark)

# ...Disable benchmark installation
set(BENCHMARK_ENABLE_INSTALL
    OFF
    CACHE
      BOOL
      "Enable installation of benchmark. (Projects embedding benchmark may want to turn this OFF.)"
      FORCE)
set(BENCHMARK_ENABLE_TESTING
    OFF
    CACHE BOOL "Enable testing of the benchmark library." FORCE)
set(BENCHMARK_ENABLE_GTEST_TESTS
    OFF
    CACHE BOOL "Enable testing of the benchmark library." FORCE)
set(BUILD_GMOCK
    OFF
    CACHE BOOL "Builds the googlemock subproject" FORCE)
set(INSTALL_GTEST
    OFF
    CACHE
      BOOL
      "Enable installation of googletest. (Projects embedding googletest may want to turn this OFF.)"
      FORCE)
set(BENCHMARK_USE_BUNDLED_GTEST
    OFF
    CACHE BOOL "Use gtest installation in the googlebenchmark submodule." FORCE)
set(BENCHMARK_INSTALL_DOCS
    OFF
    CACHE
      BOOL
      "Enable installation of benchmark documentation. (Projects embedding benchmark may want to turn this OFF.)"
      FORCE)

# ...Hide the benchmark options in advanced
mark_as_advanced(
  BENCHMARK_FORCE_WERROR
  BENCHMARK_BUILD_32_BITS
  BENCHMARK_DOWNLOAD_DEPENDENCIES
  BENCHMARK_ENABLE_ASSEMBLY_TESTS
  BENCHMARK_ENABLE_DOXYGEN
  BENCHMARK_ENABLE_EXCEPTIONS
  BENCHMARK_ENABLE_GTEST_TESTS
  BENCHMARK_ENABLE_INSTALL
  BENCHMARK_ENABLE_LIBPFM
  BENCHMARK_ENABLE_LTO
  BENCHMARK_ENABLE_TESTING
  BENCHMARK_ENABLE_WERROR
  BENCHMARK_INSTALL_DOCS
  BENCHMARK_USE_BUNDLED_GTEST
  BENCHMARK_USE_LIBCXX
  BUILD_GMOCK
  FETCHCONTENT_BASE_DIR
  FETCHCONTENT_FULLY_DISCONNECTED
  FETCHCONTENT_QUIET
  FETCHCONTENT_SOURCE_DIR_GOOGLEBENCHMARK
  FETCHCONTENT_SOURCE_DIR_GOOGLETEST
  FETCHCONTENT_UPDATES_DISCONNECTED
  FETCHCONTENT_UPDATES_DISCONNECTED_GOOGLEBENCHMARK
  FETCHCONTENT_UPDATES_DISCONNECTED_GOOGLETEST
  INSTALL_GTEST
  CXXFEATURECHECK_DEBUG)

set_target_properties(benchmark PROPERTIES EXCLUDE_FROM_ALL TRUE)

add_executable(gahm_benchmark gahm_benchmark.cpp)
target_include_directories(gahm_benchmark
                           PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../src)
target_link_libraries(gahm_benchmark gahm benchmark::benchmark)
add_dependencies(gahm_benchmark gahm_static)
