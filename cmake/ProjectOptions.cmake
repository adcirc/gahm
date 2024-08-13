include(cmake/SystemLink.cmake)
include(cmake/LibFuzzer.cmake)
include(CMakeDependentOption)
include(CheckCXXCompilerFlag)

macro(gahm_supports_sanitizers)

  # If we are on Darwin arm64, we can't use sanitizers
  if(CMAKE_SYSTEM_NAME STREQUAL "Darwin" AND CMAKE_SYSTEM_PROCESSOR STREQUAL "arm64")
    set(SUPPORTS_ASAN OFF)
    set(SUPPORTS_UBSAN OFF)
  else()
    if((CMAKE_CXX_COMPILER_ID MATCHES ".*Clang.*" OR CMAKE_CXX_COMPILER_ID MATCHES ".*GNU.*") AND NOT WIN32)
      set(SUPPORTS_UBSAN ON)
    else()
      set(SUPPORTS_UBSAN OFF)
    endif()

    if((CMAKE_CXX_COMPILER_ID MATCHES ".*Clang.*" OR CMAKE_CXX_COMPILER_ID MATCHES ".*GNU.*") AND WIN32)
      set(SUPPORTS_ASAN OFF)
    else()
      set(SUPPORTS_ASAN ON)
    endif()
  endif()
endmacro()

macro(gahm_setup_options)

  gahm_supports_sanitizers()

  option(gahm_ENABLE_COVERAGE "Enable coverage reporting" OFF)

  if(NOT PROJECT_IS_TOP_LEVEL OR NOT gahm_MAINTAINER_MODE)
    option(gahm_MAINTAINER_MODE "Enable maintainer mode" OFF)
    option(gahm_ENABLE_HARDENING "Enable hardening" OFF)
    cmake_dependent_option(
      gahm_ENABLE_GLOBAL_HARDENING
      "Attempt to push hardening options to built dependencies"
      OFF
      gahm_ENABLE_HARDENING
      OFF)
    option(gahm_ENABLE_IPO "Enable IPO/LTO" OFF)
    option(gahm_WARNINGS_AS_ERRORS "Treat Warnings As Errors" OFF)
    option(gahm_ENABLE_USER_LINKER "Enable user-selected linker" OFF)
    option(gahm_ENABLE_SANITIZER_ADDRESS "Enable address sanitizer" OFF)
    option(gahm_ENABLE_SANITIZER_LEAK "Enable leak sanitizer" OFF)
    option(gahm_ENABLE_SANITIZER_UNDEFINED "Enable undefined sanitizer" OFF)
    option(gahm_ENABLE_SANITIZER_THREAD "Enable thread sanitizer" OFF)
    option(gahm_ENABLE_SANITIZER_MEMORY "Enable memory sanitizer" OFF)
    option(gahm_ENABLE_UNITY_BUILD "Enable unity builds" OFF)
    #        option(gahm_ENABLE_CLANG_TIDY "Enable clang-tidy" OFF)
    option(gahm_ENABLE_CPPCHECK "Enable cpp-check analysis" OFF)
    option(gahm_ENABLE_PCH "Enable precompiled headers" OFF)
    option(gahm_ENABLE_CACHE "Enable ccache" OFF)
  else()
    option(gahm_MAINTAINER_MODE "Enable maintainer mode" ON)
    option(gahm_ENABLE_HARDENING "Enable hardening" ON)
    cmake_dependent_option(
      gahm_ENABLE_GLOBAL_HARDENING
      "Attempt to push hardening options to built dependencies"
      ON
      gahm_ENABLE_HARDENING
      OFF)
    option(gahm_ENABLE_IPO "Enable IPO/LTO" OFF)
    option(gahm_WARNINGS_AS_ERRORS "Treat Warnings As Errors" ON)
    option(gahm_ENABLE_USER_LINKER "Enable user-selected linker" OFF)
    option(gahm_ENABLE_SANITIZER_ADDRESS "Enable address sanitizer" ${SUPPORTS_ASAN})
    option(gahm_ENABLE_SANITIZER_LEAK "Enable leak sanitizer" OFF)
    option(gahm_ENABLE_SANITIZER_UNDEFINED "Enable undefined sanitizer" ${SUPPORTS_UBSAN})
    option(gahm_ENABLE_SANITIZER_THREAD "Enable thread sanitizer" OFF)
    option(gahm_ENABLE_SANITIZER_MEMORY "Enable memory sanitizer" OFF)
    option(gahm_ENABLE_UNITY_BUILD "Enable unity builds" OFF)
    #        option(gahm_ENABLE_CLANG_TIDY "Enable clang-tidy" ON)
    option(gahm_ENABLE_CPPCHECK "Enable cpp-check analysis" ON)
    option(gahm_ENABLE_PCH "Enable precompiled headers" OFF)
    option(gahm_ENABLE_CACHE "Enable ccache" ON)
  endif()

  if(NOT PROJECT_IS_TOP_LEVEL OR NOT gahm_MAINTAINER_MODE)
    mark_as_advanced(
      gahm_MAINTAINER_MODE
      gahm_ENABLE_HARDENING
      gahm_ENABLE_GLOBAL_HARDENING
      gahm_ENABLE_IPO
      gahm_WARNINGS_AS_ERRORS
      gahm_ENABLE_USER_LINKER
      gahm_ENABLE_SANITIZER_ADDRESS
      gahm_ENABLE_SANITIZER_LEAK
      gahm_ENABLE_SANITIZER_UNDEFINED
      gahm_ENABLE_SANITIZER_THREAD
      gahm_ENABLE_SANITIZER_MEMORY
      gahm_ENABLE_UNITY_BUILD
      #                gahm_ENABLE_CLANG_TIDY
      gahm_ENABLE_CPPCHECK
      gahm_ENABLE_COVERAGE
      gahm_ENABLE_PCH
      gahm_ENABLE_CACHE)
  endif()

  if(gahm_MAINTAINER_MODE)
    gahm_check_libfuzzer_support(LIBFUZZER_SUPPORTED)
    if(LIBFUZZER_SUPPORTED
       AND (gahm_ENABLE_SANITIZER_ADDRESS
            OR gahm_ENABLE_SANITIZER_THREAD
            OR gahm_ENABLE_SANITIZER_UNDEFINED))
      set(DEFAULT_FUZZER ON)
    else()
      set(DEFAULT_FUZZER OFF)
    endif()

    option(gahm_BUILD_FUZZ_TESTS "Enable fuzz testing executable" ${DEFAULT_FUZZER})
  endif()

endmacro()

macro(gahm_global_options)
  if(gahm_ENABLE_IPO)
    include(cmake/InterproceduralOptimization.cmake)
    gahm_enable_ipo()
  endif()

  gahm_supports_sanitizers()

  if(gahm_ENABLE_HARDENING AND gahm_ENABLE_GLOBAL_HARDENING)
    include(cmake/Hardening.cmake)
    if(NOT SUPPORTS_UBSAN
       OR gahm_ENABLE_SANITIZER_UNDEFINED
       OR gahm_ENABLE_SANITIZER_ADDRESS
       OR gahm_ENABLE_SANITIZER_THREAD
       OR gahm_ENABLE_SANITIZER_LEAK)
      set(ENABLE_UBSAN_MINIMAL_RUNTIME FALSE)
    else()
      set(ENABLE_UBSAN_MINIMAL_RUNTIME TRUE)
    endif()
    #        message("${gahm_ENABLE_HARDENING} ${ENABLE_UBSAN_MINIMAL_RUNTIME} ${gahm_ENABLE_SANITIZER_UNDEFINED}")
    gahm_enable_hardening(gahm_options ON ${ENABLE_UBSAN_MINIMAL_RUNTIME})
  endif()
endmacro()

macro(gahm_local_options)
  if(PROJECT_IS_TOP_LEVEL)
    include(cmake/StandardProjectSettings.cmake)
  endif()

  add_library(gahm_warnings INTERFACE)
  add_library(gahm_options INTERFACE)

  include(cmake/CompilerWarnings.cmake)
  gahm_set_project_warnings(
    gahm_warnings
    ${gahm_WARNINGS_AS_ERRORS}
    ""
    ""
    ""
    "")

  if(gahm_ENABLE_USER_LINKER)
    include(cmake/Linker.cmake)
    gahm_configure_linker(gahm_options)
  endif()

  include(cmake/Sanitizers.cmake)
  gahm_enable_sanitizers(
    gahm_options
    ${gahm_ENABLE_SANITIZER_ADDRESS}
    ${gahm_ENABLE_SANITIZER_LEAK}
    ${gahm_ENABLE_SANITIZER_UNDEFINED}
    ${gahm_ENABLE_SANITIZER_THREAD}
    ${gahm_ENABLE_SANITIZER_MEMORY})

  set_target_properties(gahm_options PROPERTIES UNITY_BUILD ${gahm_ENABLE_UNITY_BUILD})

  if(gahm_ENABLE_PCH)
    target_precompile_headers(
      gahm_options
      INTERFACE
      <vector>
      <string>
      <utility>)
  endif()

  if(gahm_ENABLE_CACHE)
    include(cmake/Cache.cmake)
    gahm_enable_cache()
  endif()

  include(cmake/StaticAnalyzers.cmake)
  #    if(gahm_ENABLE_CLANG_TIDY)
  #        gahm_enable_clang_tidy(gahm_options ${gahm_WARNINGS_AS_ERRORS})
  #    endif()

  if(gahm_ENABLE_CPPCHECK)
    gahm_enable_cppcheck(${gahm_WARNINGS_AS_ERRORS} "" # override cppcheck options
    )
  endif()

  if(gahm_ENABLE_COVERAGE)
    include(cmake/Tests.cmake)
    gahm_enable_coverage(gahm_options)
  endif()

  if(gahm_WARNINGS_AS_ERRORS)
    check_cxx_compiler_flag("-Wl,--fatal-warnings" LINKER_FATAL_WARNINGS)
    if(LINKER_FATAL_WARNINGS)
      # This is not working consistently, so disabling for now
      # target_link_options(gahm_options INTERFACE -Wl,--fatal-warnings)
    endif()
  endif()

  if(gahm_ENABLE_HARDENING AND NOT gahm_ENABLE_GLOBAL_HARDENING)
    include(cmake/Hardening.cmake)
    if(NOT SUPPORTS_UBSAN
       OR gahm_ENABLE_SANITIZER_UNDEFINED
       OR gahm_ENABLE_SANITIZER_ADDRESS
       OR gahm_ENABLE_SANITIZER_THREAD
       OR gahm_ENABLE_SANITIZER_LEAK)
      set(ENABLE_UBSAN_MINIMAL_RUNTIME FALSE)
    else()
      set(ENABLE_UBSAN_MINIMAL_RUNTIME TRUE)
    endif()
    gahm_enable_hardening(gahm_options OFF ${ENABLE_UBSAN_MINIMAL_RUNTIME})
  endif()

endmacro()
