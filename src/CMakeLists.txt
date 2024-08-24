add_subdirectory(libgahm)

if(gahm_ENABLE_PYTHON)
    add_subdirectory(pygahm)
endif()