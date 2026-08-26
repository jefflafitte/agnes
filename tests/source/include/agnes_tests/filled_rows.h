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
	template <typename Types, typename Args = Types, std::size_t = 1> class filled_rows;

	template <typename... Types, typename... Args, std::size_t N>
	class filled_rows<std::tuple<Types...>, std::tuple<Args...>, N>
	{
	public:
		constexpr filled_rows(const std::size_t offset = 0) :
			filled_rows{std::index_sequence_for<Types...>{}, offset} {}

		constexpr const auto& operator[](std::ptrdiff_t n) const noexcept { return values[n]; }

	private:
		template <std::size_t... I>
		constexpr filled_rows(std::index_sequence<I...>, const std::size_t offset)
		{
			constexpr auto columnCount = sizeof...(I);
			constexpr auto cellCount   = N*columnCount;

			const auto baseValue = offset*cellCount;

			for (auto i = std::size_t{0}; i < N; ++i)
			{
				((get<I>(values[i]) = static_cast<std::tuple_element_t<I, std::tuple<Args...>>>(
					baseValue + i*columnCount + I)), ...);
			}
		}

		std::array<std::tuple<Types...>, std::max(N, std::size_t{1})> values;
	};
}
