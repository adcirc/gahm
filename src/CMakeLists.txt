# ##############################################################################
# GAHM library
# ##############################################################################
add_library(
  gahm
  datatypes/Date.cpp
  atcf/AtcfSnap.cpp
  atcf/AtcfFile.cpp
  preprocessor/Preprocessor.cpp
  gahm/GahmEquations.cpp
  gahm/GahmRadiusSolver.cpp
  gahm/GahmSolver.cpp
  gahm/GahmRadiusSolverPrivate.cpp)

target_include_directories(gahm PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

include_directories(
  SYSTEM ${Boost_INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR}/../thirdparty/date_hh
  ${CMAKE_CURRENT_SOURCE_DIR}/../thirdparty/fmt-9.1.0/include)

target_link_libraries(gahm INTERFACE project_options project_warnings)

target_link_libraries(gahm PRIVATE fmt::fmt)
add_dependencies(gahm fmt::fmt)

# ##############################################################################