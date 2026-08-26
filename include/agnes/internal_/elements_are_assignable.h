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

#include <agnes/internal_/tuple_util.h>

namespace agnes::internal_
{
	template <
		std_tuple_like T,
		std_tuple_like U,
		typename = std::make_index_sequence<std::tuple_size_v<std::remove_reference_t<T>>>>
	struct elements_are_assignable : std::false_type {};

	template <std_tuple_like T, std_tuple_like U, std::size_t... I>
		requires (sizeof...(I) > 0)        &&
			(sizeof...(I) == std::tuple_size_v<std::remove_reference_t<U>>) &&
			(std::is_assignable_v<
				decltype(get<I>(std::forward<T>(std::declval<T>()))),
				decltype(get<I>(std::forward<U>(std::declval<U>())))> && ...)
	struct elements_are_assignable<T, U, std::index_sequence<I...>> : std::bool_constant<true> {};

	template <
		std_tuple_like T,
		std_tuple_like U,
		typename = std::make_index_sequence<std::tuple_size_v<std::remove_reference_t<T>>>>
	struct elements_are_nothrow_assignable : std::false_type {};

	template <std_tuple_like T, std_tuple_like U, std::size_t... I>
		requires (sizeof...(I) > 0)        &&
			(sizeof...(I) == std::tuple_size_v<std::remove_reference_t<U>>) &&
			(std::is_nothrow_assignable_v<
				decltype(get<I>(std::forward<T>(std::declval<T>()))),
				decltype(get<I>(std::forward<U>(std::declval<U>())))> && ...)
	struct elements_are_nothrow_assignable<T, U, std::index_sequence<I...>> : std::bool_constant<true> {};

	template <std_tuple_like T, std_tuple_like U>
	inline constexpr bool elements_are_assignable_v = elements_are_assignable<T, U>::value;

	template <std_tuple_like T, std_tuple_like U>
	inline constexpr bool elements_are_nothrow_assignable_v = elements_are_nothrow_assignable<T, U>::value;
}
