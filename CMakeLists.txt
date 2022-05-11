cmake_minimum_required(VERSION 3.20)
project(gahm2)

set(CMAKE_CXX_STANDARD 17)

add_library(gahm SHARED
        ${CMAKE_CURRENT_SOURCE_DIR}/src/StormIsotach.h
        ${CMAKE_CURRENT_SOURCE_DIR}/src/ForecastPeriod.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/ForecastPeriod.h
        ${CMAKE_CURRENT_SOURCE_DIR}/src/Date.h
        ${CMAKE_CURRENT_SOURCE_DIR}/src/Date.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/StormPosition.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/StormPosition.h
        ${CMAKE_CURRENT_SOURCE_DIR}/src/UnitConversion.h
        ${CMAKE_CURRENT_SOURCE_DIR}/src/Physical.h
        ${CMAKE_CURRENT_SOURCE_DIR}/src/Atmospheric.h
        ${CMAKE_CURRENT_SOURCE_DIR}/src/Constants.h
        ${CMAKE_CURRENT_SOURCE_DIR}/src/GahmRadiusSolver.h
        ${CMAKE_CURRENT_SOURCE_DIR}/src/GahmRadiusSolver.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/GahmRadiusSolverPrivate.h
        ${CMAKE_CURRENT_SOURCE_DIR}/src/GahmRadiusSolverPrivate.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/Logging.h
        ${CMAKE_CURRENT_SOURCE_DIR}/src/Logging.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/StormIsotach.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/CircularArray.h
        ${CMAKE_CURRENT_SOURCE_DIR}/src/GahmSolver.h
        ${CMAKE_CURRENT_SOURCE_DIR}/src/GahmSolver.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/StormQuadrant.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/StormQuadrant.h
        ${CMAKE_CURRENT_SOURCE_DIR}/src/GahmEquations.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/GahmEquations.h
        ${CMAKE_CURRENT_SOURCE_DIR}/src/PositionWeights.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/PositionWeights.h
        ${CMAKE_CURRENT_SOURCE_DIR}/src/StringUtilities.h
        ${CMAKE_CURRENT_SOURCE_DIR}/src/Atcf.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/Atcf.h
        ${CMAKE_CURRENT_SOURCE_DIR}/src/GahmVortex.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/GahmVortex.h
        ${CMAKE_CURRENT_SOURCE_DIR}/src/Vortex.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/Vortex.h)

target_include_directories(gahm PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/date_hh
        ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/fmt-8.1.1/include
        ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/boost_1_79_0)


add_executable(test1 ${CMAKE_CURRENT_SOURCE_DIR}/test/test.cpp)
target_link_libraries(test1 gahm)
add_dependencies(test1 gahm)
target_include_directories(test1 PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_include_directories(test1 PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/fmt-8.1.1/include)