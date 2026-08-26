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

#include <type_traits>

#include <agnes/pointer.h>
#include <agnes/type_traits.h>

namespace agnes
{
	template <typename...> struct fields;
	template <typename> struct pointer_traits;
}

namespace agnes::internal_
{
	template <typename Alloc> struct allocator_traits_pointer :
		pointer_to<typename Alloc::value_type> {};

	template <typename Alloc>
		requires requires { typename Alloc::pointer; }
	struct allocator_traits_pointer<Alloc> :
		std::type_identity<typename Alloc::pointer> {};

	template <typename Alloc, typename Value> struct allocator_traits_pointer_type_impl;

	template <typename Alloc, typename...T> struct allocator_traits_pointer_type_impl<Alloc, fields<T...>> :
		std::type_identity<
			typename pointer_traits<typename allocator_traits_pointer<Alloc>::type>::template rebind<T...>> {};

	template <typename Alloc> struct allocator_traits_const_pointer :
		allocator_traits_pointer_type_impl<Alloc, add_const_t<typename Alloc::value_type>> {};

	template <typename Alloc>
		requires requires { typename Alloc::const_pointer; }
	struct allocator_traits_const_pointer<Alloc> :
		std::type_identity<typename Alloc::const_pointer> {};

	template <typename Alloc> struct allocator_traits_void_pointer :
		allocator_traits_pointer_type_impl<Alloc, void_t<typename Alloc::value_type>> {};

	template <typename Alloc>
		requires requires { typename Alloc::void_pointer; }
	struct allocator_traits_void_pointer<Alloc> :
		std::type_identity<typename Alloc::void_pointer> {};

	template <typename Alloc> struct allocator_traits_const_void_pointer :
		allocator_traits_pointer_type_impl<Alloc, add_const_t<void_t<typename Alloc::value_type>>> {};

	template <typename Alloc>
		requires requires { typename Alloc::const_void_pointer; }
	struct allocator_traits_const_void_pointer<Alloc> :
		std::type_identity<typename Alloc::const_void_pointer> {};

	template <typename Alloc> struct allocator_traits_difference_type :
		std::type_identity<typename pointer_traits<typename allocator_traits_pointer<Alloc>::type>::difference_type> {};

	template <typename Alloc>
		requires requires { typename Alloc::difference_type; }
	struct allocator_traits_difference_type<Alloc> :
		std::type_identity<typename Alloc::difference_type> {};

	template <typename Alloc> struct allocator_traits_size_type :
		std::make_unsigned<typename allocator_traits_difference_type<Alloc>::type> {};

	template <typename Alloc>
		requires requires { typename Alloc::size_type; }
	struct allocator_traits_size_type<Alloc> :
		std::type_identity<typename Alloc::size_type> {};

	template <typename Alloc> struct allocator_traits_propagate_on_container_copy_assignment :
		std::false_type {};

	template <typename Alloc>
		requires requires { typename Alloc::propagate_on_container_copy_assignment; }
	struct allocator_traits_propagate_on_container_copy_assignment<Alloc> :
		std::type_identity<typename Alloc::propagate_on_container_copy_assignment> {};

	template <typename Alloc> struct allocator_traits_propagate_on_container_move_assignment :
		std::false_type {};

	template <typename Alloc>
		requires requires { typename Alloc::propagate_on_container_move_assignment; }
	struct allocator_traits_propagate_on_container_move_assignment<Alloc> :
		std::type_identity<typename Alloc::propagate_on_container_move_assignment> {};

	template <typename Alloc> struct allocator_traits_propagate_on_container_swap :
		std::false_type {};

	template <typename Alloc>
		requires requires { typename Alloc::propagate_on_container_swap; }
	struct allocator_traits_propagate_on_container_swap<Alloc> :
		std::type_identity<typename Alloc::propagate_on_container_swap> {};

	template <typename Alloc> struct allocator_traits_is_always_equal :
		std::is_empty<Alloc> {};

	template <typename Alloc>
		requires requires { typename Alloc::is_always_equal; }
	struct allocator_traits_is_always_equal<Alloc> :
		std::type_identity<typename Alloc::is_always_equal> {};

	template <typename Alloc, typename... T> struct allocator_traits_rebind_alloc;

	template <template <typename...> typename Alloc, typename... T, typename... U>
	struct allocator_traits_rebind_alloc<Alloc<U...>, T...> :
		std::type_identity<Alloc<T...>> {};

	template <typename Alloc, typename... T>
		requires requires { typename Alloc::template rebind<T...>::other; }
	struct allocator_traits_rebind_alloc<Alloc, T...> :
		std::type_identity<typename Alloc::template rebind<T...>::other> {};

	template <typename Alloc>
	using allocator_traits_pointer_t =
		typename allocator_traits_pointer<Alloc>::type;

	template <typename Alloc>
	using allocator_traits_const_pointer_t =
		typename allocator_traits_const_pointer<Alloc>::type;

	template <typename Alloc>
	using allocator_traits_void_pointer_t =
		typename allocator_traits_void_pointer<Alloc>::type;

	template <typename Alloc>
	using allocator_traits_const_void_pointer_t =
		typename allocator_traits_const_void_pointer<Alloc>::type;

	template <typename Alloc>
	using allocator_traits_difference_type_t =
		typename allocator_traits_difference_type<Alloc>::type;

	template <typename Alloc>
	using allocator_traits_size_type_t =
		typename allocator_traits_size_type<Alloc>::type;

	template <typename Alloc>
	using allocator_traits_propagate_on_container_copy_assignment_t =
		typename allocator_traits_propagate_on_container_copy_assignment<Alloc>::type;

	template <typename Alloc>
	using allocator_traits_propagate_on_container_move_assignment_t =
		typename allocator_traits_propagate_on_container_move_assignment<Alloc>::type;

	template <typename Alloc>
	using allocator_traits_propagate_on_container_swap_t =
		typename allocator_traits_propagate_on_container_swap<Alloc>::type;

	template <typename Alloc>
	using allocator_traits_is_always_equal_t =
		typename allocator_traits_is_always_equal<Alloc>::type;

	template <typename Alloc, typename... T>
	using allocator_traits_rebind_alloc_t =
		typename allocator_traits_rebind_alloc<Alloc, T...>::type;
}
