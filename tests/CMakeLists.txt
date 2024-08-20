# ##############################################################################
# Testing
# ##############################################################################

if(NOT DEFINED gahm_BUILD_TESTS AND gahm_MAINTAINER_MODE)
    option(gahm_BUILD_TESTS "Build all tests." ON)
else()
    option(gahm_BUILD_TESTS "Build all tests." OFF)
endif()

if(gahm_BUILD_TESTS)
    enable_testing()
    add_subdirectory(cxx)
endif()