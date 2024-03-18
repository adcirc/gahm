find_package(SWIG 4.0 REQUIRED)
find_package(
  Python3
  COMPONENTS Interpreter Development.Module
  REQUIRED)

message(STATUS "Python3_EXECUTABLE: ${Python3_EXECUTABLE}")
message(STATUS "Python3_INCLUDE_DIRS: ${Python3_INCLUDE_DIRS}")
message(STATUS "Python3_SiteLibs: ${Python3_SITELIB}")

add_custom_command(
  OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/python_gahm_wrap.cxx
  COMMAND
    ${SWIG_EXECUTABLE} -outdir ${CMAKE_CURRENT_BINARY_DIR} -c++ -python
    -I${CMAKE_CURRENT_SOURCE_DIR}/.. -I${Python3_INCLUDE_DIRS} -o
    ${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/python_gahm_wrap.cxx
    ${CMAKE_CURRENT_SOURCE_DIR}/gahm.i
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
  DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/gahm.i
  COMMENT "Generating SWIG interface to Python...")

add_library(pygahm SHARED
            ${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/python_gahm_wrap.cxx)
target_include_directories(pygahm PRIVATE ${Python3_INCLUDE_DIRS}
                                          ${CMAKE_CURRENT_SOURCE_DIR}/..)
target_link_libraries(pygahm gahm_objectlib)

if(APPLE)
  target_link_libraries(pygahm Python3::Module)
endif(APPLE)

set_target_properties(pygahm PROPERTIES PREFIX "_")
set_target_properties(pygahm PROPERTIES INSTALL_NAME_DIR "gahm")
set_property(
  DIRECTORY
  APPEND
  PROPERTY ADDITIONAL_MAKE_CLEAN_FILES pygahm.py
           CMakeFiles/python_gahm_wrap.cxx)

add_dependencies(pygahm gahm_objectlib)

if(PYTHON_PACKAGE_BUILD)
  set_target_properties(pygahm PROPERTIES LIBRARY_OUTPUT_DIRECTORY
                                          ${CMAKE_INSTALL_PREFIX})
  add_custom_command(
    TARGET pygahm
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_BINARY_DIR}/pygahm.py
            ${CMAKE_INSTALL_PREFIX}/pygahm.py)
else()
  install(TARGETS pygahm LIBRARY DESTINATION ${Python3_SITELIB}/pygahm)
  install(FILES ${CMAKE_CURRENT_BINARY_DIR}/pygahm.py
          DESTINATION ${Python3_SITELIB}/pygahm)
  install(
    FILES ${CMAKE_CURRENT_SOURCE_DIR}/init_py.default
    DESTINATION ${Python3_SITELIB}/pygahm
    RENAME __init__.py)
endif()

if(APPLE)
  set_target_properties(pygahm PROPERTIES SUFFIX ".so")
endif()
# ##############################################################################
