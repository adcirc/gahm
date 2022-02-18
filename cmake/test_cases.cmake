option(GAHM_ENABLE_TESTING OFF "Enable building the test suite")
if(GAHM_ENABLE_TESTING)

  # Pre-build the Catch2 main because it is a lengthy compile
  add_library(catch_boilerplate ${CMAKE_CURRENT_SOURCE_DIR}/testing/test_cases/catch_boilerplate.cpp)
  target_include_directories(catch_boilerplate PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/catch2)

  file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/testcases)

  set(TEST_LIST cxx_atcf.cpp cxx_physical.cpp cxx_owi.cpp)

  if(GAHM_ENABLE_FORTRAN)
      set(TEST_LIST ${TEST_LIST} ftn_gahm.f90)
  endif()

  foreach(TESTFILE ${TEST_LIST})
    get_filename_component(TESTNAME test_${TESTFILE} NAME_WE)
    add_executable(${TESTNAME} ${CMAKE_SOURCE_DIR}/testing/test_cases/${TESTFILE})
    add_dependencies(${TESTNAME} gahm catch_boilerplate)
    target_link_libraries(${TESTNAME} gahm catch_boilerplate)
    target_include_directories(
      ${TESTNAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src
                          ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/catch2)
    set_target_properties(
      ${TESTNAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY
                             ${CMAKE_BINARY_DIR}/testcases)

    if(GAHM_ENABLE_FORTRAN)
        target_include_directories(${TESTNAME} PRIVATE ${CMAKE_BINARY_DIR}/CMakeFiles/mod/gahm)
    endif()

    add_test(
      NAME ${TESTNAME}
      COMMAND ${CMAKE_BINARY_DIR}/testcases/${TESTNAME}
      WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/testing)
  endforeach()

endif()
