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
#include <cstddef>
#include <type_traits>

#include <agnes/column.h>
#include <agnes/memory.h>

namespace agnes { template <typename... Columns> class row; }

namespace agnes::internal_
{
	template <typename, typename...> class table_base;
	template <typename, typename...> class row_iterator;

	template <std::size_t I, typename Column, typename Head, typename... Tail>
	struct column_index_impl : column_index_impl<I + 1, Column, Tail...> {};

	template <std::size_t I, typename Column, typename... Tail>
	struct column_index_impl<I, Column, Column, Tail...> : std::integral_constant<size_t, I> {};

	template <typename Column, typename... Columns>
	struct column_index : column_index_impl<0, Column, Columns...> {};

	template <typename Column, typename... Columns>
	inline constexpr std::size_t column_index_v = column_index<Column, Columns...>::value;

	template <typename, typename... ColumnSubset> struct row_iterator_subset;

	template <typename RowIterator> struct row_iterator_subset<RowIterator>;

	template <typename Pointer, typename... Columns, typename... ColumnSubset>
	struct row_iterator_subset<row_iterator<Pointer, Columns...>, ColumnSubset...> :
		std::type_identity<row_iterator<
			pointer_subset_t<Pointer, column_index_v<ColumnSubset, Columns...>...>,
			ColumnSubset...>> {};

	template <typename RowIterator, typename... ColumnSubset>
	using row_iterator_subset_t = typename row_iterator_subset<RowIterator, ColumnSubset...>::type;

	template <typename Pointer, typename... Columns>
	class row_iterator
	{
		using pointer_traits = agnes::pointer_traits<Pointer>;
		using pointer        = Pointer;

		class row_proxy
		{
		public:
			constexpr agnes::row<Columns...>* operator->() { return &row_; }
		private:
			friend class row_iterator;
			constexpr explicit row_proxy(const pointer& p) : row_{p} {}
			agnes::row<Columns...> row_;
		};

	public:
		using row = agnes::row<Columns...>;

		constexpr row_iterator() noexcept = default;

		template <typename OtherPointer, typename... OtherColumns>
			requires (!std::is_same_v<OtherPointer, Pointer>)   &&
				std::is_convertible_v<OtherPointer, Pointer>    &&
				(sizeof...(OtherColumns) == sizeof...(Columns)) &&
				(std::is_same_v<const_column_t<OtherColumns>, Columns> && ...)
		constexpr row_iterator(const row_iterator<OtherPointer, OtherColumns...>& other) noexcept : p_{other.p_} {}

		constexpr row_iterator operator++(int) noexcept { auto previous = *this; ++p_; return previous; }

		constexpr row_iterator operator--(int) noexcept { auto previous = *this; --p_; return previous; }

		constexpr row operator[](const std::ptrdiff_t n) const noexcept { return p_ + n; }

		constexpr row_proxy operator->() noexcept { return row_proxy{p_}; }

		constexpr row operator*() const noexcept { return p_; }

		constexpr row_iterator& operator++() noexcept { ++p_; return *this; }

		constexpr row_iterator& operator--() noexcept { --p_; return *this; }

		constexpr row_iterator& operator+=(const std::ptrdiff_t n) noexcept { p_ += n; return *this; }

		constexpr row_iterator& operator-=(const std::ptrdiff_t n) noexcept { p_ -= n; return *this; }

		friend constexpr row_iterator operator+(const row_iterator& lhs, const std::ptrdiff_t rhs) noexcept
		{ return row_iterator{lhs.p_ + rhs}; }

		friend constexpr row_iterator operator-(const row_iterator& lhs, const std::ptrdiff_t rhs) noexcept
		{ return row_iterator{lhs.p_ - rhs}; }

		friend constexpr std::ptrdiff_t operator-(const row_iterator& lhs, const row_iterator& rhs) noexcept
		{ return lhs.p_ - rhs.p_; }

		template <typename OtherPointer, typename... OtherColumns>
		friend constexpr bool operator==(
			const row_iterator&                                lhs,
			const row_iterator<OtherPointer, OtherColumns...>& rhs) noexcept
		{ return lhs.equal_to(rhs); }

		template <typename OtherPointer, typename... OtherColumns>
		friend constexpr auto operator<=>(
			const row_iterator&                                lhs,
			const row_iterator<OtherPointer, OtherColumns...>& rhs) noexcept
		{ return lhs.compare_three_way(rhs); }

		template <typename... ColumnSubset>
		friend constexpr row_iterator_subset_t<row_iterator, ColumnSubset...> subset(const row_iterator& iter) noexcept
		{ return iter.get_subset<ColumnSubset...>(); }

	private:
		template <typename, typename...> friend class table_base;
		template <typename, typename...> friend class row_iterator;

		constexpr row_iterator(const pointer& p) noexcept : p_{p} {}

		template <typename OtherPointer>
			requires (!std::is_same_v<OtherPointer, Pointer>) && std::is_convertible_v<OtherPointer, Pointer>
		constexpr row_iterator(const OtherPointer& p) noexcept : p_{p} {}

		template <typename OtherPointer, typename... OtherColumns>
		constexpr bool equal_to(const row_iterator<OtherPointer, OtherColumns...>& rhs) const
		{ return p_ == rhs.p_; }

		template <typename OtherPointer, typename... OtherColumns>
		constexpr auto compare_three_way(const row_iterator<OtherPointer, OtherColumns...>& rhs) const
		{ return p_ <=> rhs.p_; }

		template <typename... ColumnSubset>
		constexpr row_iterator_subset_t<row_iterator, ColumnSubset...> get_subset() const
		{ return subset<column_index_v<ColumnSubset, Columns...>...>(p_); }

		pointer p_;
	};
}
