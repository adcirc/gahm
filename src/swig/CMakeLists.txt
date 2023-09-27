find_package(SWIG REQUIRED)
find_package(
  Python3
  COMPONENTS Interpreter Development
  REQUIRED)

message(STATUS "Python3_EXECUTABLE: ${Python3_EXECUTABLE}")
message(STATUS "Python3_INCLUDE_DIRS: ${Python3_INCLUDE_DIRS}")
message(STATUS "Python3_LIBRARIES: ${Python3_LIBRARIES}")
message(STATUS "Python3_SITELIB: ${Python3_SITELIB}")

add_custom_command(
  OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/python_gahm_wrap.cxx
  COMMAND
    ${SWIG_EXECUTABLE} -outdir ${CMAKE_CURRENT_BINARY_DIR} -c++ -python
    -I${CMAKE_CURRENT_SOURCE_DIR}/.. -I${Python3_INCLUDE_DIRS} -o
    ${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/python_gahm_wrap.cxx
    ${CMAKE_CURRENT_SOURCE_DIR}/gahm.i
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
  COMMENT "Generating SWIG interface to Python...")

add_library(pygahm SHARED
            ${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/python_gahm_wrap.cxx)
target_include_directories(pygahm PRIVATE ${Python3_INCLUDE_DIRS}
                                          ${CMAKE_CURRENT_SOURCE_DIR}/..)
target_link_libraries(pygahm gahm_static)

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

add_dependencies(pygahm gahm)

set_target_properties(pygahm PROPERTIES LIBRARY_OUTPUT_DIRECTORY
                                        ${CMAKE_CURRENT_BINARY_DIR})
install(TARGETS pygahm LIBRARY DESTINATION ${Python3_SITELIB})
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/pygahm.py
        DESTINATION ${Python3_SITELIB})

if(APPLE)
  set_target_properties(pygahm PROPERTIES SUFFIX ".so")
endif()
# ##############################################################################
