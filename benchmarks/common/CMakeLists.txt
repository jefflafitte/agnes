cmake_minimum_required(VERSION 3.24)

project(agnes_benchmarks_common VERSION 1.0.0 LANGUAGES CXX)

add_subdirectory(../.. agnes)

add_library(agnes_benchmarks_common STATIC
  include/agnes_benchmarks/agnes_particle_table.h
  include/agnes_benchmarks/agnes_particle_vector.h
  include/agnes_benchmarks/agnes_row_table.h
  include/agnes_benchmarks/agnes_row_vector.h
  include/agnes_benchmarks/cache_configuration.h
  include/agnes_benchmarks/device.h
  include/agnes_benchmarks/euler.h
  include/agnes_benchmarks/particle.h
  include/agnes_benchmarks/particle_struct_of_arrays.h
  include/agnes_benchmarks/particle_struct_of_vectors.h
  include/agnes_benchmarks/particle_vector.h
  include/agnes_benchmarks/row.h
  include/agnes_benchmarks/row_struct_of_arrays.h
  include/agnes_benchmarks/row_struct_of_vectors.h
  include/agnes_benchmarks/row_sum.h
  include/agnes_benchmarks/row_vector.h
  src/cache_configuration.cpp)

target_include_directories(agnes_benchmarks_common PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(agnes_benchmarks_common PUBLIC agnes)
