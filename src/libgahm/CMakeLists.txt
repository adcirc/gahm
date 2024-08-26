include(${CMAKE_CURRENT_SOURCE_DIR}/../../cmake/SystemLink.cmake)


set(GAHM_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/datatypes/Datetime.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/datatypes/Vec.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/datatypes/Point.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/datatypes/RotationMatrix.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/util/ioutils.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/atcf/AtcfPeriod.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/atcf/AtcfTrack.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/atcf/AtcfIO.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/storm/Isotach.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/storm/Quadrant.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/storm/StormTranslation.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/gahm/GahmParameters.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/gahm/GahmEquations.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/gahm/GahmRadiusSolver.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/gahm/GahmRadiusSolverPrivate.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/gahm/GahmSolver.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/gahm/GahmSolution.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/output/RadialProfile.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/output/PointOutput.cpp
)

if(gahm_ENABLE_PLOTTING)
    list(APPEND GAHM_SOURCES
            ${CMAKE_CURRENT_SOURCE_DIR}/plotting/AtcfPeriodPlot.cpp
    )
endif()

add_library(gahm_objects OBJECT ${GAHM_SOURCES})
target_include_directories(gahm_objects PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(gahm_objects PUBLIC gahm::gahm_warnings gahm::gahm_options)
target_link_system_libraries(gahm_objects PUBLIC Boost::boost)
target_compile_features(gahm_objects PUBLIC cxx_std_20)

if(gahm_ENABLE_PLOTTING)
    target_link_system_libraries(gahm_objects PUBLIC ${VTK_LIBRARIES})
    vtk_module_autoinit(
            TARGETS gahm_objects
            MODULES ${VTK_LIBRARIES}
    )
endif()

if(gahm_ENABLE_SHARED)
    add_library(gahm SHARED $<TARGET_OBJECTS:gahm_objects>)
    if(gahm_ENABLE_PLOTTING)
        target_link_system_libraries(gahm PUBLIC ${VTK_LIBRARIES})
        vtk_module_autoinit(
                TARGETS gahm
                MODULES ${VTK_LIBRARIES}
        )
    endif()
endif()

if(gahm_ENABLE_STATIC)
    add_library(gahm_static STATIC $<TARGET_OBJECTS:gahm_objects>)
    set_target_properties(gahm_static PROPERTIES OUTPUT_NAME gahm)
    if(gahm_ENABLE_PLOTTING)
        target_link_system_libraries(gahm_static PUBLIC ${VTK_LIBRARIES})
        vtk_module_autoinit(
                TARGETS gahm_static
                MODULES ${VTK_LIBRARIES}
        )
    endif()
endif()



