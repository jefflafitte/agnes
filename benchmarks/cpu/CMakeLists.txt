cmake_minimum_required(VERSION 3.24)

project(agnes_benchmarks_cpu VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "" FORCE)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_COMPILE_WARNING_AS_ERROR ON)

include(FetchContent)

set(BENCHMARK_ENABLE_TESTING OFF CACHE BOOL "" FORCE)
set(BENCHMARK_ENABLE_INSTALL OFF CACHE BOOL "" FORCE)
set(BENCHMARK_ENABLE_WERROR OFF CACHE BOOL "" FORCE)

FetchContent_Declare(
  googlebenchmark
  GIT_REPOSITORY https://github.com/google/benchmark.git
  GIT_TAG        v1.9.5)

FetchContent_MakeAvailable(googlebenchmark)

add_subdirectory(../common agnes_benchmarks_common)

function(add_benchmark_executable name no_vectorization avoid_cache_conflicts)
  add_executable(${name}
    $<$<CXX_COMPILER_ID:MSVC>:../../extras/agnes.natvis>
    src/agnes_benchmarks/cpu/container_fixture.h
    src/agnes_benchmarks/cpu/euler_agnes_table.h
    src/agnes_benchmarks/cpu/euler_agnes_vector.h
    src/agnes_benchmarks/cpu/euler_fixture.h
    src/agnes_benchmarks/cpu/euler_struct_of_arrays.h
    src/agnes_benchmarks/cpu/euler_struct_of_vectors.h
    src/agnes_benchmarks/cpu/euler_vector_of_structs.h
    src/agnes_benchmarks/cpu/row_sum_agnes_vector.h
    src/agnes_benchmarks/cpu/row_sum_fixture.h
    src/agnes_benchmarks/cpu/row_sum_struct_of_arrays.h
    src/agnes_benchmarks/cpu/row_sum_struct_of_vectors.h
    src/agnes_benchmarks/cpu/row_sum_vector_of_structs.h
    src/agnes_benchmarks/cpu/vectorization.h
    src/agnes_benchmarks_cpu.cpp)

  target_include_directories(${name} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)

  target_compile_definitions(${name} PRIVATE
    AGNES_NO_VECTORIZATION=${no_vectorization}
    AGNES_AVOID_CACHE_CONFLICTS=${avoid_cache_conflicts})

  if(MSVC)
    target_compile_options(${name} PRIVATE $<$<CONFIG:Release>:/arch:AVX2 /fp:fast /O2> /Zc:preprocessor)
  else()
    target_compile_options(${name} PRIVATE $<$<CONFIG:Release>:-ffast-math -march=x86-64-v3 -O3>)
    if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
      target_compile_options(${name} PRIVATE $<$<CONFIG:Release>:--param vect-max-version-for-alias-checks=36>)
    endif()
  endif()

  target_link_libraries(${name} PRIVATE benchmark::benchmark)
  target_link_libraries(${name} PRIVATE agnes)
  target_link_libraries(${name} PRIVATE agnes_benchmarks_common)
endfunction()

add_benchmark_executable(agnes_benchmarks_cpu                                        0 0)
add_benchmark_executable(agnes_benchmarks_cpu_no_vectorization                       1 0)
add_benchmark_executable(agnes_benchmarks_cpu_no_vectorization_avoid_cache_conflicts 1 1)
add_benchmark_executable(agnes_benchmarks_cpu_avoid_cache_conflicts                  0 1)
