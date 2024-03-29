file(GLOB_RECURSE FuzzList ${CMAKE_CURRENT_SOURCE_DIR}/FUZZ_*.cpp)

# Allow short runs during automated testing to see if something new breaks
set(FUZZ_RUNTIME
    10
    CACHE STRING "Number of seconds to run fuzz tests during ctest run"
)# Default of 10 seconds

foreach(test ${FuzzList})
  get_filename_component(test_name ${test} NAME_WE)
  add_executable(${test_name} ${test})
  add_test(NAME ${test_name} COMMAND ${test_name}
                                     -max_total_time=${FUZZ_RUNTIME})
  target_include_directories(${test_name}
                             PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../../src
  )
  include_directories(${test_name} SYSTEM
                             ${CMAKE_CURRENT_SOURCE_DIR}/../thirdparty/fmt-9.1.0/include
                             ${Boost_INCLUDE_DIRS}
  )
  target_link_libraries(
    ${test_name} PRIVATE gahm_objectlib gahm_interface project_options project_warnings -coverage
                         -fsanitize=fuzzer,undefined,address)
  target_compile_options(${test_name}
                         PRIVATE -fsanitize=fuzzer,undefined,address)

  if(GAHM_ENABLE_COVERAGE)
    set_target_properties(
            ${test_name} PROPERTIES COMPILE_FLAGS ${GAHM_COVERAGE_COMPILE_FLAGS}
            LINK_FLAGS ${GAHM_COVERAGE_LINK_FLAGS})
  endif()

  set_target_properties(
    ${test_name} PROPERTIES RUNTIME_OUTPUT_DIRECTORY
                            ${CMAKE_CURRENT_BINARY_DIR}/fuzz)
  add_dependencies(${test_name} gahm_objectlib gahm_interface)
endforeach()
