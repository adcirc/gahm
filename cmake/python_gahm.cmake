# ##############################################################################
# Python GAHM Library
# ##############################################################################
if(SWIG_FOUND
   AND Python3_FOUND)

  set(PYTHONFLAG "-py3")

  if(WIN32)

    # ...Builds a dummy cmake project Which will add some windows specific
    # include files not required by Unix/Cygwin [mingw64] Further testing
    # required to see if this is required by MSVC compilers
    file(
      WRITE ${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/python_wrapper.cmake
      "
            FILE(READ \${SRC} DATA)
            FILE(WRITE \${DST} \"#include <math.h>\")
            FILE(APPEND \${DST} \"\\n\\n\")
            FILE(APPEND \${DST} \"\${DATA}\")
        ")

    add_custom_command(
      OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/python_gahm_wrap.cxx
      COMMAND
        ${SWIG_EXECUTABLE} -outdir ${CMAKE_CURRENT_BINARY_DIR} -c++ -python
        ${PYTHONFLAG} -I${CMAKE_CURRENT_SOURCE_DIR}/src
        -I${Python3_INCLUDE_DIRS}
        ${SWIG_GDAL_FLAG} -o
        ${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/python_gahm_wrap.cxx.1
        ${CMAKE_CURRENT_SOURCE_DIR}/swig/gahm.i
      COMMAND
        ${CMAKE_COMMAND}
        -DSRC=\"${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/python_gahm_wrap.cxx.1\"
        -DDST=\"${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/python_gahm_wrap.cxx\" -P
        ${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/python_wrapper.cmake
      WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
      COMMENT "Generating SWIG interface to Python...")

  else(WIN32)
    add_custom_command(
      OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/python_gahm_wrap.cxx
      COMMAND
        ${SWIG_EXECUTABLE} -outdir ${CMAKE_CURRENT_BINARY_DIR} -c++ -python
        ${PYTHONFLAG} -I${CMAKE_CURRENT_SOURCE_DIR}/src
        -I${PYTHON_INCLUDE_PATH}
        ${SWIG_GDAL_FLAG} -o
        ${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/python_gahm_wrap.cxx
        ${CMAKE_CURRENT_SOURCE_DIR}/swig/gahm.i
      WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
      COMMENT "Generating SWIG interface to Python...")
  endif(WIN32)

  add_library(pygahm SHARED
              ${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/python_gahm_wrap.cxx)
  target_include_directories(
    pygahm PRIVATE ${Python3_INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR}/src)
  target_link_libraries(pygahm gahm )
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

  if(WIN32)
    set_target_properties(
      pygahm PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
                          RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
    install(
      TARGETS pygahm
      LIBRARY DESTINATION bin
      RUNTIME DESTINATION ${PYTHON_INSTALL_DIRECTORY})
  elseif(CYGWIN)
    set_target_properties(pygahm PROPERTIES LIBRARY_OUTPUT_DIRECTORY
                                              ${CMAKE_CURRENT_BINARY_DIR})
    install(
      TARGETS pygahm
      LIBRARY DESTINATION lib
      RUNTIME DESTINATION ${PYTHON_INSTALL_DIRECTORY})
  else(WIN32)
    set_target_properties(pygahm PROPERTIES LIBRARY_OUTPUT_DIRECTORY
                                              ${CMAKE_CURRENT_BINARY_DIR})
    install(TARGETS pygahm LIBRARY DESTINATION ${PYTHON_INSTALL_DIRECTORY})
  endif(WIN32)

  install(FILES ${CMAKE_CURRENT_BINARY_DIR}/pygahm.py
          DESTINATION ${PYTHON_INSTALL_DIRECTORY})

  if(WIN32)
    set_target_properties(pygahm PROPERTIES SUFFIX ".pyd")
    if(CMAKE_SIZEOF_VOID_P EQUAL 8)
      # 64 bits
      target_compile_definitions(pygahm PRIVATE "MS_WIN64")
    elseif(CMAKE_SIZEOF_VOID_P EQUAL 4)
      # 32 bits
      target_compile_definitions(pygahm PRIVATE "MS_WIN32")
    endif()
  elseif(APPLE)
    set_target_properties(pygahm PROPERTIES SUFFIX ".so")
  endif(WIN32)
endif()
# ##############################################################################
