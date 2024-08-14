find_package(
  Boost 1.58.0
  COMPONENTS headers
  REQUIRED)

set(GAHM_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/datatypes/Datetime.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/datatypes/Vec.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/datatypes/Point.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/datatypes/RotationMatrix.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/util/ioutils.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/atcf/AtcfPeriod.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/atcf/AtcfTrack.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/atcf/AtcfIO.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/storm/Isotach.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/storm/Quadrant.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/storm/StormTranslation.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/gahm/GahmParameters.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/gahm/GahmEquations.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/gahm/GahmRadiusSolver.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/gahm/GahmRadiusSolverPrivate.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/gahm/GahmSolver.cpp
)

add_library(gahm_objects OBJECT ${GAHM_SOURCES})
target_include_directories(gahm_objects PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(gahm_objects PUBLIC gahm::gahm_warnings gahm::gahm_options Boost::headers)
target_compile_features(gahm_objects PUBLIC cxx_std_20)

add_library(gahm SHARED $<TARGET_OBJECTS:gahm_objects>)
