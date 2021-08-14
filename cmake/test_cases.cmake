option(GAHM_ENABLE_TESTING OFF "Enable building the test suite")
if(GAHM_ENABLE_TESTING)
  file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/testcases)

  set(TEST_LIST cxx_atcf.cpp cxx_physical.cpp cxx_owi.cpp)

  if(GAHM_ENABLE_FORTRAN)
      set(TEST_LIST ${TEST_LIST} ftn_gahm.f90)
  endif()

  foreach(TESTFILE ${TEST_LIST})
    get_filename_component(TESTNAME ${TESTFILE} NAME_WE)
    add_executable(${TESTNAME} ${CMAKE_SOURCE_DIR}/testing/test_cases/${TESTFILE})
    add_dependencies(${TESTNAME} gahm)
    target_link_libraries(${TESTNAME} gahm)
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
      NAME TEST_${TESTNAME}
      COMMAND ${CMAKE_BINARY_DIR}/testcases/${TESTNAME}
      WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/testing)
  endforeach()

endif()
