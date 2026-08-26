cmake_minimum_required(VERSION 3.24)

project(agnes VERSION 1.0.0 LANGUAGES CXX)

add_library(agnes INTERFACE)

target_include_directories(agnes INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_compile_features(agnes INTERFACE cxx_std_20)

target_sources(agnes PRIVATE
  include/agnes/internal_/algorithm.h
  include/agnes/internal_/allocated_values.h
  include/agnes/internal_/allocator_traits_types.h
  include/agnes/internal_/calc_aligned_unit_count.h
  include/agnes/internal_/can_construct_at.h
  include/agnes/internal_/column_impl.h
  include/agnes/internal_/contiguous_base.h
  include/agnes/internal_/contiguous_iterator.h
  include/agnes/internal_/contiguous_storage.h
  include/agnes/internal_/elements_are_assignable.h
  include/agnes/internal_/is_placeable.h
  include/agnes/internal_/pointer_impl.h
  include/agnes/internal_/pointer_traits_types.h
  include/agnes/internal_/pointer_tuple.h
  include/agnes/internal_/row_iterator.h
  include/agnes/internal_/table_base.h
  include/agnes/internal_/transactional_construct.h
  include/agnes/internal_/tuple_util.h
  include/agnes/internal_/type_traits_impl.h
  include/agnes/internal_/vector_base.h
  include/agnes/alignment.h
  include/agnes/cache.h
  include/agnes/column.h
  include/agnes/fields.h
  include/agnes/memory.h
  include/agnes/pointer.h
  include/agnes/reference.h
  include/agnes/row.h
  include/agnes/table.h
  include/agnes/type_traits.h
  include/agnes/utility.h
  include/agnes/vector.h)

source_group("Header Files\\internal_" REGULAR_EXPRESSION include/agnes/internal_/...)
