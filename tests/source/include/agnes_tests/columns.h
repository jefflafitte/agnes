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

#include <agnes/column.h>

#include "agnes_tests/macros.h"
#include <agnes_tests/recorded.h>

namespace agnes_tests
{
	template <typename Column, typename ValueType>
	concept valid_column =
		std::is_same_v<typename Column::column_type, Column> &&
		std::is_same_v<typename Column::element_type, ValueType> &&
		std::is_constructible_v<Column, ValueType&> &&
		std::is_nothrow_copy_constructible_v<Column> &&
		std::is_nothrow_move_constructible_v<Column> &&
		std::is_assignable_v<Column&, const Column&> &&
		std::is_assignable_v<Column&, Column&&> &&
		std::is_assignable_v<Column&, const typename Column::const_column_type&> &&
		std::is_same_v<decltype(std::declval<Column>().get()), ValueType&> &&
		std::is_same_v<typename Column::const_column_type::column_type, Column> &&
		std::is_same_v<typename Column::const_column_type::element_type, const ValueType> &&
		std::is_constructible_v<typename Column::const_column_type, const ValueType&> &&
		std::is_trivially_copy_constructible_v<typename Column::const_column_type> &&
		std::is_trivially_move_constructible_v<typename Column::const_column_type> &&
		std::is_nothrow_copy_constructible_v<typename Column::const_column_type> &&
		std::is_nothrow_move_constructible_v<typename Column::const_column_type> &&
		std::is_same_v<decltype(std::declval<typename Column::const_column_type>().get()), const ValueType&>;

	AGNES_COLUMN(int, two_arg);
	AGNES_COLUMN(column_three_arg, int, three_arg);

	AGNES_COLUMN(column_i, const_column_i, recorded<int>, i);

	AGNES_COLUMN(column_x, const_column_x, float, x);
	AGNES_COLUMN(column_y, const_column_y, float, y);
	AGNES_COLUMN(column_z, const_column_z, float, z);

	AGNES_COLUMN_TEMPLATE(one_arg);

	AGNES_COLUMN_TEMPLATE(column_a, a);
	AGNES_COLUMN_TEMPLATE(column_b, b);
	AGNES_COLUMN_TEMPLATE(column_c, c);

	struct const_column_j;

	struct column_j
	{
		using column_type       = column_j; 
		using const_column_type = const_column_j;
		using element_type      = recorded<int>;

		constexpr column_j(recorded<int>& value) noexcept : i{value} {}

		constexpr column_j(const column_j& other) noexcept : i{other.i} {}

		constexpr column_j& operator=(const       column_j&  other) noexcept { i =           other.i ; return *this; }
		constexpr column_j& operator=(            column_j&& other) noexcept { i = std::move(other.i); return *this; }
		constexpr column_j& operator=(const const_column_j&  other) noexcept;

		constexpr recorded<int>& get() const noexcept { return i; }

		recorded<int>& i;
	};

	struct const_column_j
	{
		using column_type  = column_j;
		using element_type = const recorded<int>;

		constexpr const_column_j(const recorded<int>& value) noexcept : i{value} {}

		constexpr const_column_j(const column_j& other) noexcept : i{other.i} {}

		constexpr const recorded<int>& get() const noexcept { return i; }

		const recorded<int>& i;
	};

	constexpr column_j& column_j::operator=(const const_column_j& other) noexcept { i = other.i ; return *this; }

	constexpr bool operator==(const column_j&       lhs, const column_j&       rhs) noexcept { return lhs.i == rhs.i; }
	constexpr bool operator==(const column_j&       lhs, const const_column_j& rhs) noexcept { return lhs.i == rhs.i; }
	constexpr bool operator==(const const_column_j& lhs, const const_column_j& rhs) noexcept { return lhs.i == rhs.i; }

	constexpr auto operator<=>(const column_j&       lhs, const column_j&       rhs) noexcept { return lhs.i <=> rhs.i; }
	constexpr auto operator<=>(const column_j&       lhs, const const_column_j& rhs) noexcept { return lhs.i <=> rhs.i; }
	constexpr auto operator<=>(const const_column_j& lhs, const const_column_j& rhs) noexcept { return lhs.i <=> rhs.i; }
}

#define COLUMN_COND(I, ...) \
	[&] { if constexpr (sizeof...(Columns) > I) { return (__VA_ARGS__); } else { return true; } }()
#define COLUMN_A_COND(...) COLUMN_COND(0, __VA_ARGS__)
#define COLUMN_B_COND(...) COLUMN_COND(1, __VA_ARGS__)
#define COLUMN_C_COND(...) COLUMN_COND(2, __VA_ARGS__)

#define COLUMN_REQUIRE(I, ...) if constexpr (sizeof...(Columns) > I) { AGNES_REQUIRE(__VA_ARGS__); }
#define COLUMN_A_REQUIRE(...) COLUMN_REQUIRE(0, __VA_ARGS__)
#define COLUMN_B_REQUIRE(...) COLUMN_REQUIRE(1, __VA_ARGS__)
#define COLUMN_C_REQUIRE(...) COLUMN_REQUIRE(2, __VA_ARGS__)
