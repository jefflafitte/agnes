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

/// @file

#pragma once

#include <concepts>

#include <agnes/internal_/column_impl.h>

namespace agnes
{
	/// Names the mutable column type for `Column`.
	template <typename Column> using column_t = typename Column::column_type;

	/// Names the const column type for `Column`.
	template <typename Column> using const_column_t = typename Column::column_type::const_column_type;

	/// Names the column element type for `Column`.
	template <typename Column> using column_element_t = typename Column::element_type;

	/// A concept that is satisfied if and only if `Column` is
	/// the same types as `column_t<Column>`.
	template <typename Column>
	concept column = std::same_as<Column, column_t<Column>>;

	/// A concept that is satisfied if and only if `Column` is
	/// the same types as `const_column_t<Column>`.
	template <typename Column>
	concept const_column = std::same_as<Column, const_column_t<Column>>;
}

/// Defines a pair of column classes for mutable and const columns.
///
/// @par Overloads:
///
/// @code AGNES_COLUMN(ValueType, ColumnName) @endcode
///
/// Defines column classes `struct column_##ColumnName` and `struct const_column_##ColumnName`.
///
/// @code AGNES_COLUMN(ColumnClass, ValueType, ColumnName) @endcode
///
/// Defines column classes `struct ColumnClass` and `struct const_##ColumnClass`.
///
/// @code AGNES_COLUMN(ColumnClass, ConstColumnClass, ValueType, ColumnName) @endcode
///
/// Defines column classes `struct ColumnClass` and `struct ConstColumnClass`.
///
/// @par Results:
///
/// All overloads produce the following class definitions:
///
/// @code
///
/// struct ColumnClass
/// {
/// 	static_assert( std::is_object_v  <ValueType>, "A column type must be an object."           );
/// 	static_assert(!std::is_const_v   <ValueType>, "A column type cannot be const-qualified."   );
/// 	static_assert(!std::is_volatile_v<ValueType>, "A column type cannot be volatile-qualified.");
///
/// 	using column_type       = ColumnClass;
/// 	using const_column_type = ConstColumnClass;
/// 	using element_type      = ValueType;
///
/// 	constexpr ColumnClass(ValueType& value) noexcept : ColumnName{value} {}
///
/// 	constexpr ColumnClass(const ColumnClass& other) noexcept : ColumnName{other.ColumnName} {}
///
/// 	constexpr ColumnClass& operator=(const ColumnClass& other)
/// 		noexcept(std::is_nothrow_assignable_v<ValueType&, const ValueType&>)
/// 	{ ColumnName = other.ColumnName; return *this; }
///
/// 	constexpr ColumnClass& operator=(ColumnClass&& other)
/// 		noexcept(std::is_nothrow_assignable_v<ValueType&, ValueType&&>)
/// 	{ ColumnName = std::move(other.ColumnName); return *this; }
///
/// 	constexpr ColumnClass& operator=(const ConstColumnClass& other)
/// 		noexcept(std::is_nothrow_assignable_v<ValueType&, const ValueType&>);
///
/// 	constexpr ValueType& get() const noexcept { return ColumnName; }
///
/// 	ValueType& ColumnName;
/// };
///
/// struct ConstColumnClass
/// {
/// 	using column_type  = ColumnClass;
/// 	using element_type = const ValueType;
///
/// 	constexpr ConstColumnClass(const ValueType& value) noexcept : ColumnName{value} {}
///
/// 	constexpr ConstColumnClass(const ColumnClass& other) noexcept : ColumnName{other.ColumnName} {}
///
/// 	constexpr const ValueType& get() const noexcept { return ColumnName; }
///
/// 	const ValueType& ColumnName;
/// };
///
/// constexpr ColumnClass& ColumnClass::operator=(const ConstColumnClass& other)
/// 	noexcept(std::is_nothrow_assignable_v<ValueType&, const ValueType&>)
/// { ColumnName = other.ColumnName; return *this; }
///
/// constexpr bool operator==(const ColumnClass&  lhs, const ColumnClass& rhs)
/// { return std::equal_to{}(lhs.ColumnName, rhs.ColumnName); }
///
/// constexpr bool operator==(const ColumnClass& lhs, const ConstColumnClass& rhs)
/// { return std::equal_to{}(lhs.ColumnName, rhs.ColumnName); }
///
/// constexpr bool operator==(const ConstColumnClass& lhs, const ConstColumnClass& rhs)
/// { return std::equal_to{}(lhs.ColumnName, rhs.ColumnName); }
///
/// constexpr auto operator<=>(const ColumnClass& lhs, const ColumnClass& rhs)
/// { return std::compare_three_way{}(lhs.ColumnName, rhs.ColumnName); }
///
/// constexpr auto operator<=>(const ColumnClass& lhs, const ConstColumnClass& rhs)
/// { return std::compare_three_way{}(lhs.ColumnName, rhs.ColumnName); }
///
/// constexpr auto operator<=>(const ConstColumnClass& lhs, const ConstColumnClass& rhs)
/// { return std::compare_three_way{}(lhs.ColumnName, rhs.ColumnName); }
///
/// @endcode
#define AGNES_COLUMN(...) AGNES_COLUMN_IMPL_(__VA_ARGS__)

/// Defines a column class template with specializations for mutable and const columns.
///
/// @par Overloads:
///
/// @code AGNES_COLUMN_TEMPLATE(ColumnName) @endcode
///
/// Defines column classes `template <typename T> struct column_##ColumnName`
/// and `template <> struct column##ColumnName<const T>`.
///
/// @code AGNES_COLUMN_TEMPLATE(ColumnClass, ColumnName) @endcode
///
/// Defines column classes `template <typename T> struct ColumnClass`
/// and `template <> struct ColumnClass<const T>`.
///
/// @par Results:
///
/// All overloads produce the following class definitions:
///
/// @code
///
/// template <typename T>
/// struct ColumnClass
/// {
/// 	static_assert( std::is_object_v  <T>, "A column type must be an object."           );
/// 	static_assert(!std::is_const_v   <T>, "A column type cannot be const-qualified."   );
/// 	static_assert(!std::is_volatile_v<T>, "A column type cannot be volatile-qualified.");
///
/// 	using column_type       = ColumnClass;
/// 	using const_column_type = ColumnClass<const T>;
/// 	using element_type      = T;
///
/// 	constexpr ColumnClass(T& value) noexcept : ColumnName{value} {}
///
/// 	constexpr ColumnClass(const ColumnClass& other) noexcept : ColumnName{other.ColumnName} {}
///
/// 	constexpr ColumnClass& operator=(const ColumnClass& other)
/// 		noexcept(std::is_nothrow_assignable_v<T&, const T&>)
/// 	{ ColumnName = other.ColumnName; return *this; }
///
/// 	constexpr ColumnClass& operator=(ColumnClass&& other)
/// 		noexcept(std::is_nothrow_assignable_v<T&, T&&>)
/// 	{ ColumnName = std::move(other.ColumnName); return *this; }
///
/// 	constexpr ColumnClass& operator=(const ColumnClass<const T>& other)
/// 		noexcept(std::is_nothrow_assignable_v<T&, const T&>);
///
/// 	constexpr T& get() const noexcept { return ColumnName; }
///
/// 	T& ColumnName;
/// };
///
/// template <typename T>
/// struct ColumnClass<const T>
/// {
/// 	using column_type  = ColumnClass<T>;
/// 	using element_type = const T;
///
/// 	constexpr ColumnClass(const T& value) noexcept : ColumnName{value} {}
///
/// 	constexpr ColumnClass(const ColumnClass<T>& other) noexcept : ColumnName{other.ColumnName} {}
///
/// 	constexpr const T& get() const noexcept { return ColumnName; }
///
/// 	const T& ColumnName;
/// };
///
/// template <typename T>
/// constexpr ColumnClass<T>& ColumnClass<T>::operator=(const ColumnClass<const T>& other)
/// 	noexcept(std::is_nothrow_assignable_v<T&, const T&>)
/// { ColumnName = other.ColumnName; return *this; }
///
/// template <typename T>
/// constexpr bool operator==(const ColumnClass<T>&  lhs, const ColumnClass<T>& rhs)
/// { return std::equal_to{}(lhs.ColumnName, rhs.ColumnName); }
///
/// template <typename T>
/// constexpr bool operator==(const ColumnClass<T>& lhs, const ColumnClass<const T>& rhs)
/// { return std::equal_to{}(lhs.ColumnName, rhs.ColumnName); }
///
/// template <typename T>
/// constexpr bool operator==(const ColumnClass<const T>& lhs, const ColumnClass<const T>& rhs)
/// { return std::equal_to{}(lhs.ColumnName, rhs.ColumnName); }
///
/// template <typename T>
/// constexpr auto operator<=>(const ColumnClass<T>& lhs, const ColumnClass<T>& rhs)
/// { return std::compare_three_way{}(lhs.ColumnName, rhs.ColumnName); }
///
/// template <typename T>
/// constexpr auto operator<=>(const ColumnClass<T>& lhs, const ColumnClass<const T>& rhs)
/// { return std::compare_three_way{}(lhs.ColumnName, rhs.ColumnName); }
///
/// template <typename T>
/// constexpr auto operator<=>(const ColumnClass<const T>& lhs, const ColumnClass<const T>& rhs)
/// { return std::compare_three_way{}(lhs.ColumnName, rhs.ColumnName); }
///
/// @endcode
#define AGNES_COLUMN_TEMPLATE(...) AGNES_COLUMN_TEMPLATE_IMPL_(__VA_ARGS__)
