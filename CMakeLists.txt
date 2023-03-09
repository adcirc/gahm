cmake_minimum_required(VERSION 3.24)
project(gahm)

set(CMAKE_CXX_STANDARD 17)

FIND_PACKAGE(Boost 1.75.0 REQUIRED)

add_subdirectory(thirdparty/fmt-9.1.0 EXCLUDE_FROM_ALL)

add_library(gahm SHARED
        src/datatypes/Date.cpp
        src/atcf/AtcfSnap.cpp
        src/atcf/AtcfFile.cpp
        src/preprocessor/Preprocessor.cpp
        src/gahm/GahmEquations.cpp
        src/gahm/GahmRadiusSolver.cpp
        src/gahm/GahmSolver.cpp
        src/gahm/GahmRadiusSolverPrivate.cpp)

target_include_directories(gahm PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${Boost_INCLUDE_DIRS}
        ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/date_hh
        ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/fmt-9.1.0/include)

target_link_libraries(gahm PRIVATE fmt::fmt)
add_dependencies(gahm fmt::fmt)

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
        add_executable(${test_name} ${test})
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