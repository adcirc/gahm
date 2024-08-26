
function(mark_as_advanced_wildcard variable_prefix)
  get_cmake_property(CACHE_VARS VARIABLES)
  foreach(CACHE_VAR ${CACHE_VARS})
    if(CACHE_VAR MATCHES "^${variable_prefix}")
      mark_as_advanced(${CACHE_VAR})
    endif()
  endforeach()
endfunction()

macro(gahm_setup_dependencies)

  include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/CPM.cmake)

  # Mark the CPM variables as advanced
  mark_as_advanced_wildcard("CPM_")

  # ##################################################################################################################
  # Boost @ 1.70.0
  # ##################################################################################################################
  find_package(Boost 1.70.0 CONFIG REQUIRED)
  mark_as_advanced(Boost_DIR)


  # ##################################################################################################################
  # fmt
  # ##################################################################################################################
    CPMAddPackage(
            NAME fmt
            GITHUB_REPOSITORY fmtlib/fmt
            GIT_TAG "11.0.2"
            EXCLUDE_FROM_ALL
    )
    mark_as_advanced_wildcard("FMT_")
    set_target_properties(fmt PROPERTIES CXX_CPPCHECK "" CXX_CLANG_TIDY "")

  # ##################################################################################################################
  # SWIG
  # ##################################################################################################################
  if(gahm_ENABLE_PYTHON)
    find_package(SWIG 4 REQUIRED)

    # We also need the python interpreter and headers
    find_package(Python3 COMPONENTS Interpreter Development.Module REQUIRED)

  endif()


  # ##################################################################################################################
  # Vtk
  # ##################################################################################################################
  if(gahm_ENABLE_PLOTTING)
    find_package(VTK REQUIRED ChartsCore
            CommonColor
            CommonCore
            CommonDataModel
            InteractionStyle
            RenderingContextOpenGL2
            RenderingCore
            RenderingFreeType
            RenderingGL2PSOpenGL2
            RenderingOpenGL2
            ViewsContext2D
            IOImage
            QUIET
    )

    if(NOT VTK_FOUND)
      message(FATAL_ERROR "VTK was not found. Please set VTK_DIR.")
    endif()

    mark_as_advanced(VTK_DIR)
    mark_as_advanced(Tiff_DIR)
    mark_as_advanced(LZMA_LIBRARY)
    mark_as_advanced(LZMA_INCLUDE_DIR)

  endif()

  # ##################################################################################################################
  # Catch2
  # ##################################################################################################################
  if(gahm_BUILD_TESTS)
    CPMAddPackage(
            NAME Catch2
            GITHUB_REPOSITORY catchorg/Catch2
            VERSION 3.7.0
            EXCLUDE_FROM_ALL
    )
    mark_as_advanced_wildcard("CATCH_")

    # Disable cppcheck and clang-tidy on Catch2
    set_target_properties(Catch2 PROPERTIES CXX_CPPCHECK "" CXX_CLANG_TIDY "")
    set_target_properties(Catch2WithMain PROPERTIES CXX_CPPCHECK "" CXX_CLANG_TIDY "")

  endif()

  # ##################################################################################################################
  # Google Benchmark
  # ##################################################################################################################
  if(gahm_ENABLE_BENCHMARKS)
    set(BENCHMARK_DOWNLOAD_DEPENDENCIES ON)
    set(BENCHMARK_ENABLE_TESTING OFF)
    set(BENCHMARK_ENABLE_GTEST_TESTS OFF)
    set(BENCHMARK_INSTALL_DOCS OFF)
    set(BENCHMARK_ENABLE_INSTALL OFF)
    CPMAddPackage(
            NAME benchmark
            GITHUB_REPOSITORY google/benchmark
            GIT_TAG v1.9.0
            EXCLUDE_FROM_ALL
    )
    mark_as_advanced_wildcard("BENCHMARK_")
    mark_as_advanced(GOOGLETEST_PATH)
    mark_as_advanced(CXXFEATURECHECK_DEBUG)
    mark_as_advanced(LLVM_FILECHECK_EXE)

    # Disable cppcheck and clang-tidy on Google Benchmark
    set_target_properties(benchmark PROPERTIES CXX_CPPCHECK "" CXX_CLANG_TIDY "")
    set_target_properties(benchmark_main PROPERTIES CXX_CPPCHECK "" CXX_CLANG_TIDY "")
  endif()

  # Mark the FetchContent variables as advanced
  mark_as_advanced_wildcard("FETCHCONTENT_")
  mark_as_advanced("pugixml_DIR")

endmacro()
