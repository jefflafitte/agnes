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
#include <utility>

#include <agnes/internal_/algorithm.h>
#include <agnes/internal_/table_base.h>

#include <agnes/column.h>
#include <agnes/pointer.h>
#include <agnes/reference.h>
#include <agnes/row.h>

namespace agnes
{
	template <typename... T>
	class table : public internal_::make_table_base_t<T...>
	{
		using base = internal_::make_table_base_t<T...>;

	public:
		using value_type      = base::value_type;
		using allocator_type  = base::allocator_type;
		using pointer         = base::pointer;
		using const_pointer   = base::const_pointer;
		using reference       = base::reference;
		using const_reference = base::const_reference;
		using row             = base::row;
		using const_row       = base::const_row;
		using size_type       = base::size_type;
		using difference_type = base::difference_type;
		using iterator        = base::iterator;
		using const_iterator  = base::const_iterator;

		using base::base;

		constexpr table(const table&) = default;

		constexpr table(table&&) = default;

		constexpr table& operator=(const table&) = default;

		constexpr table& operator=(table&&) = default;

		using base::operator=;
	};

	template <std::size_t, typename Table> struct table_column;

	/// Provides a member typedef `type` that names the
	/// `I`th column type of `table<T...>`.
	template <std::size_t I, typename... T> 
	struct table_column<I, table<T...>> : internal_::table_base_column<I, internal_::make_table_base_t<T...>> {};

	/// Helper type that is an alias for `typename table_column<I, T>::type`.
	template <std::size_t I, typename Table>
	using table_column_t = typename table_column<I, Table>::type;

	template <std::size_t, typename Table> struct table_element;

	/// Provides a member typedef `type` that names the
	/// `I`th column element type of `table<T...>`.
	template <std::size_t I, typename... T> 
	struct table_element<I, table<T...>> : internal_::table_base_element<I, internal_::make_table_base_t<T...>> {};

	/// Helper type that is an alias for `typename table_element<I, table<T...>>::type`.
	template <std::size_t I, typename Table>
	using table_element_t = typename table_element<I, Table>::type;

	/// Determines if `lhs` and `rhs` contain the same number of
	/// object cross-sections and each cross-section in lhs compares equal with
	/// the cross-section in rhs at the same position. 
	template <typename... T>
	constexpr bool operator==(const table<T...>& lhs, const table<T...>& rhs)
	{
		return (&lhs == &rhs) ||
			((lhs.size() == rhs.size()) && internal_::equal(lhs.begin(), lhs.end(), rhs.begin(), rhs.end()));
	}

	/// Compare the contents of `lhs` and `rhs` lexicographically.
	template <typename... T>
	constexpr auto operator<=>(const table<T...>& lhs, const table<T...>& rhs)
	{
		using result_type = decltype(std::compare_three_way{}(*lhs.begin(), *rhs.begin()));

		return (&lhs == &rhs) ?
			result_type::equivalent :
			internal_::lexicographical_compare_three_way(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
	}

	/// Swaps the contents of `x` and `y`.
	template <typename... T>
	constexpr void swap(table<T...>& x, table<T...>& y) noexcept(noexcept(x.swap(y)))
	{ return x.swap(y); }

	/// Removes all rows from `t` that compare equal to `row`.
	template <typename... T, typename Row>
	constexpr typename table<T...>::size_type erase(table<T...>& t, const Row& row)
	{
		auto       newEnd = internal_::remove(t.begin(), t.end(), row);
		const auto result = t.cend() - newEnd;

		t.erase(newEnd, t.cend());

		return result;
	}

	/// Removes all rows from `t` that satisfy the predicate `pred`.
	template <typename... T, typename Predicate>
	constexpr typename table<T...>::size_type erase_if(table<T...>& t, Predicate pred)
	{
		auto       newEnd = internal_::remove_if(t.begin(), t.end(), pred);
		const auto result = t.cend() - newEnd;

		t.erase(newEnd, t.cend());

		return result;
	}

	template <typename Table, typename... ColumnSubset> class table_subview;

	template <typename Table> class table_subview<Table>;

	/// Subview of a table that exposes rows of
	/// only the objects at each index in `I...`.
	template <typename Table, typename... ColumnSubset>
	class table_subview
	{
	public:
		using size_type = typename Table::size_type;

		constexpr table_subview(Table& t) noexcept : t_{t} {}

		constexpr auto begin() const noexcept { return subset<ColumnSubset...>(t_.begin()); }

		constexpr auto end() const noexcept { return subset<ColumnSubset...>(t_.end()); }

		constexpr auto cbegin() const noexcept { return subset<ColumnSubset...>(t_.cbegin()); }

		constexpr auto cend() const noexcept { return subset<ColumnSubset...>(t_.cend()); }

		constexpr size_type size() const noexcept { return t_.size(); }

		constexpr auto operator[](const size_type n) const { return *(subset<ColumnSubset...>(t_.begin()) + n); }

		constexpr auto at(const size_type n) const { return *(subset<ColumnSubset...>(t_.begin()) + n); }

		constexpr auto front() const { return *subset<ColumnSubset...>(t_.begin()); }

		constexpr auto back() const { return *(subset<ColumnSubset...>(t_.end()) - 1); }

		constexpr auto data() const noexcept { return subset<ColumnSubset...>(t_.data()); }

	private:
		Table& t_;
	};

	/// Creates a mutable subview of #table `t`.
	template <typename... ColumnSubset, typename... T>
	constexpr auto subview(table<T...>& t)
	{ return table_subview<table<T...>, ColumnSubset...>{t}; }

	/// Creates a const subview of #table `t`.
	template <typename... ColumnSubset, typename... T>
	constexpr auto subview(const table<T...>& t)
	{ return table_subview<const table<T...>, ColumnSubset...>{t}; }

#ifdef AGNES_FOR_DOCUMENTATION_ONLY
	/// Stores columns of objects of multiple types in sequences arranged linearly
	/// by column. Provides fast random access to any row.
	template <typename... T>
	class table
	{
	public:
		/// The value types stored in rows of the table.
		using value_type      = fields<T...>;
		/// The allocator type used by the table.
		using allocator_type  = implementation-defined;
		/// A type that provides pointers to mutable objects in a row of the table.
		using pointer         = allocator_traits::pointer;
		/// A type that provides pointers to const objects in a row of the table.
		using const_pointer   = allocator_traits::const_pointer;
		/// A type that provides references to mutable objects in a row of the table.
		using reference       = lvalue_reference_to_t<value_type>;
		/// A type that provides references to const objects in a row of the table.
		using const_reference = lvalue_reference_to_t<add_const_t<value_type>>;
		/// A type containing a reference data member for each column in the table.
		using row             = agnes::row<Columns...>;
		/// A type containing a const reference data member for each column in the table.
		using const_row       = agnes::row<const_column_t<Columns>...>;
		/// A type that represents sizes and indexes in the table.
		using size_type       = allocator_traits::size_type;
		/// A type that represents distances in the table.
		using difference_type = allocator_traits::difference_type;
		/// A random-access iterator to mutable rows in the table.
		using iterator        = implementation-defined;
		/// A random-access iterator to const rows in the table.
		using const_iterator  = implementation-defined;

		/// Constructs an empty table with a default allocator.
		constexpr table() noexcept(noexcept(Allocator{}));

		/// Constructs an empty table with a given allocator.
		constexpr explicit table(const Allocator& alloc) noexcept;

		/// Constructs a table with `n` default-constructed rows.
		constexpr explicit table(size_type n, const Allocator& alloc = Allocator{});

		/// Constructs a table with `n` rows copied from `values`.
		constexpr table(
			size_type        n,
			const T&...      values,
			const Allocator& alloc = Allocator{});

		/// Constructs a table with `n` rows copied from `values`.
		constexpr table(
			size_type                      n,
			agnes::reference<const T&...>& values,
			const allocator_type&          alloc = Allocator{});

		/// Constructs a table by copying cross-sections of
		/// objects in the range [`first`, `last`).
		template <std::input_iterator InputIterator>
		constexpr table(
			InputIterator    first,
			InputIterator    last,
			const Allocator& alloc = Allocator{});

		/// Constructs a table by copying cross-sections of
		/// objects in the range [`first`, `last`).
		constexpr table(
			const agnes::pointer<const T*...>& first,
			const agnes::pointer<const T*...>& last,
			const Allocator&                   alloc = Allocator{});

		/// Constructs a table by copying rows of
		/// objects in the range [`first`, `last`).
		constexpr table(
			const_iterator&  first,
			const_iterator&  last,
			const Allocator& alloc = Allocator{});

		/// Constructs a table by copying the contents of `other`.
		constexpr table(const table& other);

		/// Constructs a table by moving the contents of `other`.
		constexpr table(table&& other) noexcept;

		/// Constructs a table by copying the contents of `other` and
		/// using `alloc` as its allocator.
		constexpr table(const table& other, const Allocator& alloc);

		/// Constructs a table by moving the contents of `other` and
		/// using `alloc` as its allocator.
		constexpr table(table&& other, const Allocator& alloc);

		/// Constructs a table by copying cross-sections of objects in `il`.
		constexpr table(
			std::initializer_list<std::tuple<T...>> il,
			const Allocator&                        alloc = Allocator{});

		/// Destroys the table.
		constexpr ~table();

		/// Replaces the contents of the table by copying the contents of /p other.
		constexpr table& operator=(const table& other);

		/// Replaces the contents of the table by moving the contents of /p other.
		constexpr table& operator=(table&& other);

		/// Replaces the contents of the table by copying cross-sections of
		/// objects in `il`.
		constexpr table& operator=(std::initializer_list<std::tuple<T...>> il);

		/// Replaces the contents of the table by copying cross-sections of
		/// objects from the range [`first`, `last`).
		template <std::input_iterator InputIterator>
		constexpr void assign(InputIterator first, InputIterator last);

		/// Replaces the contents of the table by copying cross-sections of
		/// objects from the range [`first`, `last`).
		template <typename... U>
			requires (sizeof...(T) == sizeof...(U))
		constexpr void assign(const agnes::pointer<U*...>& first, const agnes::pointer<U*...>& last);

		/// Replaces the contents of the table by copying rows from
		/// the range [`first`, `last`).
		constexpr void assign(const const_iterator& first, const const_iterator& last);

		/// Replaces the contents of the table by making `n` copies of `values`.
		constexpr void assign(size_type n, const T&... values);

		/// Replaces the contents of the table by making `n` copies of `values`.
		constexpr void assign(size_type n, const agnes::reference<const T&...>& values);

		/// Replaces the contents of the table by copying cross-sections of
		/// objects from `il`.
		constexpr void assign(std::initializer_list<std::tuple<T...>> il);

		/// Returns a copy of the allocator object.
		constexpr allocator_type get_allocator() const noexcept;

		/// Returns a random access iterator to the first row in the table.
		constexpr iterator begin() noexcept;

		/// Returns a random access const iterator to the first row in the table.
		constexpr const_iterator begin() const noexcept;

		/// Returns a random access iterator to the last row in the table.
		constexpr iterator end() noexcept;

		/// Returns a random access const iterator to the last row in the table.
		constexpr const_iterator end() const noexcept;

		/// Returns a random access const the first row in the table.
		constexpr const_iterator cbegin() const noexcept;

		/// Returns a random access const iterator to the last row in the table.
		constexpr const_iterator cend() const noexcept;

		/// Determines if the table is empty.
		[[nodiscard]] constexpr bool empty() const noexcept;

		/// Returns the number of rows in the table.
		constexpr size_type size() const noexcept;

		/// Returns the maximum number of rows the table can hold.
		constexpr size_type max_size() const noexcept;

		/// Returns the number of rows the table has allocated memory for.
		constexpr size_type capacity() const noexcept;

		/// Resizes the table to contain `sz` rows and
		/// default-constructs any new rows.
		constexpr void resize(const size_type sz);

		/// Resizes the table to contain `sz` rows and
		/// assigns any new rows to copies of `values`.
		constexpr void resize(size_type sz, const T&... values);

		/// Allocates memory for at least `n` rows.
		constexpr void reserve(size_type n);

		/// Reduces allocated memory to the exact amount needed by the table.
		constexpr void shrink_to_fit();

		/// Returns the mutable row at index `n` in the table.
		constexpr row operator[](const size_type n);

		/// Returns the const row at index `n` in the table.
		constexpr const_row operator[](size_type n) const;

		/// Returns the mutable row at index `n` in the table.
		constexpr row at(size_type n);

		/// Returns the const row at index `n` in the table.
		constexpr const_row at(size_type n) const;

		/// Returns the mutable first row in the table.
		constexpr row front();

		/// Returns the const first row in the table.
		constexpr const_row front() const;

		/// Returns the mutable last row in the table.
		constexpr row back();

		/// Returns the const last row in the table.
		constexpr const_row back() const;

		/// Returns mutable pointers to objects in the first row of the table.
		constexpr agnes::pointer<T*...> data() noexcept;

		/// Returns const pointers to objects in the first row of the table.
		constexpr agnes::pointer<const T*...> data() const noexcept;

		/// Adds a row to the end of the table by copying `values`.
		constexpr void push_back(const T&... values);

		/// Adds a row to the end of the table by moving `values`.
		constexpr void push_back(T&&... values);

		/// Discards the last row in the table.
		constexpr void pop_back();

		/// Constructs a row in place at the end of the table.
		template <typename... Args>
		constexpr row emplace_back(Args&&... args);

		/// Constructs a row in place in the table at `position`.
		template <typename... Args>
		constexpr iterator emplace(const const_iterator& position, Args&&... args);

		/// Inserts a row in the table at `position` and copies `values` into them.
		constexpr iterator insert(const const_iterator& position, const T&... values);

		/// Inserts a row in the table at `position` and moves `values` into them.
		constexpr iterator insert(const const_iterator& position, T&&... values);

		/// Inserts `n` row in the table at `position` and copies `values` into them.
		constexpr iterator insert(const const_iterator& position, size_type n, const T&... values);

		/// Inserts each cross-section of objects in
		/// the range [`first`, `last`) into the table at `position`.
		template <std::input_iterator InputIterator>
		constexpr iterator insert(
			const const_iterator& position,
			InputIterator         first,
			InputIterator         last);

		/// Inserts each cross-section of objects in
		/// the range [`first`, `last`) into the table at `position`.
		template <typename... U>
			requires (sizeof...(T) == sizeof...(U))
		constexpr iterator insert(
			const const_iterator&        position,
			const agnes::pointer<U*...>& first,
			const agnes::pointer<U*...>& last);

		/// Inserts each row in the range [`first`, `last`) into
		/// the table at `position`.
		constexpr iterator insert(
			const const_iterator& position,
			const const_iterator& first,
			const const_iterator& last);

		/// Inserts each ross-section of objects in `il` into
		/// the table at `position`.
		constexpr iterator insert(const const_iterator& position, std::initializer_list<std::tuple<T...>> il);

		/// Removes the row from the table at `position`. 
		constexpr iterator erase(const const_iterator& position);

		/// Removes rows from the table in the range of postions [`first`, `last`). 
		constexpr iterator erase(const const_iterator& first, const const_iterator& last);

		/// Swaps the contents of the table with `other`..
		constexpr void swap(table& other);

		/// Clears the contents of the table.
		constexpr void clear() noexcept;

		/// Returns a random access iterator to a subset of
		/// the first row in the table.
		template <std::size_t... I>
		friend constexpr auto begin(table_base& v) noexcept;

		/// Returns a random access const iterator to a subset of
		/// the first row in the table.
		template <std::size_t... I>
		friend constexpr auto begin(const table_base& v) noexcept;

		/// Returns a random access iterator to a subset of
		/// the last row in the table.
		template <std::size_t... I>
		friend constexpr auto end(table_base& v) noexcept;

		/// Returns a random access const iterator to a subset of
		/// the last row in the table.
		template <std::size_t... I>
		friend constexpr auto end(const table_base& v) noexcept;

		/// Returns a random access const iterator to a subset of
		/// the first row in the table.
		template <std::size_t... I>
		friend constexpr auto cbegin(const table_base& v) noexcept;

		/// Returns a random access const iterator to a subset of
		/// the last row in the table.
		template <std::size_t... I>
		friend constexpr auto cend(const table_base& v) noexcept;

		/// Returns a mutable subset of the row at index `n` in the table.
		template <std::size_t... I>
		friend constexpr auto at(table_base& v, size_type n);

		/// Returns a const subset of the row at index `n` in the table.
		template <std::size_t... I>
		friend constexpr auto at(const table_base& v, size_type n);

		/// Returns a  mutable subset of the first row in the table.
		template <std::size_t... I>
		friend constexpr auto front(table_base& v);

		/// Returns a const subset of the first row in the table.
		template <std::size_t... I>
		friend constexpr auto front(const table_base& v);

		/// Returns a  mutable subset of the last row in the table.
		template <std::size_t... I>
		friend constexpr auto back(table_base& v);

		/// Returns a const subset of the last row in the table.
		template <std::size_t... I>
		friend constexpr auto back(const table_base& v);
	};
#endif
}
