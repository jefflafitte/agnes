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
#include <type_traits>

#include <agnes/vector.h>

namespace agnes_benchmarks
{
	namespace detail
	{
		template <typename, typename, std::size_t ColumnCount>
		struct make_agnes_row_vector_impl;

		template <typename... T, typename U>
		struct make_agnes_row_vector_impl<agnes::vector<T...>, U, 0> :
			std::type_identity<agnes::vector<T...>> {};

		template <typename... T, typename U, std::size_t ColumnCount>
		struct make_agnes_row_vector_impl<agnes::vector<T...>, U, ColumnCount> :
			make_agnes_row_vector_impl<agnes::vector<T..., U>, U, ColumnCount - 1> {};

		template <typename T, std::size_t ColumnCount>
		struct make_agnes_row_vector : make_agnes_row_vector_impl<agnes::vector<>, T, ColumnCount> {};
	}

	template <typename T, std::size_t ColumnCount>
	using agnes_row_vector = typename detail::make_agnes_row_vector<T, ColumnCount>::type;
}
