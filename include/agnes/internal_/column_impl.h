// SPDX-FileCopyrightText: 2026 Jeff Lafitte
// SPDX-License-Identifier: AGPL-3.0-or-later

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Affero General Public License as published
// by the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Affero General Public License for more details.
// 
// You should have received a copy of the GNU Affero General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

#pragma once

#include <compare>
#include <functional>
#include <type_traits>
#include <utility>

// AGNES_COLUMN(...) implementation:

#define AGNES_COLUMN_IMPL_(...) AGNES_COLUMN_GET_(__VA_ARGS__)(__VA_ARGS__)

#define AGNES_COLUMN_GET_(...) AGNES_COLUMN_GET_IMPL_( \
	_0,                                                \
	__VA_ARGS__ __VA_OPT__(,)                          \
	AGNES_COLUMN_N_,                                   \
	AGNES_COLUMN_4_,                                   \
	AGNES_COLUMN_3_,                                   \
	AGNES_COLUMN_2_,                                   \
	AGNES_COLUMN_1_,                                   \
	AGNES_COLUMN_0_)
#define AGNES_COLUMN_GET_IMPL_(_0, _1, _2, _3, _4, _N, AgnesColumn, ...) AgnesColumn
#define AGNES_COLUMN_0_(...) AGNES_COLUMN_ERROR_("No arguments")
#define AGNES_COLUMN_1_(...) AGNES_COLUMN_ERROR_("Only one argument")
#define AGNES_COLUMN_2_(ValueType, ColumnName) AGNES_DEFINE_COLUMN_(column_##ColumnName, const_column_##ColumnName, ValueType, ColumnName)
#define AGNES_COLUMN_3_(ColumnClass, ValueType, ColumnName) AGNES_DEFINE_COLUMN_(ColumnClass, const_##ColumnClass, ValueType, ColumnName)
#define AGNES_COLUMN_4_(ColumnClass, ConstColumnClass, ValueType, ColumnName) AGNES_DEFINE_COLUMN_(ColumnClass, ConstColumnClass, ValueType, ColumnName)
#define AGNES_COLUMN_N_(...) AGNES_COLUMN_ERROR_("Too many arguments")
#define AGNES_COLUMN_ERROR_(Msg) static_assert(false, Msg " passed to AGNES_COLUMN(). "               \
	"Must be AGNES_COLUMN(ValueType, ColumnName), AGNES_COLUMN(ColumnClass, ValueType, ColumnName), " \
	"or AGNES_COLUMN(ColumnClass, ConstColumnClass, ValueType, ColumnName).")

#define AGNES_DEFINE_COLUMN_(ColumnClass, ConstColumnClass, ValueType, ColumnName) \
	AGNES_DEFINE_COLUMN_IMPL_(, ColumnClass, ConstColumnClass, ColumnClass, ConstColumnClass, ValueType, ColumnName)

// AGNES_COLUMN_TEMPLATE(...) implementation:

#define AGNES_COLUMN_TEMPLATE_IMPL_(...) AGNES_COLUMN_TEMPLATE_GET_(__VA_ARGS__)(__VA_ARGS__)

#define AGNES_COLUMN_TEMPLATE_GET_(...) AGNES_COLUMN_TEMPLATE_GET_IMPL_( \
	_0,                                                                  \
	__VA_ARGS__ __VA_OPT__(,)                                            \
	AGNES_COLUMN_TEMPLATE_N_,                                            \
	AGNES_COLUMN_TEMPLATE_2_,                                            \
	AGNES_COLUMN_TEMPLATE_1_,                                            \
	AGNES_COLUMN_TEMPLATE_0_)
#define AGNES_COLUMN_TEMPLATE_GET_IMPL_(_0, _1, _2, _N, AgnesColumnTemplate, ...) AgnesColumnTemplate
#define AGNES_COLUMN_TEMPLATE_0_(...) AGNES_COLUMN_TEMPLATE_ERROR_("No arguments")
#define AGNES_COLUMN_TEMPLATE_1_(ColumnName) AGNES_DEFINE_COLUMN_TEMPLATE_(column_##ColumnName, ColumnName)
#define AGNES_COLUMN_TEMPLATE_2_(ColumnClass, ColumnName) AGNES_DEFINE_COLUMN_TEMPLATE_(ColumnClass, ColumnName)
#define AGNES_COLUMN_TEMPLATE_N_(...) AGNES_COLUMN_TEMPLATE_ERROR_("Too many arguments")
#define AGNES_COLUMN_TEMPLATE_ERROR_(Msg) static_assert(false, Msg " passed to AGNES_COLUMN_TEMPLATE(). " \
	"Must be AGNES_COLUMN_TEMPLATE( ColumnName) or AGNES_COLUMN_TEMPLATE(ColumnClass, ColumnName), ")

#define AGNES_DEFINE_COLUMN_TEMPLATE_(ColumnClass, ColumnName) \
	AGNES_DEFINE_COLUMN_IMPL_(template <typename T>, ColumnClass, ColumnClass, ColumnClass<T>, ColumnClass<const T>, T, ColumnName)

// Column class definitions:

#define AGNES_DEFINE_COLUMN_IMPL_(Template, ColumnClass, ConstColumnClass, Specialization, ConstSpecialization, ValueType, ColumnName)  \
	Template struct ColumnClass;                                                                                                        \
	Template struct ConstSpecialization;                                                                                                \
	Template                                                                                                                            \
	struct ColumnClass                                                                                                                  \
	{                                                                                                                                   \
		static_assert( std::is_object_v  <ValueType>, "A column type must be an object."           );                                   \
		static_assert(!std::is_const_v   <ValueType>, "A column type cannot be const-qualified."   );                                   \
		static_assert(!std::is_volatile_v<ValueType>, "A column type cannot be volatile-qualified.");                                   \
		using column_type       = ColumnClass;                                                                                          \
		using const_column_type = ConstSpecialization;                                                                                  \
		using element_type      = ValueType;                                                                                            \
		constexpr ColumnClass(ValueType& value) noexcept : ColumnName{value} {}                                                         \
		constexpr ColumnClass(const ColumnClass& other) noexcept : ColumnName{other.ColumnName} {}                                      \
		constexpr ColumnClass& operator=(const ColumnClass& other)                                                                      \
			noexcept(std::is_nothrow_assignable_v<ValueType&, const ValueType&>)                                                        \
		{ ColumnName = other.ColumnName; return *this; }                                                                                \
		constexpr ColumnClass& operator=(ColumnClass&& other)                                                                           \
			noexcept(std::is_nothrow_assignable_v<ValueType&, ValueType&&>)                                                             \
		{ ColumnName = std::move(other.ColumnName); return *this; }                                                                     \
		constexpr ColumnClass& operator=(const ConstSpecialization& other)                                                              \
			noexcept(std::is_nothrow_assignable_v<ValueType&, const ValueType&>);                                                       \
		constexpr ValueType& get() const noexcept { return ColumnName; }                                                                \
		ValueType& ColumnName;                                                                                                          \
	};                                                                                                                                  \
	Template                                                                                                                            \
	struct ConstSpecialization                                                                                                          \
	{                                                                                                                                   \
		using column_type  = Specialization;                                                                                            \
		using element_type = const ValueType;                                                                                           \
		constexpr ConstColumnClass(const ValueType& value) noexcept : ColumnName{value} {}                                              \
		constexpr ConstColumnClass(const Specialization& other) noexcept : ColumnName{other.ColumnName} {}                              \
		constexpr const ValueType& get() const noexcept { return ColumnName; }                                                          \
		const ValueType& ColumnName;                                                                                                    \
	};                                                                                                                                  \
	Template                                                                                                                            \
	constexpr Specialization& Specialization::operator=(const ConstSpecialization& other)                                               \
		noexcept(std::is_nothrow_assignable_v<ValueType&, const ValueType&>)                                                            \
	{ ColumnName = other.ColumnName; return *this; }                                                                                    \
	Template                                                                                                                            \
	constexpr bool operator==(const Specialization&  lhs, const Specialization& rhs)                                                    \
	{ return std::equal_to{}(lhs.ColumnName, rhs.ColumnName); }                                                                         \
	Template                                                                                                                            \
	constexpr bool operator==(const Specialization& lhs, const ConstSpecialization& rhs)                                                \
	{ return std::equal_to{}(lhs.ColumnName, rhs.ColumnName); }                                                                         \
	Template                                                                                                                            \
	constexpr bool operator==(const ConstSpecialization& lhs, const ConstSpecialization& rhs)                                           \
	{ return std::equal_to{}(lhs.ColumnName, rhs.ColumnName); }                                                                         \
	Template                                                                                                                            \
	constexpr auto operator<=>(const Specialization& lhs, const Specialization& rhs)                                                    \
	{ return std::compare_three_way{}(lhs.ColumnName, rhs.ColumnName); }                                                                \
	Template                                                                                                                            \
	constexpr auto operator<=>(const Specialization& lhs, const ConstSpecialization& rhs)                                               \
	{ return std::compare_three_way{}(lhs.ColumnName, rhs.ColumnName); }                                                                \
	Template                                                                                                                            \
	constexpr auto operator<=>(const ConstSpecialization& lhs, const ConstSpecialization& rhs)                                          \
	{ return std::compare_three_way{}(lhs.ColumnName, rhs.ColumnName); }
