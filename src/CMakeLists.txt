# ##############################################################################
# GAHM library
# ##############################################################################
set(SOURCES
    datatypes/Date.h
    datatypes/Date.cpp
    datatypes/Point.h
    datatypes/PointCloud.h
    datatypes/PointPosition.h
    datatypes/Uvp.h
    datatypes/VortexSolution.h
    datatypes/WindGrid.h
    atcf/AtcfFile.h
    atcf/AtcfSnap.h
    atcf/AtcfIsotach.h
    atcf/AtcfQuadrant.h
    atcf/StormPosition.h
    atcf/StormTranslation.h
    atcf/AtcfSnap.cpp
    atcf/AtcfFile.cpp
    preprocessor/Preprocessor.h
    preprocessor/Preprocessor.cpp
    gahm/GahmEquations.h
    gahm/GahmRadiusSolver.h
    gahm/GahmSolver.h
    gahm/GahmRadiusSolverPrivate.h
    gahm/GahmEquations.cpp
    gahm/GahmRadiusSolver.cpp
    gahm/GahmSolver.cpp
    gahm/GahmRadiusSolverPrivate.cpp
    vortex/Vortex.h
    vortex/Vortex.cpp
    output/OwiOutput.cpp
    output/OutputFile.h
    output/OwiOutput.h
    physical/Atmospheric.h
    physical/Constants.h
    physical/Earth.h
    physical/Units.h
    util/Interpolation.h
    util/StringUtilities.h)

# ##############################################################################

# ##############################################################################
# Fortran
# ##############################################################################
if(GAHM_ENABLE_FORTRAN)
  set(CMAKE_Fortran_MODULE_DIRECTORY ${CMAKE_BINARY_DIR}/fortran)
  add_library(
    gahm_fortran OBJECT ${CMAKE_CURRENT_SOURCE_DIR}/fortran/gahm.F90
                        ${CMAKE_CURRENT_SOURCE_DIR}/fortran/gahm_fortran.cpp)
  target_include_directories(gahm_fortran PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
endif()
# ##############################################################################

add_library(gahm_interface INTERFACE)
add_library(gahm_objectlib OBJECT ${SOURCES})

if(GAHM_ENABLE_STATIC)
  add_library(gahm_static STATIC $<TARGET_OBJECTS:gahm_objectlib>)
  set_target_properties(gahm_static PROPERTIES OUTPUT_NAME gahm)
  target_link_libraries(gahm_static PUBLIC gahm_interface)
  if(GAHM_ENABLE_FORTRAN)
    target_link_libraries(gahm_static PRIVATE gahm_fortran)
  endif()
endif()

if(GAHM_ENABLE_SHARED)
  add_library(gahm_shared SHARED $<TARGET_OBJECTS:gahm_objectlib>)
  set_target_properties(gahm_shared PROPERTIES OUTPUT_NAME gahm)
  target_link_libraries(gahm_shared PUBLIC gahm_interface)
  if(GAHM_ENABLE_FORTRAN)
    target_link_libraries(gahm_shared PRIVATE gahm_fortran)
  endif()
endif()

# ...If we detected that the system did not have std::uncaught_exceptions, then
# define HAS_UNCAUGHT_EXCEPTIONS to 0 otherwise leave it undefined. This is an
# oddity of C++17 on MacOS. We're doing this because it is in Howard Hinant's
# date library, and we don't want to force C++20 onto users
if(NOT GAHM_HAS_UNCAUGHT_EXCEPTIONS)
  target_compile_definitions(gahm_objectlib PRIVATE HAS_UNCAUGHT_EXCEPTIONS=0)
endif()

target_include_directories(gahm_objectlib PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(
  gahm_objectlib SYSTEM
  PRIVATE ${Boost_INCLUDE_DIRS}
          ${CMAKE_CURRENT_SOURCE_DIR}/../thirdparty/date_hh
          ${CMAKE_CURRENT_SOURCE_DIR}/../thirdparty/fmt-9.1.0/include)

if(GAHM_ENABLE_WARNINGS)
  target_link_libraries(gahm_objectlib PRIVATE project_warnings)
endif()

target_link_libraries(gahm_interface INTERFACE project_options)
target_link_libraries(gahm_interface INTERFACE fmt::fmt)
add_dependencies(gahm_interface fmt::fmt)

if(GAHM_ENABLE_COVERAGE AND CMAKE_CXX_COMPILER_ID MATCHES "GNU")
  message(
    STATUS
      "Enabling coverage for GAHM with flags ${GAHM_COVERAGE_COMPILE_FLAGS}")
  set_target_properties(
    gahm_objectlib PROPERTIES COMPILE_FLAGS ${GAHM_COVERAGE_COMPILE_FLAGS}
                              LINK_FLAGS ${GAHM_COVERAGE_LINK_FLAGS})
  if(GAHM_ENABLE_STATIC)
    set_target_properties(
      gahm_static PROPERTIES COMPILE_FLAGS ${GAHM_COVERAGE_COMPILE_FLAGS}
                             LINK_FLAGS ${GAHM_COVERAGE_LINK_FLAGS})
  endif()
  if(GAHM_ENABLE_SHARED)
    set_target_properties(
      gahm_shared PROPERTIES COMPILE_FLAGS ${GAHM_COVERAGE_COMPILE_FLAGS}
                             LINK_FLAGS ${GAHM_COVERAGE_LINK_FLAGS})
  endif()
elseif(GAHM_ENABLE_COVERAGE)
  message(WARNING "Coverage is only supported with GCC")
endif()

# ##############################################################################
# Install
# ##############################################################################
if(GAHM_ENABLE_STATIC)
install(
  DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
  FILES_MATCHING
  PATTERN "*.h"
  PATTERN "*Private.h" EXCLUDE)
install(
  TARGETS gahm_static
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  RUNTIME DESTINATION bin)
endif()

if(GAHM_ENABLE_SHARED)
  install(
    TARGETS gahm_shared
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin)
endif()

# ##############################################################################
# PyGAHM
# ##############################################################################
if(GAHM_ENABLE_PYTHON)
  add_subdirectory(swig)
endif()
# ##############################################################################
