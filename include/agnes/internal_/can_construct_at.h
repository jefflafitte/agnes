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
#include <tuple>
#include <type_traits>
#include <utility>

#include <agnes/internal_/is_placeable.h>
#include <agnes/internal_/tuple_util.h>

#include <agnes/reference.h>
#include <agnes/type_traits.h>

namespace agnes { template <typename...> class pointer; }

namespace agnes::internal_
{
	template <typename Pointer> struct can_construct_at : std::false_type {};

	template <typename... T>
		requires (sizeof...(T) > 0) && (is_placeable_v<T> && ...)
	struct can_construct_at<pointer<T*...>> : std::true_type {};

	template <typename... Args> struct can_construct_at_from_single_arg : std::false_type {};

	template <typename Arg>
		requires (!agnes::is_reference_v<Arg>) &&
			(!std_tuple_like<Arg>)             &&
			(!std::is_same_v<Arg, std::piecewise_construct_t>)
	struct can_construct_at_from_single_arg<Arg> : std::true_type {};

	template <typename Pointer, typename... Args> struct can_construct_at_from_args : std::false_type {};

	template <typename... T, typename... Args>
		requires (sizeof...(T) > 0)                                                      &&
			(sizeof...(T) == sizeof...(Args))                                            &&
			((sizeof...(Args) != 1) || can_construct_at_from_single_arg<Args...>::value) &&
			(is_placeable_v<T, Args> && ...)
	struct can_construct_at_from_args<pointer<T*...>, Args...> : std::true_type {};

	template <
		typename Pointer,
		typename Reference,
		typename = std::make_index_sequence<reference_size_v<std::remove_cvref_t<Reference>>>>
	struct can_construct_at_from_reference_impl : std::false_type {};

	template <typename... T, typename Reference, std::size_t... I>
		requires (sizeof...(T) > 0)                                            &&
			(sizeof...(T) == reference_size_v<std::remove_cvref_t<Reference>>) &&
			(is_placeable_v<T, decltype(get<I>(std::declval<Reference>()))> && ...)
	struct can_construct_at_from_reference_impl<pointer<T*...>, Reference, std::index_sequence<I...>> :
		std::true_type {};

	template <typename Pointer, typename Reference>
	struct can_construct_at_from_reference : std::false_type {};

	template <typename... T, typename Reference>
		requires agnes::is_reference_v<Reference>
	struct can_construct_at_from_reference<pointer<T*...>, Reference> :
		can_construct_at_from_reference_impl<pointer<T*...>, Reference> {};

	template <
		typename       Pointer,
		std_tuple_like TupleLike,
		typename = std::make_index_sequence<std::tuple_size_v<std::remove_cvref_t<TupleLike>>>>
	struct can_construct_at_from_std_tuple_like_impl : std::false_type {};

	template <typename... T, std_tuple_like TupleLike, std::size_t... I>
		requires (sizeof...(T) > 0)                                             &&
			(sizeof...(T) == std::tuple_size_v<std::remove_cvref_t<TupleLike>>) &&
			(is_placeable_v<T, decltype(std::get<I>(std::declval<TupleLike>()))> && ...)
	struct can_construct_at_from_std_tuple_like_impl<pointer<T*...>, TupleLike, std::index_sequence<I...>> :
		std::true_type {};

	template <typename Pointer, std_tuple_like TupleLike>
	struct can_construct_at_from_std_tuple_like : std::false_type {};

	template <typename... T, std_tuple_like TupleLike>
	struct can_construct_at_from_std_tuple_like<pointer<T*...>, TupleLike> :
		can_construct_at_from_std_tuple_like_impl<pointer<T*...>, TupleLike> {};

	template <
		typename Pointer,
		typename Tuple,
		typename = std::make_index_sequence<std::tuple_size_v<std::remove_cvref_t<Tuple>>>>
	struct can_construct_at_piecewise_impl : std::false_type {};

	template <typename T, typename Tuple, std::size_t... I>
		requires is_placeable_v<T, decltype(std::get<I>(std::declval<Tuple>()))...>
	struct can_construct_at_piecewise_impl<T*, Tuple, std::index_sequence<I...>> :
		std::true_type {};

	template <typename Pointer, typename... Tuples> struct can_construct_at_piecewise : std::false_type {};

	template <typename... T, typename... Tuples>
		requires (sizeof...(T) > 0)                &&
			(sizeof...(T) == sizeof...(Tuples))    &&
			(internal_::is_tuple_v<Tuples> && ...) &&
			(can_construct_at_piecewise_impl<T*, Tuples>::value && ...)
	struct can_construct_at_piecewise<pointer<T*...>, Tuples...> : std::true_type {};

	template <typename Pointer>
	inline constexpr bool can_construct_at_v = can_construct_at<Pointer>::value;

	template <typename Pointer, typename... Args>
	inline constexpr bool can_construct_at_from_args_v = can_construct_at_from_args<Pointer, Args...>::value;

	template <typename Pointer, typename Reference>
	inline constexpr bool can_construct_at_from_reference_v =
		can_construct_at_from_reference<Pointer, Reference>::value;

	template <typename Pointer, std_tuple_like TupleLike>
	inline constexpr bool can_construct_at_from_std_tuple_like_v =
		can_construct_at_from_std_tuple_like<Pointer, TupleLike>::value;

	template <typename Pointer, typename... Tuples>
	inline constexpr bool can_construct_at_piecewise_v = can_construct_at_piecewise<Pointer, Tuples...>::value;
}
