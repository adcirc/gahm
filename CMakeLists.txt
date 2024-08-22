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
cmake_minimum_required(VERSION 3.19)
project(gahm CXX)
enable_testing()

set(CMAKE_POSITION_INDEPENDENT_CODE ON)
mark_as_advanced(CLEAR CMAKE_BUILD_TYPE)
mark_as_advanced(CLEAR CMAKE_CXX_COMPILER)
mark_as_advanced(CLEAR CMAKE_CXX_FLAGS)
mark_as_advanced(CLEAR CMAKE_CXX_FLAGS_RELEASE)
mark_as_advanced(CLEAR CMAKE_CXX_FLAGS_DEBUG)

# ##############################################################################
# C++ Checks
# ##############################################################################
# Require C++20 Support from the compiler
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_NINJA_FORCE_RESPONSE_FILE "ON" CACHE BOOL "Force Ninja to use response files.")
mark_as_advanced(CMAKE_NINJA_FORCE_RESPONSE_FILE)

# Check for C++20 support
include(CheckCXXCompilerFlag)
check_cxx_compiler_flag("-std=c++20" COMPILER_SUPPORTS_CXX20)
if(NOT COMPILER_SUPPORTS_CXX20)
  message(FATAL_ERROR "The compiler ${CMAKE_CXX_COMPILER} has no C++20 support. Please use a different C++ compiler.")
endif()
# ##############################################################################

# ##############################################################################
# Set up the build options
# ##############################################################################
include(cmake/PreventInSourceBuilds.cmake)
include(cmake/ProjectOptions.cmake)
gahm_setup_options()
gahm_global_options()
gahm_local_options()
add_library(gahm::gahm_options ALIAS gahm_options)
add_library(gahm::gahm_warnings ALIAS gahm_warnings)
# ##############################################################################

# ##############################################################################
# Set Up the Dependencies
option(gahm_ENABLE_PLOTTING "Enable plotting with VTK" OFF)
option(gahm_ENABLE_BENCHMARKS "Enable benchmarks with Google Benchmark" OFF)
if(NOT DEFINED gahm_BUILD_TESTS AND gahm_MAINTAINER_MODE)
  option(gahm_BUILD_TESTS "Build all tests." ON)
else()
  option(gahm_BUILD_TESTS "Build all tests." OFF)
endif()
include(cmake/Dependencies.cmake)
gahm_setup_dependencies()
# ##############################################################################

# ##############################################################################
# Set up the build
# ##############################################################################
add_subdirectory(src)
add_subdirectory(tests)