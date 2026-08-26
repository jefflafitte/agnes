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

#include <array>
#include <cstddef>
#include <tuple>
#include <type_traits>
#include <utility>

namespace agnes::internal_
{
	template <typename> struct is_tuple_impl : std::false_type {};

	template <typename... T> struct is_tuple_impl<std::tuple<T...>> : std::true_type {};

	template <typename T> struct is_tuple : is_tuple_impl<std::remove_cvref_t<T>> {};

	template <typename T>
	inline constexpr bool is_tuple_v = is_tuple<T>::value;

	template <typename    T>                inline constexpr bool is_std_tuple_like_v                   = false;
	template <typename... T>                inline constexpr bool is_std_tuple_like_v<std::tuple<T...>> = true;
	template <typename    T, typename    U> inline constexpr bool is_std_tuple_like_v<std::pair <T, U>> = true;
	template <typename    T, std::size_t N> inline constexpr bool is_std_tuple_like_v<std::array<T, N>> = true;

	template <typename T>
	concept std_tuple_like = internal_::is_std_tuple_like_v<std::remove_cvref_t<T>>;
}
