cmake_minimum_required(VERSION 3.0)

# ...Set the default build type
if(DEFINED CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE
      ${CMAKE_BUILD_TYPE}
      CACHE STRING "Choose the type of
        build, options are: None(CMAKE_CXX_FLAGS or CMAKE_C_FLAGS used) Debug
        Release RelWithDebInfo MinSizeRel.")
else()
  set(CMAKE_BUILD_TYPE
      Release
      CACHE STRING "Choose the type of build,
        options are: None(CMAKE_CXX_FLAGS or CMAKE_C_FLAGS used) Debug Release
        RelWithDebInfo MinSizeRel.")
endif()

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
cmake_policy(SET CMP0048 NEW)
project(gahm LANGUAGES CXX VERSION 0.0.1)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(CheckIncludeFiles)
include(CheckLibraryExists)
include(CheckFunctionExists)
include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

mark_as_advanced(
  CLEAR
  CMAKE_C_COMPILER
  CMAKE_CXX_COMPILER
  CMAKE_C_CFLAGS
  CMAKE_CXX_FLAGS
  CMAKE_C_FLAGS_DEBUG
  CMAKE_C_FLAGS_RELEASE
  CMAKE_CXX_FLAGS_DEBUG
  CMAKE_CXX_FLAGS_RELEASE)

add_library(gahm SHARED
  ${CMAKE_CURRENT_SOURCE_DIR}/src/Gahm.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/Gahm.h
  ${CMAKE_CURRENT_SOURCE_DIR}/src/Physical.h
  ${CMAKE_CURRENT_SOURCE_DIR}/src/Atcf.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/Atcf.h
  ${CMAKE_CURRENT_SOURCE_DIR}/src/AtcfLine.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/AtcfLine.h
  ${CMAKE_CURRENT_SOURCE_DIR}/src/Isotach.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/Isotach.h
  ${CMAKE_CURRENT_SOURCE_DIR}/src/Date.h
  ${CMAKE_CURRENT_SOURCE_DIR}/src/Date.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/HurricanePressure.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/HurricanePressure.h
  ${CMAKE_CURRENT_SOURCE_DIR}/src/Logging.h
  ${CMAKE_CURRENT_SOURCE_DIR}/src/Logging.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/Vortex.h
  ${CMAKE_CURRENT_SOURCE_DIR}/src/Vortex.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/Assumptions.h
  ${CMAKE_CURRENT_SOURCE_DIR}/src/Assumptions.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/Assumption.h
  ${CMAKE_CURRENT_SOURCE_DIR}/src/Assumption.cpp
)

target_compile_definitions(gahm PRIVATE GAHM_LIBRARY)
target_include_directories(gahm PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/date_hh
    ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/boost_1_75_0)

include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/test_cases.cmake)

