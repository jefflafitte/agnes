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

#include <array>
#include <cstddef>
#include <tuple>

namespace agnes_tests
{
	template <typename Types, typename Args = Types, std::size_t = 1> class filled_columns;

	template <typename... Types, typename... Args, std::size_t N>
	class filled_columns<std::tuple<Types...>, std::tuple<Args...>, N>
	{
	public:
		constexpr filled_columns() : filled_columns{std::index_sequence_for<Types...>{}} {}

		template <std::size_t I>
		friend constexpr const auto& get(const filled_columns& values) noexcept
		{ return get<I>(values.values); }

	private:
		template <std::size_t... I>
		constexpr filled_columns(std::index_sequence<I...>)
		{
			for (auto i = 0; i < N; ++i)
			{
				((get<I>(values)[i] = static_cast<std::tuple_element_t<I, std::tuple<Args...>>>(
					I*sizeof...(I) + i)), ...);
			}
		}

		std::tuple<std::array<Types, std::max(N, std::size_t{1})>...> values;
	};
}
