# MIT License
#
# Copyright (c) 2020 ADCIRC Development Group
#
# Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated
# documentation files (the "Software"), to deal in the Software without restriction, including without limitation the
# rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit
# persons to whom the Software is furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all copies or substantial portions of the
# Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
# WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
# COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
# OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
#
# Author: Zach Cobell Contact: zcobell@thewaterinstitute.org
#

cmake_minimum_required(VERSION 3.0)
set(GAHM_VERSION_MAJOR 0)
set(GAHM_VERSION_MINOR 0)
set(GAHM_VERSION_PATCH 1)
set(GAHM_VERSION_STRING ${GAHM_VERSION_MAJOR}.${GAHM_VERSION_MINOR}.${GAHM_VERSION_PATCH})

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

option(GAHM_ENABLE_FORTRAN OFF "Build the FORTRAN interface to GAHM")
if(GAHM_ENABLE_FORTRAN)
  enable_language(Fortran)
endif()

option(GAHM_BUILD_SHARED_LIB OFF "Build the GAHM library as a shared object")
set(GAHM_LIBRARY_TYPE STATIC)
if(GAHM_BUILD_SHARED_LIB)
  set(GAHM_LIBRARY_TYPE SHARED)
endif()

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
cmake_policy(SET CMP0048 NEW)
project(
  gahm
  LANGUAGES CXX
  VERSION ${GAHM_VERSION_STRING})
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS NO)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

include(CheckIncludeFiles)
include(CheckLibraryExists)
include(CheckFunctionExists)
include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

enable_testing()
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/test_cases.cmake)

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

set(GAHM_LIBRARY_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/src/GahmVortex.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Constants.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Atcf.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/AtcfLine.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Isotach.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Date.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/HurricanePressure.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Logging.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Assumptions.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Assumption.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Vortex.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/CircularArray.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Preprocessor.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/VortexSolver.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/UnitConversion.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Interpolation.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/StormParameters.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/GahmState.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/OwiAscii.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/OwiAsciiDomain.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/WindData.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/WindGrid.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Point.h)

set(GAHM_LIBRARY_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/GahmVortex.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Atcf.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/AtcfLine.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Isotach.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Date.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/HurricanePressure.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Logging.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Vortex.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Assumptions.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Assumption.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Preprocessor.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/VortexSolver.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/GahmState.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/OwiAscii.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/OwiAsciiDomain.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/WindData.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/WindGrid.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Point.cpp)

# ... Fortran sources
if(GAHM_ENABLE_FORTRAN)
  set(GAHM_LIBRARY_SOURCES ${GAHM_LIBRARY_SOURCES} ${CMAKE_CURRENT_SOURCE_DIR}/interfaces/fortran/gahm_fortran.cpp
                           ${CMAKE_CURRENT_SOURCE_DIR}/interfaces/fortran/gahm.F90)
endif()

# ...Generate the library
add_library(gahm ${GAHM_LIBRARY_TYPE} ${GAHM_LIBRARY_SOURCES})
if(GAHM_ENABLE_FORTRAN)
  set_target_properties(gahm PROPERTIES Fortran_MODULE_DIRECTORY ${CMAKE_BINARY_DIR}/CMakeFiles/mod/gahm)
endif()

# ...Definitions
target_compile_definitions(gahm PRIVATE GAHM_LIBRARY)
target_include_directories(gahm PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/date_hh
                                        ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/boost_1_76_0)

write_basic_package_version_file(
  gahmConfigVersion.cmake
  VERSION ${GAHM_VERSION_STRING}
  COMPATIBILITY SameMajorVersion)

# ...Header information
set_target_properties(gahm PROPERTIES PUBLIC_HEADER "${GAHM_LIBRARY_HEADERS}")
set(HEADER_DEST "${CMAKE_INSTALL_INCLUDEDIR}/gahm")

# ...Version information
if(APPLE)
  set_target_properties(gahm PROPERTIES MACHO_CURRENT_VERSION ${GAHM_VERSION_STRING}
                                        MACHO_COMPATIBILITY_VERSION ${GAHM_VERSION_MAJOR}.${GAHM_VERSION_MINOR})
endif()
set_target_properties(gahm PROPERTIES VERSION ${GAHM_VERSION_STRING} SOVERSION ${GAHM_VERSION_MAJOR})

# ...Installation Information
install(
  TARGETS gahm
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT GAHM_RUNTIME
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT GAHM_RUNTIME
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT GAHM_DEVELOPMENT
  PUBLIC_HEADER DESTINATION ${HEADER_DEST} COMPONENT GAHM_RUNTIME)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/gahmConfigVersion.cmake DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake)

# ...Python Interface
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/python.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/python_gahm.cmake)

# ...Benchmarking
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/benchmark.cmake)
