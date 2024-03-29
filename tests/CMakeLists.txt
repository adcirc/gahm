# ##############################################################################
# Testing
# ##############################################################################
include(FetchContent)
FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG v3.3.2)
FetchContent_MakeAvailable(Catch2)
mark_as_advanced(
  CATCH_CONFIG_ANDROID_LOGWRITE
  CATCH_CONFIG_BAZEL_SUPPORT
  CATCH_CONFIG_COLOUR_WIN32
  CATCH_CONFIG_CONSOLE_WIDTH
  CATCH_CONFIG_COUNTER
  CATCH_INSTALL_DOCS
  CATCH_CONFIG_CPP11_TO_STRING
  CATCH_CONFIG_CPP17_BYTE
  CATCH_CONFIG_CPP17_OPTIONAL
  CATCH_CONFIG_CPP17_STRING_VIEWCATCH_CONFIG_CPP17_UNCAUGHT_EXCEPTIONS
  CATCH_CONFIG_CPP17_VARIANT
  CATCH_CONFIG_DEFAULT_REPORTER
  CATCH_CONFIG_DISABLE
  CATCH_CONFIG_DISABLE_EXCEPTION
  CATCH_CONFIG_DISABLE_EXCEPTIONS_CUSTOM_HANDLER
  CATCH_CONFIG_DISABLE_STRINGIFICATION
  ATCH_CONFIG_ENABLE_ALL_STRINGMAKERS
  CATCH_CONFIG_ENABLE_OPTIONAL_STRINGMAKER
  CATCH_CONFIG_ENABLE_PAIR_STRINGMAKER
  CATCH_CONFIG_CPP17_STRING_VIEW
  CATCH_CONFIG_CPP17_UNCAUGHT_EXCEPTIONS
  CATCH_CONFIG_DISABLE_EXCEPTIONS
  CATCH_CONFIG_ENABLE_ALL_STRINGMAKERS
  CATCH_CONFIG_ENABLE_TUPLE_STRINGMAKER
  CATCH_CONFIG_ENABLE_VARIANT_STRINGMAKER
  CATCH_CONFIG_EXPERIMENTAL_REDIRECT
  CATCH_CONFIG_FAST_COMPILE
  CATCH_CONFIG_GETENV
  CATCH_CONFIG_GLOBAL_NEXTAFTER
  CATCH_CONFIG_NOSTDOUT
  CATCH_CONFIG_NO_ANDROID_LOGWRITE
  CATCH_CONFIG_NO_BAZEL_SUPPORT
  CATCH_CONFIG_NO_COLOUR_WIN32
  CATCH_CONFIG_NO_COUNTER
  CATCH_CONFIG_NO_CPP11_TO_STRING
  CATCH_CONFIG_NO_CPP17_BYTE
  CATCH_CONFIG_NO_CPP17_OPTIONAL
  CATCH_CONFIG_NO_CPP17_STRING_VIEW
  CATCH_CONFIG_NO_CPP17_UNCAUGHT_EXCEPTIONS
  CATCH_CONFIG_NO_CPP17_VARIANT
  CATCH_CONFIG_NO_GETENV
  CATCH_CONFIG_NO_GLOBAL_NEXTAFTER
  CATCH_CONFIG_NO_POSIX_SIGNALS
  CATCH_CONFIG_NO_USE_ASYNC
  CATCH_CONFIG_NO_WCHAR
  CATCH_CONFIG_NO_WINDOWS_SEH
  CATCH_CONFIG_POSIX_SIGNALS
  CATCH_CONFIG_PREFIX_ALL
  CATCH_CONFIG_USE_ASYNC
  CATCH_CONFIG_WCHAR
  CATCH_CONFIG_WINDOWS_CRTDBG
  CATCH_CONFIG_WINDOWS_SEH
  CATCH_DEVELOPMENT_BUILD
  CATCH_INSTALL_EXTRAS
  FETCHCONTENT_BASE_DIR
  FETCHCONTENT_FULLY_DISCONNECTED
  FETCHCONTENT_QUIET
  FETCHCONTENT_SOURCE_DIR_CATCH2
  FETCHCONTENT_UPDATES_DISCONNECTED
  FETCHCONTENT_UPDATES_DISCONNECTED_CATCH2)

# ##############################################################################
# ...CXX TESTS
# ##############################################################################
file(GLOB_RECURSE TestList ${CMAKE_CURRENT_SOURCE_DIR}/cxx_tests/TEST_*.cpp)

find_package(OpenSSL REQUIRED)

foreach(test ${TestList})
  get_filename_component(test_name ${test} NAME_WE)
  add_executable(${test_name} ${test})
  add_test(
    NAME ${test_name}
    COMMAND ${test_name}
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

  target_include_directories(
    ${test_name}
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../src ${catch2_SOURCE_DIR}
            ${OPENSSL_INCLUDE_DIR}
            ${CMAKE_CURRENT_SOURCE_DIR}/../thirdparty/fmt-9.1.0/include)

  if(GAHM_ENABLE_COVERAGE)
    set_target_properties(
      ${test_name} PROPERTIES COMPILE_FLAGS ${GAHM_COVERAGE_COMPILE_FLAGS}
                              LINK_FLAGS ${GAHM_COVERAGE_LINK_FLAGS})
  endif()

  target_link_libraries(${test_name} PRIVATE Catch2::Catch2WithMain
                                             OpenSSL::SSL)
  set_target_properties(
    ${test_name} PROPERTIES RUNTIME_OUTPUT_DIRECTORY
                            ${CMAKE_CURRENT_BINARY_DIR}/tests)
  target_link_libraries(${test_name} PRIVATE gahm_objectlib gahm_interface)
  add_dependencies(${test_name} gahm_objectlib gahm_interface)
endforeach()
# ##############################################################################

# ##############################################################################
# ... Fortran TESTS
# ##############################################################################
if(GAHM_ENABLE_FORTRAN)
  file(GLOB_RECURSE TestList
       ${CMAKE_CURRENT_SOURCE_DIR}/fortran_tests/TEST_*.F90)
  foreach(test ${TestList})
    get_filename_component(test_name ${test} NAME_WE)
    add_executable(${test_name} ${test})
    add_test(
      NAME ${test_name}
      COMMAND ${test_name}
      WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
    target_include_directories(${test_name} PRIVATE ${CMAKE_BINARY_DIR}/fortran)

    if(GAHM_ENABLE_COVERAGE)
      set_target_properties(
        ${test_name} PROPERTIES COMPILE_FLAGS ${GAHM_COVERAGE_COMPILE_FLAGS}
                                LINK_FLAGS ${GAHM_COVERAGE_LINK_FLAGS})
    endif()

    set_target_properties(
      ${test_name} PROPERTIES RUNTIME_OUTPUT_DIRECTORY
                              ${CMAKE_CURRENT_BINARY_DIR}/tests)

    target_link_libraries(${test_name} PRIVATE gahm_objectlib gahm_fortran gahm_interface)
    add_dependencies(${test_name} gahm_objectlib gahm_fortran gahm_interface)
  endforeach()
endif()
# ##############################################################################
