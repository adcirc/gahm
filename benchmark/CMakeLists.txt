find_package(benchmark REQUIRED)
add_executable(gahm_benchmark gahm_benchmark.cpp)
target_include_directories(gahm_benchmark
                           PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../src)
target_link_libraries(gahm_benchmark gahm benchmark::benchmark)
add_dependencies(gahm_benchmark gahm_static)
