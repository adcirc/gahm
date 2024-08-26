# ##############################################################################
# ...CXX TESTS
# ##############################################################################
include(${CMAKE_CURRENT_SOURCE_DIR}/../../cmake/SystemLink.cmake)

file(GLOB_RECURSE TestList ${CMAKE_CURRENT_SOURCE_DIR}/TEST_*.cpp)

foreach(test ${TestList})
    get_filename_component(test_name ${test} NAME_WE)
    add_executable(${test_name} ${test})
    add_test(
            NAME ${test_name}
            COMMAND ${test_name}
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

    target_include_directories(${test_name} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../../src/libgahm)
    target_include_directories(${test_name} SYSTEM PUBLIC
            ${CMAKE_CURRENT_BINARY_DIR}/../../_deps/catch2-src/src
            ${CMAKE_CURRENT_BINARY_DIR}/../../_deps/catch2-build/generated-includes)
    target_link_system_libraries(${test_name} PUBLIC Catch2WithMain fmt::fmt)
    set_target_properties(
            ${test_name} PROPERTIES RUNTIME_OUTPUT_DIRECTORY
            ${CMAKE_CURRENT_BINARY_DIR}/tests)

    # Disable clang-tidy and cpp-check on the tests
    set_target_properties(${test_name} PROPERTIES CXX_CLANG_TIDY "" CXX_CPPCHECK "")

    target_link_libraries(${test_name} PRIVATE gahm_objects)
    add_dependencies(${test_name} gahm_objects)
endforeach()
# ##############################################################################