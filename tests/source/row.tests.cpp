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

#include <agnes/row.h>

#include <tuple>
#include <type_traits>
#include <utility>

#include "agnes_tests/columns.h"

#include "agnes_tests/filled_value.h"
#include "agnes_tests/macros.h"

using namespace agnes_tests;

TEMPLATE_TEST_CASE("agnes::row direct construction", "",
	(std::tuple<column_a<int>, column_i, column_j, column_x>))
{
	static constexpr auto test = [] { return []<std::size_t... Ti, typename... Columns>(
		std::index_sequence<Ti...>,
		std::tuple<Columns...>*)
	{
		using Types = std::tuple<agnes::column_element_t<Columns>...>;

		filled_value<Types, Types>       values;
		const filled_value<Types, Types> constValues;

		const agnes::reference           valueReference     {get<Ti>(values)...};
		const agnes::reference           constValueReference{get<Ti>(constValues)...};

		const agnes::pointer             valuePointer       {&get<Ti>(values)...};
		const agnes::pointer             constValuePointer  {&get<Ti>(constValues)...};

		{
			const agnes::row<Columns...> row{get<Ti>(values)...};

			AGNES_REQUIRE(((&get<Ti>(row) == &get<Ti>(values)) && ...));
		}

		{
			const agnes::row<Columns...> row{valueReference};

			AGNES_REQUIRE(((&get<Ti>(row) == &get<Ti>(values)) && ...));
		}

		{
			const agnes::row<Columns...> row{valuePointer};

			AGNES_REQUIRE(((&get<Ti>(row) == &get<Ti>(values)) && ...));
		}

		{
			const agnes::row<agnes::const_column_t<Columns>...> row{get<Ti>(constValues)...};

			AGNES_REQUIRE(((&get<Ti>(row) == &get<Ti>(constValues)) && ...));
		}

		{
			const agnes::row<agnes::const_column_t<Columns>...> row{constValueReference};

			AGNES_REQUIRE(((&get<Ti>(row) == &get<Ti>(constValues)) && ...));
		}

		{
			const agnes::row<agnes::const_column_t<Columns>...> row{constValuePointer};

			AGNES_REQUIRE(((&get<Ti>(row) == &get<Ti>(constValues)) && ...));
		}

		{
			const agnes::row<agnes::const_column_t<Columns>...> row{get<Ti>(values)...};

			AGNES_REQUIRE(((&get<Ti>(row) == &get<Ti>(values)) && ...));
		}

		{
			const agnes::row<agnes::const_column_t<Columns>...> row{valueReference};

			AGNES_REQUIRE(((&get<Ti>(row) == &get<Ti>(values)) && ...));
		}

		{
			const agnes::row<agnes::const_column_t<Columns>...> row{valuePointer};

			AGNES_REQUIRE(((&get<Ti>(row) == &get<Ti>(values)) && ...));
		}

		return true;

	}(std::make_index_sequence<std::tuple_size_v<TestType>>{}, static_cast<TestType*>(nullptr)); };

	AGNES_RUN(test);
}

TEMPLATE_TEST_CASE("agnes::row copy construction", "",
	(std::tuple<column_a<int>, column_i, column_j, column_x>))
{
	static constexpr auto test = [] { return []<std::size_t... Ti, typename... Columns>(
		std::index_sequence<Ti...>,
		std::tuple<Columns...>*)
	{
		using Types = std::tuple<agnes::column_element_t<Columns>...>;

		filled_value<Types, Types> values;

		{
			const agnes::row<Columns...> other{get<Ti>(values)...};

			const agnes::row<Columns...> row{other};

			AGNES_REQUIRE(((&get<Ti>(row) == &get<Ti>(values)) && ...));
		}

		{
			const agnes::row<Columns...> other{get<Ti>(values)...};

			const agnes::row<agnes::const_column_t<Columns>...> row{other};

			AGNES_REQUIRE(((&get<Ti>(row) == &get<Ti>(values)) && ...));
		}

		return true;

	}(std::make_index_sequence<std::tuple_size_v<TestType>>{}, static_cast<TestType*>(nullptr)); };

	AGNES_RUN(test);
}

TEMPLATE_TEST_CASE("agnes::row move construction", "",
	(std::tuple<column_a<int>, column_i, column_j, column_x>))
{
	static constexpr auto test = [] { return []<std::size_t... Ti, typename... Columns>(
		std::index_sequence<Ti...>,
		std::tuple<Columns...>*)
	{
		using Types = std::tuple<agnes::column_element_t<Columns>...>;

		filled_value<Types, Types> values;

		{
			agnes::row<Columns...> other{get<Ti>(values)...};

			const agnes::row<Columns...> row{std::move(other)};

			AGNES_REQUIRE(((&get<Ti>(row) == &get<Ti>(values)) && ...));
		}

		{
			agnes::row<Columns...> other{get<Ti>(values)...};

			const agnes::row<agnes::const_column_t<Columns>...> row{std::move(other)};

			AGNES_REQUIRE(((&get<Ti>(row) == &get<Ti>(values)) && ...));
		}

		return true;

	}(std::make_index_sequence<std::tuple_size_v<TestType>>{}, static_cast<TestType*>(nullptr)); };

	AGNES_RUN(test);
}

TEMPLATE_TEST_CASE("agnes::row copy assignment", "",
	(std::tuple<column_a<int>, column_i, column_j, column_x>))
{
	static constexpr auto test = [] { return []<std::size_t... Ti, typename... Columns>(
		std::index_sequence<Ti...>,
		std::tuple<Columns...>*)
	{
		using Types = std::tuple<agnes::column_element_t<Columns>...>;

		const auto wasCopied = []<typename T>(const T& value)
		{
			if constexpr (std::is_same_v<T, recorded<int>>)
			{
				return value.actions.last() == class_action::copyAssigned;
			}

			return true;
		};

		{
			filled_value<Types, Types> values     {0};
			filled_value<Types, Types> otherValues{1};

			const agnes::row<Columns...> other{get<Ti>(otherValues)...};

			agnes::row<Columns...> row{get<Ti>(values)...};

			row = other;

			AGNES_REQUIRE(((get<Ti>(row) == get<Ti>(otherValues)) && ...));

			AGNES_REQUIRE((wasCopied(get<Ti>(row)) && ...));
		}

		{
			filled_value<Types, Types> values     {0};
			filled_value<Types, Types> otherValues{1};

			const agnes::row<agnes::const_column_t<Columns>...> other{get<Ti>(otherValues)...};

			agnes::row<Columns...> row{get<Ti>(values)...};

			row = other;

			AGNES_REQUIRE(((get<Ti>(row) == get<Ti>(otherValues)) && ...));

			AGNES_REQUIRE((wasCopied(get<Ti>(row)) && ...));
		}

		return true;

	}(std::make_index_sequence<std::tuple_size_v<TestType>>{}, static_cast<TestType*>(nullptr)); };

	AGNES_RUN(test);
}

TEMPLATE_TEST_CASE("agnes::row move assignment", "",
	(std::tuple<column_a<int>, column_i, column_j, column_x>))
{
	static constexpr auto test = [] { return []<std::size_t... Ti, typename... Columns>(
		std::index_sequence<Ti...>,
		std::tuple<Columns...>*)
	{
		using Types = std::tuple<agnes::column_element_t<Columns>...>;

		const auto wasMoved = []<typename T>(const T& value)
		{
			if constexpr (std::is_same_v<T, recorded<int>>)
			{
				return value.actions.last() == class_action::moveAssigned;
			}

			return true;
		};

		filled_value<Types, Types> values     {0};
		filled_value<Types, Types> otherValues{1};

		agnes::row<Columns...> other{get<Ti>(otherValues)...};

		agnes::row<Columns...> row{get<Ti>(values)...};

		row = std::move(other);

		AGNES_REQUIRE(((get<Ti>(row) == get<Ti>(otherValues)) && ...));

		AGNES_REQUIRE((wasMoved(get<Ti>(row)) && ...));

		return true;

	}(std::make_index_sequence<std::tuple_size_v<TestType>>{}, static_cast<TestType*>(nullptr)); };

	AGNES_RUN(test);
}
