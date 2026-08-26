cmake_minimum_required(VERSION 3.24)

project(agnes_tests VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CONFIGURATION_TYPES "Debug;Release;DebugConstEval;ReleaseConstEval" CACHE STRING "" FORCE)

set(CMAKE_CXX_FLAGS_DEBUGCONSTEVAL "${CMAKE_CXX_FLAGS_DEBUG} -DAGNES_CONST_EVAL_TESTS=1" CACHE STRING "" FORCE)
set(CMAKE_CXX_FLAGS_RELEASECONSTEVAL "${CMAKE_CXX_FLAGS_RELEASE} -DAGNES_CONST_EVAL_TESTS=1" CACHE STRING "" FORCE)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_COMPILE_WARNING_AS_ERROR ON)

if(MSVC)
  add_compile_options(/Zc:preprocessor)
endif()

include(FetchContent)

FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG        v3.8.1)

FetchContent_MakeAvailable(Catch2)

add_subdirectory(.. agnes)

add_executable(agnes_tests source/agnes_tests.cpp)

target_include_directories(agnes_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/source/include)

target_sources(agnes_tests PRIVATE
  $<$<CXX_COMPILER_ID:MSVC>:../extras/agnes.natvis>
  source/include/agnes_tests/base.h
  source/include/agnes_tests/class_action.h
  source/include/agnes_tests/columns.h
  source/include/agnes_tests/filled_columns.h
  source/include/agnes_tests/filled_value.h
  source/include/agnes_tests/defaulted.h
  source/include/agnes_tests/derived.h
  source/include/agnes_tests/macros.h
  source/include/agnes_tests/maybe_throw.h
  source/include/agnes_tests/not_noexcept_moveable.h
  source/include/agnes_tests/recorded.h
  source/include/agnes_tests/filled_rows.h
  source/include/agnes_tests/throw_on_copy.h
  source/include/agnes_tests/type_properties.h
  source/include/agnes_tests/user_defined_pointer.h
  source/allocator.tests.cpp
  source/allocator_traits.tests.cpp
  source/column.tests.cpp
  source/construct_at.tests.cpp
  source/include_tests/include_agnes_alignment.cpp
  source/include_tests/include_agnes_cache.cpp
  source/include_tests/include_agnes_column.cpp
  source/include_tests/include_agnes_fields.cpp
  source/include_tests/include_agnes_internal_algorithm.cpp
  source/include_tests/include_agnes_internal_allocated_values.cpp
  source/include_tests/include_agnes_internal_allocator_traits_types.cpp
  source/include_tests/include_agnes_internal_calc_aligned_unit_count.cpp
  source/include_tests/include_agnes_internal_can_construct_at.cpp
  source/include_tests/include_agnes_internal_column_impl.cpp
  source/include_tests/include_agnes_internal_contiguous_base.cpp
  source/include_tests/include_agnes_internal_contiguous_iterator.cpp
  source/include_tests/include_agnes_internal_contiguous_storage.cpp
  source/include_tests/include_agnes_internal_elements_are_assignable.cpp
  source/include_tests/include_agnes_internal_is_placeable.cpp
  source/include_tests/include_agnes_internal_pointer_impl.cpp
  source/include_tests/include_agnes_internal_pointer_traits_types.cpp
  source/include_tests/include_agnes_internal_pointer_tuple.cpp
  source/include_tests/include_agnes_internal_row_iterator.cpp
  source/include_tests/include_agnes_internal_table_base.cpp
  source/include_tests/include_agnes_internal_transactional_construct.cpp
  source/include_tests/include_agnes_internal_tuple_util.cpp
  source/include_tests/include_agnes_internal_type_traits_impl.cpp
  source/include_tests/include_agnes_internal_vector_base.cpp
  source/include_tests/include_agnes_memory.cpp
  source/include_tests/include_agnes_pointer.cpp
  source/include_tests/include_agnes_reference.cpp
  source/include_tests/include_agnes_row.cpp
  source/include_tests/include_agnes_table.cpp
  source/include_tests/include_agnes_type_traits.cpp
  source/include_tests/include_agnes_utility.cpp
  source/include_tests/include_agnes_vector.cpp
  source/pointer.tests.cpp
  source/pointer_traits.tests.cpp
  source/reference.tests.cpp
  source/row.tests.cpp
  source/table.tests.cpp
  source/vector.tests.cpp)

source_group("Source Files\\include_tests" REGULAR_EXPRESSION source/include_tests/...)

target_link_libraries(agnes_tests PRIVATE Catch2::Catch2)
target_link_libraries(agnes_tests PRIVATE agnes)
