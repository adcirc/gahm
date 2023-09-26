# ##############################################################################
# GAHM library
# ##############################################################################

set(SOURCES
  datatypes/Date.cpp
  atcf/AtcfSnap.cpp
  atcf/AtcfFile.cpp
  preprocessor/Preprocessor.cpp
  gahm/GahmEquations.cpp
  gahm/GahmRadiusSolver.cpp
  gahm/GahmSolver.cpp
  gahm/GahmRadiusSolverPrivate.cpp
  vortex/Vortex.cpp
  vortex/Vortex.cpp
  output/OwiOutput.cpp)

add_library(gahm_interface INTERFACE)
add_library(gahm_objectlib OBJECT ${SOURCES})

add_library(gahm_static STATIC $<TARGET_OBJECTS:gahm_objectlib>)
add_library(gahm SHARED $<TARGET_OBJECTS:gahm_objectlib>)

target_include_directories(gahm_objectlib PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

include_directories(
  SYSTEM ${Boost_INCLUDE_DIRS}
  ${CMAKE_CURRENT_SOURCE_DIR}/../thirdparty/date_hh
  ${CMAKE_CURRENT_SOURCE_DIR}/../thirdparty/fmt-9.1.0/include)

if(GAHM_ENABLE_WARNINGS)
  target_link_libraries(gahm_objectlib INTERFACE project_warnings)
endif()

target_link_libraries(gahm_interface INTERFACE project_options)
target_link_libraries(gahm_interface INTERFACE fmt::fmt)
add_dependencies(gahm_interface fmt::fmt)

target_link_libraries(gahm_static PUBLIC gahm_interface)
target_link_libraries(gahm PUBLIC gahm_interface)

# ##############################################################################
# PyGAHM
# ##############################################################################
if(GAHM_ENABLE_PYTHON)
    add_subdirectory(swig)
endif()
# ##############################################################################
