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
# ##############################################################################

# ##############################################################################
# Fortran
# ##############################################################################
if(GAHM_ENABLE_FORTRAN)
  list(APPEND SOURCES fortran/gahm_fortran.cpp fortran/gahm.F90)
  set(CMAKE_Fortran_MODULE_DIRECTORY ${CMAKE_BINARY_DIR}/fortran)
endif()
# ##############################################################################

add_library(gahm_interface INTERFACE)
add_library(gahm_objectlib OBJECT ${SOURCES})

add_library(gahm_static STATIC $<TARGET_OBJECTS:gahm_objectlib>)
target_link_libraries(gahm_static PUBLIC gahm_interface)

if(BUILD_SHARED_LIBS)
    add_library(gahm SHARED $<TARGET_OBJECTS:gahm_objectlib>)
    target_link_libraries(gahm PUBLIC gahm_interface)
endif()

#...If we detected that the system did not have std::uncaught_exceptions, then
# define HAS_UNCAUGHT_EXCEPTIONS to 0 otherwise leave it undefined. This is an
# oddity of C++17 on MacOS. We're doing this because it is in Howard Hinant's
# date library, and we don't want to force C++20 onto users
if(NOT HAVE_STD_UNCAUGHT_EXCEPTIONS)
  target_compile_definitions(gahm_objectlib PRIVATE HAS_UNCAUGHT_EXCEPTIONS=0)
endif()

target_include_directories(
  gahm_objectlib
  PRIVATE ${CMAKE_CURRENT_SOURCE_DIR} ${Boost_INCLUDE_DIRS}
          ${CMAKE_CURRENT_SOURCE_DIR}/../thirdparty/date_hh
          ${CMAKE_CURRENT_SOURCE_DIR}/../thirdparty/fmt-9.1.0/include)

if(GAHM_ENABLE_WARNINGS)
  target_link_libraries(gahm_objectlib INTERFACE project_warnings)
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
  set_target_properties(
    gahm_static PROPERTIES COMPILE_FLAGS ${GAHM_COVERAGE_COMPILE_FLAGS}
                           LINK_FLAGS ${GAHM_COVERAGE_LINK_FLAGS})
  if(BUILD_SHARED_LIBS)
    set_target_properties(
      gahm PROPERTIES COMPILE_FLAGS ${GAHM_COVERAGE_COMPILE_FLAGS}
                  LINK_FLAGS ${GAHM_COVERAGE_LINK_FLAGS})
  endif()
elseif(GAHM_ENABLE_COVERAGE)
  message(WARNING "Coverage is only supported with GCC")
endif()

# ##############################################################################
# PyGAHM
# ##############################################################################
if(GAHM_ENABLE_PYTHON)
  add_subdirectory(swig)
endif()
# ##############################################################################
