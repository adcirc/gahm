
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
  # Boost @ 1.86.0
  # ##################################################################################################################
  set(TRY_BOOST_VERSION "1.86.0")
  set(BOOST_INCLUDE_LIBRARIES "math;algorithm;date_time")
  set(BOOST_URL
          "https://github.com/boostorg/boost/releases/download/boost-${TRY_BOOST_VERSION}/boost-${TRY_BOOST_VERSION}-cmake.tar.xz"
  )
  CPMAddPackage(
          NAME Boost
          VERSION ${TRY_BOOST_VERSION}
          URL ${BOOST_URL}
          OPTIONS "BOOST_SKIP_INSTALL_RULES OFF"
  )

  # Mark the BOOST_* variables as advanced
  mark_as_advanced_wildcard("BOOST_")

  # Do not use CPPCHECK on Boost
  set_target_properties(boost_container PROPERTIES CXX_CPPCHECK "" CXX_CLANG_TIDY "")
  set_target_properties(boost_random PROPERTIES CXX_CPPCHECK "" CXX_CLANG_TIDY "")
  set_target_properties(boost_date_time PROPERTIES CXX_CPPCHECK "" CXX_CLANG_TIDY "")
  set_target_properties(boost_algorithm PROPERTIES CXX_CPPCHECK "" CXX_CLANG_TIDY "")
  set_target_properties(boost_math PROPERTIES CXX_CPPCHECK "" CXX_CLANG_TIDY "")

  # ##################################################################################################################
  # Quill @ 6.1.2
  # ##################################################################################################################
  CPMAddPackage(
          NAME Quill
          VERSION 6.1.2
          GITHUB_REPOSITORY odygrd/quill
          GIT_TAG v6.1.2
  )
  mark_as_advanced_wildcard("QUILL_")

  # ##################################################################################################################
  # Matplot++ @ 1.2.1
  # ##################################################################################################################
  if(GAHM_ENABLE_PLOTTING)
    CPMAddPackage(
            NAME matplotplusplus
            VERSION 1.2.1
            GITHUB_REPOSITORY alandefreitas/matplotplusplus
            GIT_TAG v1.2.1
    )

    # Mark the MATPLOTPLUSPLUS_* variables as advanced
    mark_as_advanced_wildcard("MATPLOTPP_")
    mark_as_advanced_wildcard("OPENCV_")
    mark_as_advanced_wildcard("OpenCV_")
    mark_as_advanced("Tiff_DIR")
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

  # Mark the FetchContent variables as advanced
  mark_as_advanced_wildcard("FETCHCONTENT_")

endmacro()
