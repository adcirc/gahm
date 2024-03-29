# ##############################################################################
# GAHM - Generalized Asymmetric Holland Model
#
# Authors: - Zach Cobell - Rick Luettich
#
# Derivative of work by: Jie Gao
#
# License: MIT
#
# ##############################################################################
# Set up the project
# ##############################################################################
cmake_minimum_required(VERSION 3.18)
project(gahm CXX)

# Warnings, Sanitizers, etc
add_library(project_options INTERFACE)
add_library(project_warnings INTERFACE)
include(cmake/StandardProjectSettings.cmake)
include(cmake/PreventInSourceBuilds.cmake)
include(cmake/Cache.cmake)
include(cmake/CompilerWarnings.cmake)
set(CMAKE_CXX_STANDARD 17)
target_compile_features(project_options INTERFACE cxx_std_17)
set_project_warnings(project_warnings)
include(cmake/Sanitizers.cmake)
enable_sanitizers(project_options)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

mark_as_advanced(CLEAR CMAKE_BUILD_TYPE)
mark_as_advanced(CLEAR CMAKE_CXX_COMPILER)
mark_as_advanced(CLEAR CMAKE_CXX_FLAGS)
mark_as_advanced(CLEAR CMAKE_CXX_FLAGS_RELEASE)
mark_as_advanced(CLEAR CMAKE_CXX_FLAGS_DEBUG)

option(GAHM_ENABLE_WARNINGS "Enable verbose compiler warnings" OFF)
option(GAHM_ENABLE_COVERAGE "Enable code coverage" OFF)

option(GAHM_ENABLE_SHARED "Enable compilation of shared GAHM library" ON)
option(GAHM_ENABLE_STATIC "Enable compilation of static GAHM library" OFF)

# ##############################################################################
# C++ Checks
# ##############################################################################
# Check if the compiler supports std::uncaught_exceptions()
include(CheckCXXSourceCompiles)
set(CMAKE_REQUIRED_FLAGS "-std=c++17")
check_cxx_source_compiles(
  "
#include <exception>
int main() {
  return std::uncaught_exceptions();
}
"
  GAHM_HAS_UNCAUGHT_EXCEPTIONS)
unset(CMAKE_REQUIRED_FLAGS)
# ##############################################################################

# ##############################################################################

# ##############################################################################
# Coverage
# ##############################################################################
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
  set(GAHM_COVERAGE_COMPILE_FLAGS
      "-g -fprofile-arcs -ftest-coverage"
      CACHE STRING "Flags used by the C++ compiler during coverage builds."
            FORCE)
  set(GAHM_COVERAGE_LINK_FLAGS
      "-fprofile-arcs"
      CACHE STRING "Flags used by the C++ linker during coverage builds." FORCE)
else()
  set(GAHM_COVERAGE_COMPILE_FLAGS
      "-g"
      CACHE STRING "Flags used by the C++ compiler during coverage builds."
            FORCE)
  set(GAHM_COVERAGE_LINK_FLAGS
      ""
      CACHE STRING "Flags used by the C++ linker during coverage builds." FORCE)
endif()
mark_as_advanced(GAHM_COVERAGE_FLAGS)
# ##############################################################################

# ##############################################################################
# Dependencies
# ##############################################################################
find_package(Boost 1.71.0 REQUIRED)
add_subdirectory(thirdparty/fmt-9.1.0 EXCLUDE_FROM_ALL)
mark_as_advanced(
  FMT_CUDA_TEST
  FMT_DEBUG_POSTFIX
  FMT_DOC
  FMT_FUZZ
  FMT_INC_DIR
  FMT_INSTALL
  FMT_MODULE
  FMT_OS
  FMT_PEDANTIC
  FMT_SYSTEM_HEADERS
  FMT_TEST
  FMT_WERROR)
# ##############################################################################

# ##############################################################################
# Fortran
# ##############################################################################
option(GAHM_ENABLE_FORTRAN "Enable Fortran bindings" OFF)
if(GAHM_ENABLE_FORTRAN)
  enable_language(Fortran)
endif()
# ##############################################################################

# ##############################################################################
# PYTHON
# ##############################################################################
option(GAHM_ENABLE_PYTHON "Enable Python bindings" OFF)
# ##############################################################################

# ##############################################################################
# Source
# ##############################################################################
add_subdirectory(src)
# ##############################################################################

# ##############################################################################
# Tests (Unit, Fuzz, Benchmarks)
# ##############################################################################
option(GAHM_ENABLE_TESTS "Build tests" OFF)
if(GAHM_ENABLE_TESTS)
  message(STATUS "GAHM tests enabled")
  enable_testing()
  add_subdirectory(tests)

  if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    option(GAHM_ENABLE_FUZZ "Build Fuzzing" OFF)
    if(GAHM_ENABLE_FUZZ)
      message(STATUS "Building Fuzz tests")
      add_subdirectory(tests/fuzz)
    endif()
  endif()

  option(GAHM_ENABLE_BENCHMARKS "Build benchmarks" OFF)
  if(GAHM_ENABLE_BENCHMARKS)
    message(STATUS "GAHM benchmarks enabled")
    add_subdirectory(tests/benchmark)
  endif()

endif()
# ##############################################################################
