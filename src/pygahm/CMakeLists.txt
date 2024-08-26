
add_custom_command(
        OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/python_gahm_wrap.cxx
        COMMAND
        ${SWIG_EXECUTABLE} -outdir ${CMAKE_CURRENT_BINARY_DIR} -c++ -python
        -I${CMAKE_CURRENT_SOURCE_DIR}/../libgahm -I${Python3_INCLUDE_DIRS} -o
        ${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/python_gahm_wrap.cxx
        ${CMAKE_CURRENT_SOURCE_DIR}/pygahm.i
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/pygahm.i
        COMMENT "Generating SWIG interface to Python..."
)

add_library(pygahm SHARED ${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/python_gahm_wrap.cxx)
target_include_directories(pygahm PRIVATE ${Python3_INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR}/../libgahm)
target_link_libraries(pygahm PRIVATE gahm_objects)
target_link_libraries(gahm_objects PUBLIC gahm::gahm_options)
add_dependencies(pygahm gahm_objects)

# Disable clang-tidy on the generated file
set_target_properties(pygahm PROPERTIES CXX_CPPCHECK "" CXX_CLANG_TIDY "")

# Disable warnings on the generated file
target_compile_options(pygahm PRIVATE -w)

# Set the library name to _pygahm.so
set_target_properties(pygahm PROPERTIES PREFIX "_")
set_target_properties(pygahm PROPERTIES INSTALL_NAME_DIR "gahm")
set_property(
        DIRECTORY
        APPEND
        PROPERTY ADDITIONAL_MAKE_CLEAN_FILES pygahm.py
        ${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/python_gahm_wrap.cxx
)

# Options used when building the Python package for distribution
if(gahm_PYTHON_PACKAGE_BUILD)
    set_target_properties(pygahm PROPERTIES LIBRARY_OUTPUT_DIRECTORY
            ${CMAKE_INSTALL_PREFIX}
    )
    add_custom_command(
            TARGET pygahm
            POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_BINARY_DIR}/pygahm.py
            ${CMAKE_INSTALL_PREFIX}/pygahm.py
    )
else()
    install(TARGETS pygahm LIBRARY DESTINATION ${Python3_SITELIB}/pygahm)
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/pygahm.py DESTINATION ${Python3_SITELIB}/pygahm)
    install(
            FILES ${CMAKE_CURRENT_SOURCE_DIR}/init_py.default
            DESTINATION ${Python3_SITELIB}/pygahm
            RENAME __init__.py
    )
endif()

#

# Mac specific settings
if(APPLE)
    target_link_libraries(pygahm PRIVATE Python3::Module)
    set_target_properties(pygahm PROPERTIES SUFFIX ".so")
endif(APPLE)