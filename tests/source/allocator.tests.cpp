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

#include <agnes/memory.h>

#include <cstddef>
#include <cstdint>
#include <tuple>

#include <agnes/pointer.h>

#include "agnes_tests/macros.h"

TEMPLATE_TEST_CASE_SIG("agnes::allocator allocate/deallocate", "",
	((typename Types, std::size_t... Counts), Types, Counts...),
	((std::tuple<int                           >), 1, 2, 4, 8, 16),
	((std::tuple<char, int, long, float, double>), 1, 2, 4, 8, 16),
	((std::tuple<double, float, long, int, char>), 1, 2, 4, 8, 16),
	((std::tuple<float, int, char, long, double>), 1, 2, 4, 8, 16))
{
	static constexpr auto test = [] { return ([]<std::size_t... Ti, typename... T, std::size_t N>(
		std::index_sequence<Ti...>,
		std::tuple<T...>*,
		std::integral_constant<std::size_t, N>)
	{
		agnes::allocator<T...> a;

		agnes::pointer<T*...> p{a.allocate(N)};

		AGNES_REQUIRE(p != nullptr);

		if (!std::is_constant_evaluated())
		{
			const std::uintptr_t locations[]{reinterpret_cast<std::uintptr_t>(get<Ti>(p))...};

			AGNES_REQUIRE((((locations[Ti] % alignof(T)) == 0) && ...));

			const std::size_t sizes[]{sizeof(T)...};

			for (auto i = 0; i < (sizeof...(Ti) - 1); ++i)
			{
#if AGNES_AVOID_CACHE_CONFLICTS
				AGNES_REQUIRE((locations[i + 1] - (locations[i] + N*sizes[i])) <=
					std::max({alignof(T)..., agnes::cache_line_size_v}));
#else
				AGNES_REQUIRE((locations[i + 1] - (locations[i] + N*sizes[i])) < std::max({alignof(T)...}));
#endif
			}
		}

		a.deallocate(p, N);

		return true;
	}(std::make_index_sequence<std::tuple_size_v<Types>>{},
		static_cast<Types*>(nullptr),
		std::integral_constant<std::size_t, Counts>{}) && ...); };

	AGNES_RUN(test);
}
