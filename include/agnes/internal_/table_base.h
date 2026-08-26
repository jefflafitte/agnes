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

#include <cstddef>
#include <initializer_list>
#include <iterator>
#include <type_traits>
#include <tuple>
#include <utility>

#include <agnes/internal_/contiguous_base.h>
#include <agnes/internal_/row_iterator.h>

#include <agnes/column.h>
#include <agnes/memory.h>
#include <agnes/pointer.h>
#include <agnes/reference.h>
#include <agnes/utility.h>

namespace agnes { template <typename... Columns> class row; }

namespace agnes::internal_
{
	template <typename...> struct columns_are_unique : std::true_type {};

	template <typename Column, typename... Tail>
	struct columns_are_unique<Column, Tail...> :
		std::bool_constant<(!std::is_same_v<Column, Tail> && ...) && columns_are_unique<Tail...>::value> {};

	template <typename... Columns>
	inline constexpr bool columns_are_unique_v = columns_are_unique<Columns...>::value;

	template <typename, typename...> class table_base;

	template <typename Allocator> class table_base<Allocator>;

	template <typename Allocator, typename... Columns>
	class table_base : public contiguous_base<Allocator, column_element_t<Columns>...>
	{
		static_assert(
			sizeof...(Columns) > 0,
			"agnes::table must be instantiated with at least one template parameter.");

		static_assert(
			(std::is_object_v<column_element_t<Columns>> && ...),
			"All table element types must be objects.");
		static_assert(
			(!std::is_const_v<column_element_t<Columns>> && ...),
			"No table element type may be const-qualified.");
		static_assert(
			(!std::is_volatile_v<column_element_t<Columns>> && ...),
			"No table element type may be volatile-qualified.");
		static_assert(
			columns_are_unique_v<Columns...>,
			"Table columns must be unique types.");

		using base = contiguous_base<Allocator, column_element_t<Columns>...>;

	public:
		using value_type      = base::value_type;
		using allocator_type  = base::allocator_type;
		using pointer         = base::pointer;
		using const_pointer   = base::const_pointer;
		using reference       = base::reference;
		using const_reference = base::const_reference;
		using row             = agnes::row<Columns...>;
		using const_row       = agnes::row<const_column_t<Columns>...>;
		using size_type       = base::size_type;
		using difference_type = base::difference_type;
		using iterator        = row_iterator<pointer, Columns...>;
		using const_iterator  = row_iterator<const_pointer, const_column_t<Columns>...>;

		using base::base;
		using base::operator=;

		constexpr iterator begin() noexcept { return base::first(); }

		constexpr const_iterator begin() const noexcept { return base::first(); }

		constexpr iterator end() noexcept { return base::last(); }

		constexpr const_iterator end() const noexcept { return base::last(); }

		constexpr const_iterator cbegin() const noexcept { return base::first(); }

		constexpr const_iterator cend() const noexcept { return base::last(); }

		constexpr row operator[](const size_type n) { return *(base::first() + n); }

		constexpr const_row operator[](const size_type n) const { return *(base::first() + n); }

		constexpr row at(const size_type n) { base::check_range(n); return *(base::first() + n); }

		constexpr const_row at(const size_type n) const { base::check_range(n); return *(base::first() + n); }

		constexpr row front() { return *base::first(); }

		constexpr const_row front() const { return *base::first(); }

		constexpr row back() { return *(base::last() - 1); }

		constexpr const_row back() const { return *(base::last() - 1); }

		template <typename... Args>
		constexpr row emplace_back(Args&&... args)
		{ base::emplace_back(std::forward<Args>(args)...); return back(); }

		template <typename... Args>
		constexpr iterator emplace(const const_iterator& position, Args&&... args)
		{ return iterator{to_address(base::emplace(
			base::make_const_iterator(position.p_),
			std::forward<Args>(args)...))}; }

		constexpr iterator insert(const const_iterator& position, const column_element_t<Columns>&... values)
		{ return iterator{to_address(base::insert(base::make_const_iterator(position.p_), values...))}; }

		constexpr iterator insert(const const_iterator& position, column_element_t<Columns>&&... values)
		{ return iterator{to_address(base::insert(base::make_const_iterator(position.p_), std::move(values)...))}; }

		constexpr iterator insert(
			const const_iterator&               position,
			const size_type                     n,
			const column_element_t<Columns>&... values)
		{ return iterator{to_address(base::insert(base::make_const_iterator(position.p_), n, values...))}; }

		template <std::input_iterator InputIterator>
		constexpr iterator insert(
			const const_iterator& position,
			const InputIterator&  first,
			const InputIterator&  last)
		{ return iterator{to_address(base::insert(base::make_const_iterator(position.p_), first, last))}; }

		template <typename... U>
			requires (sizeof...(Columns) == sizeof...(U))
		constexpr iterator insert(
			const const_iterator&        position,
			const agnes::pointer<U*...>& first,
			const agnes::pointer<U*...>& last)
		{ return iterator{to_address(base::insert(base::make_const_iterator(position.p_), first, last))}; }

		constexpr iterator insert(
			const const_iterator& position,
			const const_iterator& first,
			const const_iterator& last)
		{ return iterator{to_address(base::insert(base::make_const_iterator(position.p_), first, last))}; }

		constexpr iterator insert(
			const const_iterator&                                                 position,
			const std::initializer_list<std::tuple<column_element_t<Columns>...>> il)
		{ return iterator{to_address(base::insert(base::make_const_iterator(position.p_), il))}; }

		constexpr iterator erase(const const_iterator& position)
		{ return iterator{to_address(base::erase(base::make_const_iterator(position.p_)))}; }

		constexpr iterator erase(const const_iterator& first, const const_iterator& last)
		{ return iterator{to_address(base::erase(
			base::make_const_iterator(first.p_),
			base::make_const_iterator(last.p_)))}; }

		template <typename... ColumnSubset>
		friend constexpr auto begin(table_base& t) noexcept
		{ return subset<ColumnSubset...>(t.begin()); }

		template <typename... ColumnSubset>
		friend constexpr auto begin(const table_base& t) noexcept
		{ return subset<const_column_t<ColumnSubset>...>(t.begin()); }

		template <typename... ColumnSubset>
		friend constexpr auto end(table_base& t) noexcept
		{ return subset<ColumnSubset...>(t.end()); }

		template <typename... ColumnSubset>
		friend constexpr auto end(const table_base& t) noexcept
		{ return subset<const_column_t<ColumnSubset>...>(t.end()); }

		template <typename... ColumnSubset>
		friend constexpr auto cbegin(const table_base& t) noexcept
		{ return subset<const_column_t<ColumnSubset>...>(t.cbegin()); }

		template <typename... ColumnSubset>
		friend constexpr auto cend(const table_base& t) noexcept
		{ return subset<const_column_t<ColumnSubset>...>(t.cend()); }

		template <typename... ColumnSubset>
		friend constexpr auto at(table_base& t, const size_type n)
		{ t.base::check_range(n); return *(subset<ColumnSubset...>(t.begin()) + n); }

		template <typename... ColumnSubset>
		friend constexpr auto at(const table_base& t, const size_type n)
		{ t.base::check_range(n); return *(subset<const_column_t<ColumnSubset>...>(t.begin()) + n); }

		template <typename... ColumnSubset>
		friend constexpr auto front(table_base& t)
		{ return *subset<ColumnSubset...>(t.begin()); }

		template <typename... ColumnSubset>
		friend constexpr auto front(const table_base& t)
		{ return *subset<const_column_t<ColumnSubset>...>(t.begin()); }

		template <typename... ColumnSubset>
		friend constexpr auto back(table_base& t)
		{ return *(subset<ColumnSubset...>(t.end()) - 1); }

		template <typename... ColumnSubset>
		friend constexpr auto back(const table_base& t)
		{ return *(subset<const_column_t<ColumnSubset>...>(t.end()) - 1); }
	};

	template <std::size_t, typename> struct table_base_column;

	template <std::size_t I, typename Allocator, typename... Columns>
	struct table_base_column<I, table_base<Allocator, Columns...>> :
		std::tuple_element<I, std::tuple<Columns...>> {};

	template <std::size_t, typename> struct table_base_element;

	template <std::size_t I, typename Allocator, typename... Columns>
	struct table_base_element<I, table_base<Allocator, Columns...>> :
		std::tuple_element<I, std::tuple<column_element_t<Columns>...>> {};

	template <typename...> struct make_table_base_impl;

	template <typename... Head, typename Next, typename... Tail>
	struct make_table_base_impl<std::tuple<Head...>, Next, Tail...> :
		make_table_base_impl<std::tuple<Head..., Next>, Tail...> {};

	template <typename... Columns>
	struct make_table_base_impl<std::tuple<Columns...>> :
		std::type_identity<table_base<allocator<column_element_t<Columns>...>, Columns...>> {};

	template <typename... Columns, typename Allocator>
	struct make_table_base_impl<std::tuple<Columns...>, allocate_with<Allocator>> :
		std::type_identity<table_base<Allocator, Columns...>> {};

	template <typename Head, typename... Tail>
	struct make_table_base : make_table_base_impl<std::tuple<Head>, Tail...> {};

	template <typename... T>
	using make_table_base_t = make_table_base<T...>::type;
}
