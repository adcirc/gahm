    file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/cxx_testcases)

    set(TEST_LIST
            cxx_atcf.cpp
            cxx_physical.cpp
            )

    foreach (TESTFILE ${TEST_LIST})
        get_filename_component(TESTNAME ${TESTFILE} NAME_WE)
        add_executable(${TESTNAME}
                ${CMAKE_SOURCE_DIR}/testing/cxx/${TESTFILE})
        add_dependencies(${TESTNAME} gahm)
        target_link_libraries(${TESTNAME} gahm)
        target_include_directories(${TESTNAME} PRIVATE
                ${CMAKE_CURRENT_SOURCE_DIR}/src
                ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/catch2
                )
        set_target_properties(
                ${TESTNAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY
                ${CMAKE_BINARY_DIR}/cxx_testcases)

        add_test(
                NAME TEST_${TESTNAME}
                COMMAND ${CMAKE_BINARY_DIR}/cxx_testcases/${TESTNAME}
                WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/testing)
    endforeach ()
