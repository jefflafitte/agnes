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

#include <agnes/memory.h>
#include <agnes/pointer.h>
#include <agnes/reference.h>
#include <agnes/utility.h>

namespace agnes::internal_
{
	template <typename, typename...> class vector_base;

	template <typename Allocator> class vector_base<Allocator>;

	template <typename Allocator, typename... T>
	class vector_base : public contiguous_base<Allocator, T...>
	{
		static_assert(sizeof...(T) > 0, "agnes::vector must be instantiated with at least one template parameter.");

		static_assert(( std::is_object_v  <T> && ...), "All agnes::vector element types must be objects."        );
		static_assert((!std::is_const_v   <T> && ...), "No agnes::vector element type may be const-qualified."   );
		static_assert((!std::is_volatile_v<T> && ...), "No agnes::vector element type may be volatile-qualified.");

		using base = contiguous_base<Allocator, T...>;

	public:
		using value_type      = base::value_type;
		using allocator_type  = base::allocator_type;
		using pointer         = base::pointer;
		using const_pointer   = base::const_pointer;
		using reference       = base::reference;
		using const_reference = base::const_reference;
		using size_type       = base::size_type;
		using difference_type = base::difference_type;
		using iterator        = base::iterator;
		using const_iterator  = base::const_iterator;

		using base::base;
		using base::operator=;

		constexpr iterator begin() noexcept { return base::make_iterator(base::first()); }

		constexpr const_iterator begin() const noexcept { return base::make_const_iterator(base::first()); }

		constexpr iterator end() noexcept { return base::make_iterator(base::last()); }

		constexpr const_iterator end() const noexcept { return base::make_const_iterator(base::last()); }

		constexpr const_iterator cbegin() const noexcept { return base::make_const_iterator(base::first()); }

		constexpr const_iterator cend() const noexcept { return base::make_const_iterator(base::last()); }

		constexpr reference operator[](const size_type n) { return *(base::first() + n); }

		constexpr const_reference operator[](const size_type n) const { return *(base::first() + n); }

		constexpr reference at(const size_type n) { base::check_range(n); return *(base::first() + n); }

		constexpr const_reference at(const size_type n) const { base::check_range(n); return *(base::first() + n); }

		constexpr reference front() { return *base::first(); }

		constexpr const_reference front() const { return *base::first(); }

		constexpr reference back() { return *(base::last() - 1); }

		constexpr const_reference back() const { return *(base::last() - 1); }

		template <typename... Args>
		constexpr reference emplace_back(Args&&... args)
		{ base::emplace_back(std::forward<Args>(args)...); return back(); }

		template <typename... Args>
		constexpr iterator emplace(const const_iterator& position, Args&&... args)
		{ return base::emplace(position, std::forward<Args>(args)...); }

		constexpr iterator insert(const const_iterator& position, const T&... values)
		{ return base::insert(position, values...); }

		constexpr iterator insert(const const_iterator& position, T&&... values)
		{ return base::insert(position, std::move(values)...); }

		constexpr iterator insert(const const_iterator& position, const size_type n, const T&... values)
		{ return base::insert(position, n, values...); }

		template <std::input_iterator InputIterator>
		constexpr iterator insert(
			const const_iterator& position,
			const InputIterator&  first,
			const InputIterator&  last)
		{ return base::insert(position, first, last); }

		template <typename... U>
			requires (sizeof...(T) == sizeof...(U))
		constexpr iterator insert(
			const const_iterator&        position,
			const agnes::pointer<U*...>& first,
			const agnes::pointer<U*...>& last)
		{ return base::insert(position, first, last); }

		constexpr iterator insert(
			const const_iterator& position,
			const const_iterator& first,
			const const_iterator& last)
		{ return base::insert(position, first, last); }

		constexpr iterator insert(const const_iterator& position, const std::initializer_list<std::tuple<T...>> il)
		{ return base::insert(position, il); }

		constexpr iterator erase(const const_iterator& position)
		{ return base::erase(position); }

		constexpr iterator erase(const const_iterator& first, const const_iterator& last)
		{ return base::erase(first, last); }

		template <std::size_t... I>
		friend constexpr auto begin(vector_base& v) noexcept
		{ return subset<I...>(v.base::first()); }

		template <std::size_t... I>
		friend constexpr auto begin(const vector_base& v) noexcept
		{ return subset<I...>(v.base::first()); }

		template <std::size_t... I>
		friend constexpr auto end(vector_base& v) noexcept
		{ return subset<I...>(v.base::last()); }

		template <std::size_t... I>
		friend constexpr auto end(const vector_base& v) noexcept
		{ return subset<I...>(v.base::last()); }

		template <std::size_t... I>
		friend constexpr auto cbegin(const vector_base& v) noexcept
		{ return subset<I...>(v.base::first()); }

		template <std::size_t... I>
		friend constexpr auto cend(const vector_base& v) noexcept
		{ return subset<I...>(v.base::last()); }

		template <std::size_t... I>
		friend constexpr auto at(vector_base& v, const size_type n)
		{ v.base::check_range(n); return *(subset<I...>(v.base::first()) + n); }

		template <std::size_t... I>
		friend constexpr auto at(const vector_base& v, const size_type n)
		{ v.base::check_range(n); return *(subset<I...>(v.base::first()) + n); }

		template <std::size_t... I>
		friend constexpr auto front(vector_base& v)
		{ return *subset<I...>(v.base::first()); }

		template <std::size_t... I>
		friend constexpr auto front(const vector_base& v)
		{ return *subset<I...>(v.base::first()); }

		template <std::size_t... I>
		friend constexpr auto back(vector_base& v)
		{ return *subset<I...>(v.base::last() - 1); }

		template <std::size_t... I>
		friend constexpr auto back(const vector_base& v)
		{ return *subset<I...>(v.base::last() - 1); }
	};

	template <std::size_t, typename> struct vector_base_element;

	template <std::size_t I, typename Allocator, typename... T>
	struct vector_base_element<I, vector_base<Allocator, T...>> : std::tuple_element<I, std::tuple<T...>> {};

	template <typename...> struct make_vector_base_impl;

	template <typename... Head, typename Next, typename... Tail>
	struct make_vector_base_impl<std::tuple<Head...>, Next, Tail...> :
		make_vector_base_impl<std::tuple<Head..., Next>, Tail...> {};

	template <typename... T>
	struct make_vector_base_impl<std::tuple<T...>> :
		std::type_identity<vector_base<allocator<T...>, T...>> {};

	template <typename... T, typename Allocator>
	struct make_vector_base_impl<std::tuple<T...>, allocate_with<Allocator>> :
		std::type_identity<vector_base<Allocator, T...>> {};

	template <typename Head, typename... Tail>
	struct make_vector_base : make_vector_base_impl<std::tuple<Head>, Tail...> {};

	template <typename... T>
	using make_vector_base_t = make_vector_base<T...>::type;
}
