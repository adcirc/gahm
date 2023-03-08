cmake_minimum_required(VERSION 3.24)
project(gahm)

set(CMAKE_CXX_STANDARD 17)

FIND_PACKAGE(Boost 1.75.0 REQUIRED)

add_library(gahm SHARED
        src/datatypes/CircularArray.h
        src/datatypes/Date.h
        src/datatypes/Date.cpp
        src/physical/Constants.h
        src/physical/Earth.h
        src/physical/Units.h
        src/atcf/AtcfSnap.cpp
        src/atcf/AtcfSnap.h
        src/atcf/AtcfIsotach.h
        src/atcf/AtcfQuadrant.h
        src/atcf/AtcfFile.cpp
        src/atcf/AtcfFile.h
        src/util/StringUtilities.h
        src/preprocessor/Preprocessor.cpp
        src/gahm/GahmEquations.cpp
        src/gahm/GahmEquations.h
        src/gahm/GahmRadiusSolver.cpp
        src/gahm/GahmRadiusSolver.h
        src/gahm/GahmSolver.cpp
        src/gahm/GahmSolver.h
        src/gahm/GahmRadiusSolverPrivate.cpp
        src/gahm/GahmRadiusSolverPrivate.h)


target_include_directories(gahm PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${Boost_INCLUDE_DIRS}
        ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/date_hh)

option(BUILD_TESTS "Build tests" ON)
if (BUILD_TESTS)
    enable_testing()
    Include(FetchContent)
    FetchContent_Declare(
            Catch2
            GIT_REPOSITORY https://github.com/catchorg/Catch2.git
            GIT_TAG        v3.3.2
    )
    FetchContent_MakeAvailable(Catch2)
    FILE(GLOB_RECURSE TestList ${CMAKE_CURRENT_SOURCE_DIR}/tests/cxx_tests/TEST_*.cpp)

    foreach (test ${TestList})
        get_filename_component(test_name ${test} NAME_WE)
        add_executable(${test_name} ${test} src/physical/Atmospheric.h)
        add_test(NAME ${test_name} COMMAND ${test_name}
                 WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests)
        target_include_directories(${test_name} PRIVATE
                ${CMAKE_CURRENT_SOURCE_DIR}/src
                ${catch2_SOURCE_DIR}
                )
        target_link_libraries(${test_name} PRIVATE gahm Catch2::Catch2WithMain)
        set_target_properties(${test_name} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/tests)
        add_dependencies(${test_name} gahm Catch2::Catch2WithMain)
    endforeach ()
endif ()