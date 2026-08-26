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

#include <compare>
#include <cstddef>
#include <functional>
#include <type_traits>
#include <utility>

#include <agnes/column.h>
#include <agnes/pointer.h>
#include <agnes/reference.h>

namespace agnes
{
	/// A row of elements in a #table.
	///
	/// Specializations:
	///
	/// - @ref mutable_row
	/// - @ref const_row
	template <typename... Columns> class row;

	/// @defgroup mutable_row template <column... Columns> class row<Columns...>
	/// A #row of mutable elements.
	template <column... Columns>
	class row<Columns...> : public Columns...
	{
	public:
		/// @ingroup mutable_row
		/// Binds each element reference in the row directly to a reference.
		constexpr row(column_element_t<Columns>&... values) noexcept : Columns{values}... {}

		/// @ingroup mutable_row
		/// Binds element references in the row to the element references in another row.
		constexpr row(const row& other) = default;

		/// @ingroup mutable_row
		/// Binds element references in the row to the references in a #reference.
		constexpr row(const reference<column_element_t<Columns>&...>& values) noexcept : row{indexes, values} {}

		/// @ingroup mutable_row
		/// Binds element references in the row to the dereferenced pointers in a #pointer.
		constexpr row(const pointer<column_element_t<Columns>*...>& values) noexcept : row{indexes, values} {}

		/// @ingroup mutable_row
		/// Copies the values of elements in another row into this row.
		constexpr row& operator=(const row& other) = default;

		/// @ingroup mutable_row
		/// Moves the values of elements in another row into this row.
		constexpr row& operator=(row&& other) = default;

		/// @ingroup mutable_row
		/// Copies the values of elements in a const row into this row.
		constexpr row& operator=(const row<const_column_t<Columns>...>& other)
			noexcept((std::is_nothrow_assignable_v<Columns&, const const_column_t<Columns>&> && ...))
		{ (Columns::operator=(other), ...); return *this; }

		/// @ingroup mutable_row
		/// Returns a reference to the Ith element in a row.
		template <std::size_t I>
		friend constexpr auto& get(const row& r) noexcept
		{ return static_cast<const std::tuple_element_t<I, std::tuple<Columns...>>&>(r).get(); }

	private:
		static constexpr auto indexes = std::index_sequence_for<Columns...>{};

		template <std::size_t... I>
		constexpr row(std::index_sequence<I...>, const reference<column_element_t<Columns>&...>& values) :
			Columns{get<I>(values)}... {}

		template <std::size_t... I>
		constexpr row(std::index_sequence<I...>, const pointer<column_element_t<Columns>*...>& values) :
			Columns{*get<I>(values)}... {}
	};

	/// @defgroup const_row template <const_column... Columns> class row<Columns...>
	/// A #row of const elements.
	template <const_column... Columns>
	class row<Columns...> : public Columns...
	{
	public:
		/// @ingroup const_row
		/// Binds each element reference in the row directly to a reference.
		constexpr row(column_element_t<Columns>&... values) noexcept : Columns{values}... {}

		/// @ingroup const_row
		/// Binds element references in the row to the element references in another row.
		constexpr row(const row& other) = default;

		/// @ingroup const_row
		/// Binds element references in the row to the references in a #reference.
		constexpr row(const reference<column_element_t<Columns>&...>& values) noexcept : row{indexes, values} {}

		/// @ingroup const_row
		/// Binds element references in the row to the dereferenced pointers in a #pointer.
		constexpr row(const pointer<column_element_t<Columns>*...>& values) noexcept : row{indexes, values} {}

		/// @ingroup const_row
		/// Binds element references in the row to the element references in a mutable row.
		constexpr row(const row<column_t<Columns>...>& other) noexcept : Columns{other}... {}

		/// @ingroup const_row
		/// Binds element references in the row to the mutable references in a #reference.
		constexpr row(const reference<column_element_t<column_t<Columns>>&...>& values) noexcept :
			row{indexes, values} {}

		/// @ingroup const_row
		/// Binds element references in the row to the mutable dereferenced pointers in a #pointer.
		constexpr row(const pointer<column_element_t<column_t<Columns>>*...>& values) noexcept :
			row{indexes, values} {}

		/// @ingroup const_row
		/// Returns a reference to the Ith element in a row.
		template <std::size_t I>
		friend constexpr auto& get(const row& r) noexcept
		{ return static_cast<const std::tuple_element_t<I, std::tuple<Columns...>>&>(r).get(); }

	private:
		static constexpr auto indexes = std::index_sequence_for<Columns...>{};

		template <std::size_t... I>
		constexpr row(std::index_sequence<I...>, const reference<column_element_t<Columns>&...>& values) :
			Columns{get<I>(values)}... {}

		template <std::size_t... I>
		constexpr row(std::index_sequence<I...>, const pointer<column_element_t<Columns>*...>& values) :
			Columns{*get<I>(values)}... {}
	};

	template <std::size_t, typename Row> struct row_column;

	/// Provides a member typedef `type` that names the
	/// `I`th column type of `row<T...>`.
	template <std::size_t I, typename... Columns> 
	struct row_column<I, row<Columns...>> : std::tuple_element<I, std::tuple<Columns...>> {};

	/// Helper type that is an alias for `typename row_column<I, row<Columns...>>::type`.
	template <std::size_t I, typename Row>
	using row_column_t = typename row_column<I, Row>::type;

	template <std::size_t, typename Row> struct row_element;

	/// Provides a member typedef `type` that names the
	/// `I`th column element type of `row<T...>`.
	template <std::size_t I, typename... Columns> 
	struct row_element<I, row<Columns...>> : std::type_identity<column_element_t<row_column_t<I, row<Columns...>>>> {};

	/// Helper type that is an alias for `typename row_element<I, row<Columns...>>::type`.
	template <std::size_t I, typename Row>
	using row_element_t = typename row_element<I, Row>::type;

	template <typename... LhsColumns, typename... RhsColumns>
	constexpr bool operator==(const row<LhsColumns...>& lhs, const row<RhsColumns...>& rhs)
	{ return (std::equal_to{}(static_cast<const LhsColumns&>(lhs), static_cast<const RhsColumns&>(rhs)) && ...); }

	template <typename... LhsColumns, typename... RhsColumns>
	constexpr auto operator<=>(const row<LhsColumns...>& lhs, const row<RhsColumns...>& rhs)
	{
		using result_type = std::common_comparison_category_t<
			std::compare_three_way_result_t<LhsColumns, RhsColumns>...>;

		auto result = result_type::equivalent;

		std::ignore = (((result = std::compare_three_way{}(
			static_cast<const LhsColumns&>(lhs),
			static_cast<const RhsColumns&>(rhs))), result != 0) || ...);

		return result;
	}
}
