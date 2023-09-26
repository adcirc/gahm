
find_package(SWIG REQUIRED)
find_package(Python3 COMPONENTS Interpreter Development REQUIRED)

if(SWIG_FOUND AND Python3_FOUND)

    execute_process(
            COMMAND ${Python3_EXECUTABLE} -m site --user-site
            OUTPUT_VARIABLE temp_PYTHON_SITE_DIRECTORY
            OUTPUT_STRIP_TRAILING_WHITESPACE)
    set(Python3_INSTALL_DIRECTORY
            ${temp_PYTHON_SITE_DIRECTORY}
            CACHE STRING "Location where Python libraries will be installed")

    add_custom_command(
            OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/python_gahm_wrap.cxx
            COMMAND
            ${SWIG_EXECUTABLE} -outdir ${CMAKE_CURRENT_BINARY_DIR} -c++ -python
            -I${CMAKE_CURRENT_SOURCE_DIR}/..
            -I${Python3_INCLUDE_DIR}
            -o ${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/python_gahm_wrap.cxx
            ${CMAKE_CURRENT_SOURCE_DIR}/gahm.i
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
            COMMENT "Generating SWIG interface to Python...")
    
    add_library(pygahm SHARED ${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/python_gahm_wrap.cxx)
    target_include_directories(pygahm PRIVATE ${Python3_INCLUDE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/..)
    target_link_libraries(pygahm gahm_static)

    if(APPLE)
        target_link_libraries(pygahm Python3::Module)
    endif(APPLE)

    set_target_properties(pygahm PROPERTIES PREFIX "_")
    set_target_properties(pygahm PROPERTIES INSTALL_NAME_DIR "gahm")
    set_property(
            DIRECTORY
            APPEND
            PROPERTY ADDITIONAL_MAKE_CLEAN_FILES pygahm.py
            CMakeFiles/python_gahm_wrap.cxx)

    add_dependencies(pygahm gahm)
    
    set_target_properties(pygahm PROPERTIES LIBRARY_OUTPUT_DIRECTORY
            ${CMAKE_CURRENT_BINARY_DIR})
    install(TARGETS pygahm LIBRARY DESTINATION ${Python3_INSTALL_DIRECTORY})
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/pygahm.py
            DESTINATION ${Python3_INSTALL_DIRECTORY})

    if(APPLE)
        set_target_properties(pygahm PROPERTIES SUFFIX ".so")
    endif()
endif()
# ##############################################################################