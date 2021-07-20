if(GAHM_ENABLE_BENCHMARKING)
  set(GAHM_GOOGLE_BENCHMARK_HOME
      "GOOGLE_BENCHMARK_NOT_FOUND"
      CACHE STRING "Google Benchmark Library home directory")

  if(NOT $ENV{GOOGLE_BENCHMARK_HOME} STREQUAL "")
    set(GAHM_GOOGLE_BENCHMARK_HOME ${ENV{GOOGLE_BENCHMARK_HOME})
  endif()

  if(${GAHM_GOOGLE_BENCHMARK_HOME} STREQUAL "GOOGLE_BENCHMARK_NOT_FOUND")
    message(
      SEND_ERROR "Google Benchmark enabled but library home location not set")
  endif()

  find_package(Threads REQUIRED)

  add_executable(gahm_benchmark
                 ${CMAKE_CURRENT_SOURCE_DIR}/testing/cxx/cxx_benchmark.cpp)
  target_link_libraries(gahm_benchmark PRIVATE gahm benchmark
                                               ${CMAKE_THREAD_LIBS_INIT})
  target_include_directories(gahm_benchmark
                             PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)
  target_include_directories(gahm_benchmark
                             PRIVATE ${GAHM_GOOGLE_BENCHMARK_HOME}/include)
  target_link_directories(gahm_benchmark PRIVATE
                          ${GAHM_GOOGLE_BENCHMARK_HOME}/lib)
endif()
