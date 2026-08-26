cmake_minimum_required(VERSION 3.24)

project(agnes_benchmarks_cuda VERSION 1.0.0 LANGUAGES CXX CUDA)

set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "" FORCE)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CUDA_STANDARD 20)
set(CMAKE_CUDA_STANDARD_REQUIRED ON)

set(CMAKE_CUDA_ARCHITECTURES native)

find_package(CUDAToolkit REQUIRED)

include(FetchContent)

set(NVBench_ENABLE_EXAMPLES OFF CACHE BOOL "" FORCE)
set(NVBench_ENABLE_TESTING  OFF CACHE BOOL "" FORCE)

FetchContent_Declare(
  nvbench
  GIT_REPOSITORY https://github.com/NVIDIA/nvbench
  GIT_TAG        main
  SYSTEM)

FetchContent_MakeAvailable(nvbench)

add_subdirectory(../common agnes_benchmarks_common)

add_executable(agnes_benchmarks_cuda
  $<$<CXX_COMPILER_ID:MSVC>:../../../extras/agnes.natvis>
  src/agnes_benchmarks/cuda/euler_particles_agnes_device_vector.cu
  src/agnes_benchmarks/cuda/euler_particles_device_vector_of_structs.cu
  src/agnes_benchmarks/cuda/euler_particles_kernel.cu
  src/agnes_benchmarks/cuda/euler_particles_round_trip_copy_agnes_vector.cu
  src/agnes_benchmarks/cuda/euler_particles_round_trip_copy_agnes_vector2.cu
  src/agnes_benchmarks/cuda/euler_particles_round_trip_copy_struct_of_vectors.cu
  src/agnes_benchmarks/cuda/euler_particles_round_trip_copy_vector_of_structs.cu
  src/agnes_benchmarks/cuda/euler_particles_struct_of_device_vectors.cu
  src/agnes_benchmarks/cuda/particle_struct_of_device_vectors.cu
  src/agnes_benchmarks_cuda.cu)

target_sources(agnes_benchmarks_cuda PRIVATE
  FILE_SET HEADERS
    BASE_DIRS src
    FILES
      src/agnes_benchmarks/cuda/agnes_device_allocator.cuh
      src/agnes_benchmarks/cuda/agnes_particle_device_vector.cuh
      src/agnes_benchmarks/cuda/euler_particles_agnes_device_vector.cuh
      src/agnes_benchmarks/cuda/euler_particles_device_vector_of_structs.cuh
      src/agnes_benchmarks/cuda/euler_particles_kernel.cuh
      src/agnes_benchmarks/cuda/euler_particles_round_trip_copy_agnes_vector.cuh
      src/agnes_benchmarks/cuda/euler_particles_round_trip_copy_agnes_vector2.cuh
      src/agnes_benchmarks/cuda/euler_particles_round_trip_copy_struct_of_vectors.cuh
      src/agnes_benchmarks/cuda/euler_particles_round_trip_copy_vector_of_structs.cuh
      src/agnes_benchmarks/cuda/euler_particles_struct_of_device_vectors.cuh
      src/agnes_benchmarks/cuda/particle_struct_of_device_vectors.cuh)

target_compile_options(agnes_benchmarks_cuda PRIVATE
  $<$<AND:$<COMPILE_LANGUAGE:CUDA>,$<CXX_COMPILER_ID:MSVC>>:-Xcompiler=/wd4668>)

target_link_libraries(agnes_benchmarks_cuda PRIVATE nvbench::nvbench)
target_link_libraries(agnes_benchmarks_cuda PRIVATE nvbench::main)
target_link_libraries(agnes_benchmarks_cuda PRIVATE agnes)
target_link_libraries(agnes_benchmarks_cuda PRIVATE agnes_benchmarks_common)

add_custom_command(TARGET agnes_benchmarks_cuda POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_if_different
    $<TARGET_RUNTIME_DLLS:agnes_benchmarks_cuda>
    $<TARGET_FILE_DIR:agnes_benchmarks_cuda>
  COMMAND_EXPAND_LISTS)
