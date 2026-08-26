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

#include <cstddef>
#include <type_traits>

namespace agnes
{
	/// Tag template that carries the types in a row of column-oriented data.
	template <typename... T> struct fields {};

	template <typename> struct allocation_size;

	template <typename... T>
	struct allocation_size<fields<T...>> : std::integral_constant<std::size_t, (sizeof(T) + ... + std::size_t{0})> {};

	template <typename T>
	inline constexpr std::size_t allocation_size_v = allocation_size<T>::value;
}
