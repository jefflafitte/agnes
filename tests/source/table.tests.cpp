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

#include <agnes/table.h>

#include <cstddef>
#include <type_traits>
#include <tuple>
#include <utility>

#include "agnes_tests/columns.h"
#include "agnes_tests/filled_columns.h"
#include "agnes_tests/filled_rows.h"
#include "agnes_tests/filled_value.h"
#include "agnes_tests/macros.h"
#include "agnes_tests/not_noexcept_moveable.h"
#include "agnes_tests/recorded.h"
#include "agnes_tests/user_defined_allocator.h"

using namespace agnes_tests;

TEMPLATE_TEST_CASE("agnes::table default construction", "",
	(std::tuple<column_a<int>                 >),
	(std::tuple<column_a<int>, column_b<float>>))
{
	static constexpr auto test = [] { return []<typename... Columns>(std::tuple<Columns...>*)
	{
		const agnes::table<Columns...> t;

		AGNES_REQUIRE(t.empty());

		return true;

	}(static_cast<TestType*>(nullptr)); };

	AGNES_RUN(test);
}

TEMPLATE_TEST_CASE_SIG("agnes::table default inserted construction", "",
	((typename ColumnTypes, std::size_t... Counts), ColumnTypes, Counts...),
	((std::tuple<column_a<recorded<int, 0>>                            >), 0, 1, 2),
	((std::tuple<column_a<recorded<int, 0>>, column_b<recorded<int, 1>>>), 0, 1, 2))
{
	static constexpr auto test = [] { return ([]<typename... Columns, std::size_t N>(
		std::tuple<Columns...>*,
		std::integral_constant<std::size_t, N>)
	{
		const agnes::table<Columns...> t(N);

		AGNES_REQUIRE(t.size() == N);

		for (auto j = 0; j < N; ++j)
		{
			COLUMN_A_REQUIRE(t[j].a == agnes::table_element_t<0, agnes::table<Columns...>>{});
			COLUMN_B_REQUIRE(t[j].b == agnes::table_element_t<1, agnes::table<Columns...>>{});

			COLUMN_A_REQUIRE(t[j].a.actions.equals({class_action::defaultConstructed}));
			COLUMN_B_REQUIRE(t[j].b.actions.equals({class_action::defaultConstructed}));
		}

		return true;

	}(static_cast<ColumnTypes*>(nullptr), std::integral_constant<std::size_t, Counts>{}) && ...); };

	AGNES_RUN(test);
}

TEMPLATE_TEST_CASE_SIG("agnes::table copy inserted construction", "",
	((typename ColumnTypes, typename Args, std::size_t... Counts), ColumnTypes, Args, Counts...),
	((std::tuple<column_a<recorded<int, -1>>                             >), (std::tuple<int       >), 0, 1, 2),
	((std::tuple<column_a<recorded<int, -1>>, column_b<recorded<int, -2>>>), (std::tuple<int, short>), 0, 1, 2))
{
	static constexpr auto test = [] { return ([]<std::size_t... Ti, typename... Columns, std::size_t N>(
		std::index_sequence<Ti...>,
		std::tuple<Columns...>*,
		std::integral_constant<std::size_t, N>)
	{
		using Types = std::tuple<agnes::column_element_t<Columns>...>;

		const Types inserted{filled_value<Types, Args>{}};

		{
			const agnes::table<Columns...> t(N, get<Ti>(inserted)...);

			AGNES_REQUIRE(t.size() == N);

			for (auto j = 0; j < N; ++j)
			{
				COLUMN_A_REQUIRE(t[j].a == get<0>(inserted));
				COLUMN_B_REQUIRE(t[j].b == get<1>(inserted));

				COLUMN_A_REQUIRE(t[j].a.actions.equals({class_action::copyConstructed}));
				COLUMN_B_REQUIRE(t[j].b.actions.equals({class_action::copyConstructed}));
			}
		}

		{
			const agnes::table<Columns...> t(N, agnes::reference{get<Ti>(inserted)...});

			AGNES_REQUIRE(t.size() == N);

			for (auto j = 0; j < N; ++j)
			{
				COLUMN_A_REQUIRE(t[j].a == get<0>(inserted));
				COLUMN_B_REQUIRE(t[j].b == get<1>(inserted));

				COLUMN_A_REQUIRE(t[j].a.actions.equals({class_action::copyConstructed}));
				COLUMN_B_REQUIRE(t[j].b.actions.equals({class_action::copyConstructed}));
			}
		}

		return true;

	}(std::make_index_sequence<std::tuple_size_v<ColumnTypes>>{},
		static_cast<ColumnTypes*>(nullptr),
		std::integral_constant<std::size_t, Counts>{}) && ...); };

	AGNES_RUN(test);
}

TEMPLATE_TEST_CASE_SIG("agnes::table iterator inserted construction", "",
	((typename ColumnTypes, typename Args, std::size_t... Counts), ColumnTypes, Args, Counts...),
	((std::tuple<column_a<recorded<int, -1>>                             >), (std::tuple<int       >), 0, 1, 2),
	((std::tuple<column_a<recorded<int, -1>>, column_b<recorded<int, -2>>>), (std::tuple<int, short>), 0, 1, 2))
{
	static constexpr auto test = [] { return ([]<std::size_t... Ti, typename... Columns, std::size_t N>(
		std::index_sequence<Ti...>,
		std::tuple<Columns...>*,
		std::integral_constant<std::size_t, N>)
	{
		using Types = std::tuple<agnes::column_element_t<Columns>...>;

		const filled_columns<Types, Args, N> iterated;

		const agnes::pointer first{get<Ti>(iterated).data()...};
		const agnes::pointer last {first + N};

		const agnes::table<Columns...> t{first, last};

		AGNES_REQUIRE(t.size() == N);

		for (auto j = 0; j < N; ++j)
		{
			COLUMN_A_REQUIRE(t[j].a == get<0>(iterated)[j]);
			COLUMN_B_REQUIRE(t[j].b == get<1>(iterated)[j]);

			COLUMN_A_REQUIRE(t[j].a.actions.equals({class_action::copyConstructed}));
			COLUMN_B_REQUIRE(t[j].b.actions.equals({class_action::copyConstructed}));
		}

		return true;

	}(std::make_index_sequence<std::tuple_size_v<ColumnTypes>>{},
		static_cast<ColumnTypes*>(nullptr),
		std::integral_constant<std::size_t, Counts>{}) && ...); };

	AGNES_RUN(test);
}

TEMPLATE_TEST_CASE_SIG("agnes::table copy construction", "",
	((typename ColumnTypes, typename Args, std::size_t... Counts), ColumnTypes, Args, Counts...),
	((std::tuple<column_a<recorded<int, -1>>                             >), (std::tuple<int     >), 0, 1, 2),
	((std::tuple<column_a<recorded<int, -1>>, column_b<recorded<int, -2>>>), (std::tuple<int, int>), 0, 1, 2))
{
	static constexpr auto test = [] { return ([]<std::size_t... Ti, typename... Columns, std::size_t N>(
		std::index_sequence<Ti...>,
		std::tuple<Columns...>*,
		std::integral_constant<std::size_t, N>)
	{
		using Types = std::tuple<agnes::column_element_t<Columns>...>;

		const filled_rows<Types, Args, N> otherInitial;

		const agnes::table<Columns...> other{otherInitial[Ti]...};

		const agnes::table<Columns...> t{other};

		AGNES_REQUIRE((t == other));

		for (auto j = 0; j < N; ++j)
		{
			COLUMN_A_REQUIRE(t[j].a.actions.equals({class_action::copyConstructed}));
			COLUMN_B_REQUIRE(t[j].b.actions.equals({class_action::copyConstructed}));
		}

		return true;

	}(std::make_index_sequence<Counts>{},
		static_cast<ColumnTypes*>(nullptr),
		std::integral_constant<std::size_t, Counts>{}) && ...); };

	AGNES_RUN(test);
}

TEMPLATE_TEST_CASE_SIG("agnes::table move construction", "",
	((typename ColumnTypes, typename Args, std::size_t... Counts), ColumnTypes, Args, Counts...),
	((std::tuple<column_a<recorded<int, -1>>                             >), (std::tuple<int     >), 0, 1, 2),
	((std::tuple<column_a<recorded<int, -1>>, column_b<recorded<int, -2>>>), (std::tuple<int, int>), 0, 1, 2))
{
	static constexpr auto test = [] { return ([]<std::size_t... Ti, typename... Columns, std::size_t N>(
		std::index_sequence<Ti...>,
		std::tuple<Columns...>*,
		std::integral_constant<std::size_t, N>)
	{
		using Types = std::tuple<agnes::column_element_t<Columns>...>;

		const filled_rows<Types, Args, N> otherInitial;

		agnes::table<Columns...> other   {otherInitial[Ti]...};
		const agnes::pointer     prevData{other.data()};

		const agnes::table<Columns...> t{std::move(other)};

		AGNES_REQUIRE(t.data() == prevData);

		return true;

	}(std::make_index_sequence<Counts>{},
		static_cast<ColumnTypes*>(nullptr),
		std::integral_constant<std::size_t, Counts>{}) && ...); };

	AGNES_RUN(test);
}

TEMPLATE_TEST_CASE_SIG("agnes::table construction from initializer list", "",
	((typename ColumnTypes, typename Args, std::size_t... Counts), ColumnTypes, Args, Counts...),
	((std::tuple<column_a<recorded<int, -1>>                             >), (std::tuple<int     >), 0, 1, 2),
	((std::tuple<column_a<recorded<int, -1>>, column_b<recorded<int, -2>>>), (std::tuple<int, int>), 0, 1, 2))
{
	static constexpr auto test = [] { return ([]<std::size_t... Ti, typename... Columns, std::size_t N>(
		std::index_sequence<Ti...>,
		std::tuple<Columns...>*,
		std::integral_constant<std::size_t, N>)
	{
		using Types = std::tuple<agnes::column_element_t<Columns>...>;

		const filled_rows<Types, Args, N> initializing;

		const agnes::table<Columns...> t{initializing[Ti]...};

		for (auto j = 0; j < N; ++j)
		{
			COLUMN_A_REQUIRE(t[j].a == get<0>(initializing[j]));
			COLUMN_B_REQUIRE(t[j].b == get<1>(initializing[j]));

			COLUMN_A_REQUIRE(t[j].a.actions.equals({class_action::copyConstructed}));
			COLUMN_B_REQUIRE(t[j].b.actions.equals({class_action::copyConstructed}));
		}

		return true;

	}(std::make_index_sequence<Counts>{},
		static_cast<ColumnTypes*>(nullptr),
		std::integral_constant<std::size_t, Counts>{}) && ...); };

	AGNES_RUN(test);
}

TEMPLATE_TEST_CASE_SIG("agnes::table copy assignment", "",
	((typename ColumnTypes, typename Args, std::size_t... Counts), ColumnTypes, Args, Counts...),
	((std::tuple<column_a<recorded<int, -1>>                             >), (std::tuple<int     >), 0, 1, 2),
	((std::tuple<column_a<recorded<int, -1>>, column_b<recorded<int, -2>>>), (std::tuple<int, int>), 0, 1, 2))
{
	static constexpr auto test = []
	{
		const auto test = []<std::size_t... Ti, typename... Columns, std::size_t... I, std::size_t N>(
			std::index_sequence<Ti...>,
			std::tuple<Columns...>*,
			std::index_sequence<I...>,
			std::integral_constant<std::size_t, N>)
		{
			// (Too much for MSVC to call this lambda anonymously.)

			using Types = std::tuple<agnes::column_element_t<Columns>...>;

			const auto test = [](const auto& assign, const auto& compare)
			{
				// [] = [y[0]...y[N]]
				{
					agnes::table<Columns...> t;

					assign(t);

					for (auto i = 0; i < N; ++i)
					{
						AGNES_REQUIRE(compare(t, i));
						COLUMN_A_REQUIRE(t[i].a.actions.equals({class_action::copyConstructed}));
						COLUMN_B_REQUIRE(t[i].b.actions.equals({class_action::copyConstructed}));
					}
				}

				// [x[0]...x[N]] = [y[0]...y[N]]
				{
					agnes::table<Columns...> t(N);

					assign(t);

					for (auto i = 0; i < N; ++i)
					{
						AGNES_REQUIRE(compare(t, i));
						COLUMN_A_REQUIRE(t[i].a.actions.equals(
							{class_action::defaultConstructed, class_action::copyAssigned}));
						COLUMN_B_REQUIRE(t[i].b.actions.equals(
							{class_action::defaultConstructed, class_action::copyAssigned}));
					}
				}

				// [x[0], unused[1]...unused[N]] = [y[0]...y[N]]
				{
					agnes::table<Columns...> t;

					t.reserve(std::max(N, std::size_t{1}));

					t.emplace_back();

					assign(t);

					for (auto i = 0; (i < 1) && (i < N); ++i)
					{
						AGNES_REQUIRE(compare(t, i));
						COLUMN_A_REQUIRE(t[i].a.actions.equals(
							{class_action::defaultConstructed, class_action::copyAssigned}));
						COLUMN_B_REQUIRE(t[i].b.actions.equals(
							{class_action::defaultConstructed, class_action::copyAssigned}));
					}

					for (auto i = 1; i < N; ++i)
					{
						AGNES_REQUIRE(compare(t, i));
						COLUMN_A_REQUIRE(t[i].a.actions.equals({class_action::copyConstructed}));
						COLUMN_B_REQUIRE(t[i].b.actions.equals({class_action::copyConstructed}));
					}
				}

				return true;
			};

			const auto assignToVector = [&test]
			{
				const filled_rows<Types, Args, N> otherInitial;

				if constexpr (N == 0)
				{
					const agnes::table<Columns...> other;

					const auto assign  = [&other](auto& t) { t = other; };
					const auto compare = [&other](const auto& t, const auto i)
					{
						return COLUMN_A_COND(t[i].a == other[i].a) && COLUMN_B_COND(t[i].b == other[i].b);
					};

					return test(assign, compare);
				}
				else
				{
					const agnes::table<Columns...> other{otherInitial[I]...};

					const auto assign  = [&other](auto& t) { t = other; };
					const auto compare = [&other](const auto& t, const auto i)
					{
						return COLUMN_A_COND(t[i].a == other[i].a) && COLUMN_B_COND(t[i].b == other[i].b);
					};

					return test(assign, compare);
				}
			};

			const auto assignToInitializerList = [&test]
			{
				const filled_rows<Types, Args, N> initializing;

				const auto operatorAssign = [&initializing](auto& t) { t = {initializing[I]...}; };
				const auto functionAssign = [&initializing](auto& t) { t.assign({initializing[I]...}); };
				const auto compare        = [&initializing](const auto& t, const auto i)
				{
					return COLUMN_A_COND(t[i].a == get<0>(initializing[i])) &&
						COLUMN_B_COND(t[i].b == get<1>(initializing[i]));
				};

				return test(operatorAssign, compare) && test(functionAssign, compare);
			};

			const auto assignToIterators = [&test]
			{
				const filled_columns<Types, Args, N> iterated;

				const agnes::pointer first{get<Ti>(iterated).data()...};
				const agnes::pointer last {first + N};

				const auto assign  = [&first, &last](auto& t) { t.assign(first, last); };
				const auto compare = [&iterated](const auto& t, const auto i)
				{
					return COLUMN_A_COND(t[i].a == get<0>(iterated)[i]) &&
						COLUMN_B_COND(t[i].b == get<1>(iterated)[i]);
				};

				return test(assign, compare);
			};

			const auto assignToCopies = [&test]
			{
				const Types copied{filled_value<Types, Args>{}};

				const auto directAssign = [&copied](auto& t)
				{
					std::apply([&t](const auto&... copied) { t.assign(N, copied...); }, copied);
				};

				const auto referenceAssign = [&copied](auto& t)
				{
					std::apply([&t](const auto&... copied)
					{
						t.assign(N, agnes::reference{copied...});
					}, copied);
				};

				const auto compare = [&copied](const auto& t, const auto i)
				{
					return COLUMN_A_COND(t[i].a == get<0>(copied)) &&
						COLUMN_B_COND(t[i].b == get<1>(copied));
				};

				return test(directAssign, compare) && test(referenceAssign, compare);
			};

			return assignToVector() && assignToInitializerList() && assignToIterators() && assignToCopies();
		};
		
		return (test(std::make_index_sequence<std::tuple_size_v<ColumnTypes>>{},
			static_cast<ColumnTypes*>(nullptr),
			std::make_index_sequence<Counts>{},
			std::integral_constant<std::size_t, Counts>{}) && ...);
	};

	AGNES_RUN(test);
}

TEMPLATE_TEST_CASE_SIG("agnes::table move assignment", "",
	((typename ColumnTypes, typename Args, std::size_t... Counts), ColumnTypes, Args, Counts...),
	((std::tuple<column_a<recorded<int, -1>>                             >), (std::tuple<int     >), 0, 1, 2),
	((std::tuple<column_a<recorded<int, -1>>, column_b<recorded<int, -2>>>), (std::tuple<int, int>), 0, 1, 2))
{
	static constexpr auto test = [] { return ([]<std::size_t... Ti, typename... Columns, std::size_t N>(
		std::index_sequence<Ti...>,
		std::tuple<Columns...>*,
		std::integral_constant<std::size_t, N>)
	{
		using Types = std::tuple<agnes::column_element_t<Columns>...>;

		const filled_rows<Types, Args, N> otherInitial;

		agnes::table<Columns...> other   {otherInitial[Ti]...};
		const agnes::pointer     prevData{other.data()};

		agnes::table<Columns...> t;
		
		t = std::move(other);

		return true;

	}(std::make_index_sequence<Counts>{},
		static_cast<ColumnTypes*>(nullptr),
		std::integral_constant<std::size_t, Counts>{}) && ...); };

	AGNES_RUN(test);
}

TEMPLATE_TEST_CASE_SIG("agnes::table move assignment (user-defined allocator)", "",
	((typename Allocator, typename OtherAllocator, typename ColumnTypes, typename Args, std::size_t... Counts), Allocator, OtherAllocator, ColumnTypes, Args, Counts...),
	((user_defined_allocator_pocma                    <recorded<int, -1>                   >), (user_defined_allocator_pocma                    <recorded<int, -1>                   >), (std::tuple<column_a<recorded<int, -1>>                             >), (std::tuple<int     >), 0, 1, 2),
	((user_defined_allocator_pocma                    <recorded<int, -1>, recorded<int, -2>>), (user_defined_allocator_pocma                    <recorded<int, -1>, recorded<int, -2>>), (std::tuple<column_a<recorded<int, -1>>, column_b<recorded<int, -2>>>), (std::tuple<int, int>), 0, 1, 2),
	((user_defined_allocator_no_pocma_always_equal    <recorded<int, -1>                   >), (user_defined_allocator_no_pocma_always_equal    <recorded<int, -1>                   >), (std::tuple<column_a<recorded<int, -1>>                             >), (std::tuple<int     >), 0, 1, 2),
	((user_defined_allocator_no_pocma_always_equal    <recorded<int, -1>, recorded<int, -2>>), (user_defined_allocator_no_pocma_always_equal    <recorded<int, -1>, recorded<int, -2>>), (std::tuple<column_a<recorded<int, -1>>, column_b<recorded<int, -2>>>), (std::tuple<int, int>), 0, 1, 2),
	((user_defined_allocator_no_pocma_not_always_equal<recorded<int, -1>                   >), (user_defined_allocator_no_pocma_not_always_equal<recorded<int, -1>                   >), (std::tuple<column_a<recorded<int, -1>>                             >), (std::tuple<int     >), 0, 1, 2),
	((user_defined_allocator_no_pocma_not_always_equal<recorded<int, -1>, recorded<int, -2>>), (user_defined_allocator_no_pocma_not_always_equal<recorded<int, -1>, recorded<int, -2>>), (std::tuple<column_a<recorded<int, -1>>, column_b<recorded<int, -2>>>), (std::tuple<int, int>), 0, 1, 2))
{
	static constexpr auto test = [] { return ([]<std::size_t... Ti, typename... Columns, std::size_t N>(
		std::index_sequence<Ti...>,
		std::tuple<Columns...>*,
		std::integral_constant<std::size_t, N>)
	{
		using Types = std::tuple<agnes::column_element_t<Columns>...>;

		const filled_rows<Types, Args, N> otherInitial;

		agnes::table<Columns...> other   {otherInitial[Ti]...};
		const agnes::pointer     prevData{other.data()};

		agnes::table<Columns...> t;
		
		t = std::move(other);

		return true;

	}(std::make_index_sequence<Counts>{},
		static_cast<ColumnTypes*>(nullptr),
		std::integral_constant<std::size_t, Counts>{}) && ...); };

	AGNES_RUN(test);
}

TEMPLATE_TEST_CASE_SIG("agnes::table iterators", "",
	((typename ColumnTypes, typename Args, std::size_t... SubsetIndex), ColumnTypes, Args, SubsetIndex...),
	((std::tuple<column_a<recorded<int, -1>>                                                          >), (std::tuple<int          >), 0   ),
	((std::tuple<column_a<recorded<int, -1>>, column_b<recorded<int, -2>>                             >), (std::tuple<int, int     >), 1   ),
	((std::tuple<column_a<recorded<int, -1>>, column_b<recorded<int, -2>>, column_c<recorded<int, -3>>>), (std::tuple<int, int, int>), 0, 2))
{
	static constexpr auto test = [] { return []<typename... Columns, typename... Subset>(
		std::tuple<Columns...>*,
		std::tuple<Subset...>*)
	{
		using Types = std::tuple<agnes::column_element_t<Columns>...>;

		{
			agnes::table<Columns...> t;

			AGNES_REQUIRE(t.begin() == t.end());
			AGNES_REQUIRE(t.begin() == t.cend());
			AGNES_REQUIRE(t.cbegin() == t.cend());

			cbegin<Subset...>(t);

			AGNES_REQUIRE(begin<Subset...>(t) == end<Subset...>(t));
			AGNES_REQUIRE(begin<Subset...>(t) == cend<Subset...>(t));
			AGNES_REQUIRE(cbegin<Subset...>(t) == cend<Subset...>(t));
		}

		{
			const filled_rows<Types, Args, 4> initial;

			agnes::table<Columns...> t{initial[0], initial[1], initial[2], initial[3]};

			AGNES_REQUIRE(t.begin() + 4 == t.end());
			AGNES_REQUIRE(t.begin() + 4 == t.cend());
			AGNES_REQUIRE(t.cbegin() + 4 == t.cend());

			AGNES_REQUIRE(begin<Subset...>(t) + 4 == end<Subset...>(t));
			AGNES_REQUIRE(begin<Subset...>(t) + 4 == cend<Subset...>(t));
			AGNES_REQUIRE(cbegin<Subset...>(t) + 4 == cend<Subset...>(t));

			std::size_t i = 0;

			for (auto iter = t.begin(); iter != t.end(); ++iter)
			{
				COLUMN_A_REQUIRE(iter->a == get<0>(initial[i]));
				COLUMN_B_REQUIRE(iter->b == get<1>(initial[i]));
				COLUMN_C_REQUIRE(iter->c == get<2>(initial[i]));

				++i;
			}

			i = 0;

			for (const auto row : t)
			{
				COLUMN_A_REQUIRE(row.a == get<0>(initial[i]));
				COLUMN_B_REQUIRE(row.b == get<1>(initial[i]));
				COLUMN_C_REQUIRE(row.c == get<2>(initial[i]));

				++i;
			}

			// TODO A way to test iteration over a subset.

			for (auto iter = begin<Subset...>(t); iter != end<Subset...>(t); ++iter) {}

			for (const auto row : subview<Subset...>(t)) {}
		}

		return true;

	}(static_cast<ColumnTypes*>(nullptr),
		static_cast<std::tuple<std::tuple_element_t<SubsetIndex, ColumnTypes>...>*>(nullptr)); };

	AGNES_RUN(test);
}

TEMPLATE_TEST_CASE("agnes::table::resize()", "",
	(std::tuple<std::tuple<column_a<recorded<int>>                             >, std::tuple<int     >>),
	(std::tuple<std::tuple<column_a<recorded<int>>, column_b<recorded<int, -2>>>, std::tuple<int, int>>))
{
	using ColumnTypes = std::tuple_element_t<0, TestType>;
	using Args        = std::tuple_element_t<1, TestType>;

	static constexpr auto test = [] { return []<std::size_t... Ti, typename... Columns>(
		std::index_sequence<Ti...>,
		std::tuple<Columns...>*)
	{
		using Types = std::tuple<agnes::column_element_t<Columns>...>;

		const auto test = [](const auto& resize, const auto& compare)
		{
			const filled_rows<Types, Args, 3> initial;

			// [] -> [{}]
			{
				agnes::table<Columns...> t;

				const class_action expectedInsertAction{resize(t, 1)};

				AGNES_REQUIRE(t.size() == 1);

				AGNES_REQUIRE(compare(t[0]));

				COLUMN_A_REQUIRE(t[0].a.actions.equals({expectedInsertAction}));
				COLUMN_B_REQUIRE(t[0].b.actions.equals({expectedInsertAction}));
				COLUMN_C_REQUIRE(t[0].c.actions.equals({expectedInsertAction}));
			}

			// [initial[0]] -> [initial[0], {}]
			{
				agnes::table<Columns...> t{initial[0]};

				const class_action expectedInsertAction{resize(t, 2)};

				AGNES_REQUIRE(t.size() == 2);

				COLUMN_A_REQUIRE(t[0].a == get<0>(initial[0]));
				COLUMN_B_REQUIRE(t[0].b == get<1>(initial[0]));
				COLUMN_C_REQUIRE(t[0].c == get<2>(initial[0]));

				AGNES_REQUIRE(compare(t[1]));

				COLUMN_A_REQUIRE(t[0].a.actions.equals({class_action::moveConstructed}));
				COLUMN_B_REQUIRE(t[0].b.actions.equals({class_action::moveConstructed}));
				COLUMN_C_REQUIRE(t[0].c.actions.equals({class_action::moveConstructed}));

				COLUMN_A_REQUIRE(t[1].a.actions.equals({expectedInsertAction}));
				COLUMN_B_REQUIRE(t[1].b.actions.equals({expectedInsertAction}));
				COLUMN_C_REQUIRE(t[1].c.actions.equals({expectedInsertAction}));
			}

			// [initial[0], initial[1], initial[2]] -> [initial[0], initial[1], unused]
			{
				agnes::table<Columns...> t{initial[0], initial[1], initial[2]};

				const class_action expectedInsertAction{resize(t, 2)};

				AGNES_REQUIRE(t.size() == 2);

				COLUMN_A_REQUIRE(t[0].a == get<0>(initial[0]));
				COLUMN_B_REQUIRE(t[0].b == get<1>(initial[0]));
				COLUMN_C_REQUIRE(t[0].c == get<2>(initial[0]));

				COLUMN_A_REQUIRE(t[1].a == get<0>(initial[1]));
				COLUMN_B_REQUIRE(t[1].b == get<1>(initial[1]));
				COLUMN_C_REQUIRE(t[1].c == get<2>(initial[1]));

				COLUMN_A_REQUIRE(t[0].a.actions.equals({class_action::copyConstructed}));
				COLUMN_B_REQUIRE(t[0].b.actions.equals({class_action::copyConstructed}));
				COLUMN_C_REQUIRE(t[0].c.actions.equals({class_action::copyConstructed}));

				COLUMN_A_REQUIRE(t[1].a.actions.equals({class_action::copyConstructed}));
				COLUMN_B_REQUIRE(t[1].b.actions.equals({class_action::copyConstructed}));
				COLUMN_C_REQUIRE(t[1].c.actions.equals({class_action::copyConstructed}));
			}

			// [initial[0], unused, unused] -> [initial[0], {}, unused]
			{
				agnes::table<Columns...> t;

				t.reserve(3);

				t.emplace_back(initial[0]);

				const class_action expectedInsertAction{resize(t, 2)};

				AGNES_REQUIRE(t.size() == 2);

				COLUMN_A_REQUIRE(t[0].a == get<0>(initial[0]));
				COLUMN_B_REQUIRE(t[0].b == get<1>(initial[0]));
				COLUMN_C_REQUIRE(t[0].c == get<2>(initial[0]));

				AGNES_REQUIRE(compare(t[1]));

				COLUMN_A_REQUIRE(t[0].a.actions.equals({class_action::copyConstructed}));
				COLUMN_B_REQUIRE(t[0].b.actions.equals({class_action::copyConstructed}));
				COLUMN_C_REQUIRE(t[0].c.actions.equals({class_action::copyConstructed}));

				COLUMN_A_REQUIRE(t[1].a.actions.equals({expectedInsertAction}));
				COLUMN_B_REQUIRE(t[1].b.actions.equals({expectedInsertAction}));
				COLUMN_C_REQUIRE(t[1].c.actions.equals({expectedInsertAction}));
			}

			return true;
		};

		const auto resizeFillDefault = [&test]
		{
			const auto resize = [](auto& t, const auto n)
			{
				t.resize(n);

				return class_action::defaultConstructed;
			};

			const auto compare = [](const auto& row)
			{
				const Types expected;

				return COLUMN_A_COND(row.a == get<0>(expected)) &&
					COLUMN_B_COND(row.b == get<1>(expected)) &&
					COLUMN_C_COND(row.c == get<2>(expected));
			};

			return test(resize, compare);
		};

		const auto resizeFillCopy = [&test]
		{
			const Types inserted{filled_value<Types, Args>{3}};

			const auto resize = [&inserted](auto& t, const auto n)
			{
				t.resize(n, get<Ti>(inserted)...);

				return class_action::copyConstructed;
			};

			const auto compare = [&inserted](const auto& row)
			{
				return COLUMN_A_COND(row.a == get<0>(inserted)) &&
					COLUMN_B_COND(row.b == get<1>(inserted)) &&
					COLUMN_C_COND(row.c == get<2>(inserted));
			};

			return test(resize, compare);
		};

		return resizeFillDefault() && resizeFillCopy();

	}(std::make_index_sequence<std::tuple_size_v<ColumnTypes>>{}, static_cast<ColumnTypes*>(nullptr)); };

	AGNES_RUN(test);
}

TEMPLATE_TEST_CASE("agnes::table::reserve()", "",
	(std::tuple<std::tuple<column_a<recorded<int>>                             >, std::tuple<int     >>),
	(std::tuple<std::tuple<column_a<recorded<int>>, column_b<recorded<int, -2>>>, std::tuple<int, int>>))
{
	using ColumnTypes = std::tuple_element_t<0, TestType>;
	using Args        = std::tuple_element_t<1, TestType>;

	static constexpr auto test = [] { return []<typename... Columns>(std::tuple<Columns...>*)
	{
		using Types = std::tuple<agnes::column_element_t<Columns>...>;

		const filled_rows<Types, Args, 2> initial;

		// [] -> [unused]
		{
			agnes::table<Columns...> t;

			t.reserve(1);

			AGNES_REQUIRE(t.size() == 0);
			AGNES_REQUIRE(t.capacity() == 1);
		}

		// [initial[0], initial[1]] -> [initial[0], initial[1], unused]
		{
			agnes::table<Columns...> t{initial[0], initial[1]};

			t.reserve(3);

			AGNES_REQUIRE(t.size() == 2);
			AGNES_REQUIRE(t.capacity() == 3);

			COLUMN_A_REQUIRE(t[0].a == get<0>(initial[0]));
			COLUMN_B_REQUIRE(t[0].b == get<1>(initial[0]));
			COLUMN_C_REQUIRE(t[0].c == get<2>(initial[0]));

			COLUMN_A_REQUIRE(t[1].a == get<0>(initial[1]));
			COLUMN_B_REQUIRE(t[1].b == get<1>(initial[1]));
			COLUMN_C_REQUIRE(t[1].c == get<2>(initial[1]));

			COLUMN_A_REQUIRE(t[0].a.actions.equals({class_action::moveConstructed}));
			COLUMN_B_REQUIRE(t[0].b.actions.equals({class_action::moveConstructed}));
			COLUMN_C_REQUIRE(t[0].c.actions.equals({class_action::moveConstructed}));

			COLUMN_A_REQUIRE(t[1].a.actions.equals({class_action::moveConstructed}));
			COLUMN_B_REQUIRE(t[1].b.actions.equals({class_action::moveConstructed}));
			COLUMN_C_REQUIRE(t[1].c.actions.equals({class_action::moveConstructed}));
		}

		// [initial[0], initial[1]] -> [initial[0], initial[1]]
		{
			agnes::table<Columns...> t{initial[0], initial[1]};

			t.reserve(2);

			AGNES_REQUIRE(t.size() == 2);
			AGNES_REQUIRE(t.capacity() == 2);

			t.reserve(0);

			AGNES_REQUIRE(t.size() == 2);
			AGNES_REQUIRE(t.capacity() == 2);
		}

		return true;

	}(static_cast<ColumnTypes*>(nullptr)); };

	AGNES_RUN(test);
}

TEMPLATE_TEST_CASE("agnes::table::shrink_to_fit", "",
	(std::tuple<std::tuple<column_a<recorded<int>>                         >, std::tuple<int     >>),
	(std::tuple<std::tuple<column_a<recorded<int>>, column_b<recorded<int>>>, std::tuple<int, int>>))
{
	using ColumnTypes = std::tuple_element_t<0, TestType>;
	using Args        = std::tuple_element_t<1, TestType>;

	static constexpr auto test = [] { return []<typename... Columns>(std::tuple<Columns...>*)
	{
		using Types = std::tuple<agnes::column_element_t<Columns>...>;

		const Types initial{filled_value<Types, Args>{0}};

		// [initial[0], unused] -> [initial[0]]
		{
			agnes::table<Columns...> t;

			t.reserve(2);

			t.emplace_back(initial);

			t.shrink_to_fit();

			AGNES_REQUIRE(t.size() == 1);
			AGNES_REQUIRE(t.capacity() == 1);
		}

		// [initial[0]] -> [initial[0]]
		{
			agnes::table<Columns...> t{initial};

			t.shrink_to_fit();

			AGNES_REQUIRE(t.size() == 1);
			AGNES_REQUIRE(t.capacity() == 1);
		}

		return true;

	}(static_cast<ColumnTypes*>(nullptr)); };

	AGNES_RUN(test);
}

TEMPLATE_TEST_CASE_SIG("agnes::table element access", "",
	((typename ColumnTypes, typename Args, std::size_t... SubsetIndex), ColumnTypes, Args, SubsetIndex...),
	((std::tuple<column_a<recorded<int, -1>>                                                          >), (std::tuple<int          >), 0   ),
	((std::tuple<column_a<recorded<int, -1>>, column_b<recorded<int, -2>>                             >), (std::tuple<int, int     >), 1   ),
	((std::tuple<column_a<recorded<int, -1>>, column_b<recorded<int, -2>>, column_c<recorded<int, -3>>>), (std::tuple<int, int, int>), 0, 2))
{
	static constexpr auto test = [] { return []<typename... Columns, typename... Subset>(
		std::tuple<Columns...>*,
		std::tuple<Subset...>*)
	{
		using Types = std::tuple<agnes::column_element_t<Columns>...>;

		{
			agnes::table<Columns...> t;

			AGNES_REQUIRE(t.data() == nullptr);
		}

		{
			const filled_rows<Types, Args, 4> initial;

			agnes::table<Columns...> t{initial[0], initial[1], initial[2], initial[3]};

			COLUMN_A_REQUIRE(&t[0].a      == &t.begin()->a);
			COLUMN_B_REQUIRE(&t[0].b      == &t.begin()->b);
			COLUMN_C_REQUIRE(&t[0].c      == &t.begin()->c);
			
			COLUMN_A_REQUIRE(&t.at(0).a   == &t.begin()->a);
			COLUMN_B_REQUIRE(&t.at(0).b   == &t.begin()->b);
			COLUMN_C_REQUIRE(&t.at(0).c   == &t.begin()->c);
			
			COLUMN_A_REQUIRE(&t.front().a == &t.begin()->a);
			COLUMN_B_REQUIRE(&t.front().b == &t.begin()->b);
			COLUMN_C_REQUIRE(&t.front().c == &t.begin()->c);
			
			COLUMN_A_REQUIRE(&t.back().a  == &(t.end() - 1)->a);
			COLUMN_B_REQUIRE(&t.back().b  == &(t.end() - 1)->b);
			COLUMN_C_REQUIRE(&t.back().c  == &(t.end() - 1)->c);
			
			COLUMN_A_REQUIRE(get<0>(t.data())   == &t.begin()->a);
			COLUMN_B_REQUIRE(get<1>(t.data())   == &t.begin()->b);
			COLUMN_C_REQUIRE(get<2>(t.data())   == &t.begin()->c);

			for (auto i = std::size_t{1}; i < t.size(); ++i)
			{
				COLUMN_A_REQUIRE(&t[i].a          == &(t.begin() + i)->a);
				COLUMN_B_REQUIRE(&t[i].b          == &(t.begin() + i)->b);
				COLUMN_C_REQUIRE(&t[i].c          == &(t.begin() + i)->c);
				
				COLUMN_A_REQUIRE(&t.at(i).a       == &(t.begin() + i)->a);
				COLUMN_B_REQUIRE(&t.at(i).b       == &(t.begin() + i)->b);
				COLUMN_C_REQUIRE(&t.at(i).c       == &(t.begin() + i)->c);
				
				COLUMN_A_REQUIRE(get<0>(t.data() + i) == &(t.begin() + i)->a);
				COLUMN_B_REQUIRE(get<1>(t.data() + i) == &(t.begin() + i)->b);
				COLUMN_C_REQUIRE(get<2>(t.data() + i) == &(t.begin() + i)->c);
			}

			// TODO A way to test subset addresses.

			AGNES_REQUIRE(at<Subset...>(t, 0) == *begin<Subset...>(t));
			AGNES_REQUIRE(at<Subset...>(t, 0) == *begin<Subset...>(t));
			AGNES_REQUIRE(at<Subset...>(t, 0) == *begin<Subset...>(t));
			
			AGNES_REQUIRE(front<Subset...>(t) == *begin<Subset...>(t));
			AGNES_REQUIRE(front<Subset...>(t) == *begin<Subset...>(t));
			AGNES_REQUIRE(front<Subset...>(t) == *begin<Subset...>(t));
			
			AGNES_REQUIRE(back<Subset...>(t)  == *(end<Subset...>(t) - 1));
			AGNES_REQUIRE(back<Subset...>(t)  == *(end<Subset...>(t) - 1));
			AGNES_REQUIRE(back<Subset...>(t)  == *(end<Subset...>(t) - 1));
			
			for (auto i = std::size_t{1}; i < t.size(); ++i)
			{
				AGNES_REQUIRE(at<Subset...>(t, i) == *(begin<Subset...>(t) + i));
				AGNES_REQUIRE(at<Subset...>(t, i) == *(begin<Subset...>(t) + i));
				AGNES_REQUIRE(at<Subset...>(t, i) == *(begin<Subset...>(t) + i));
			}
		}

		return true;

	}(static_cast<ColumnTypes*>(nullptr),
		static_cast<std::tuple<std::tuple_element_t<SubsetIndex, ColumnTypes>...>*>(nullptr)); };

	AGNES_RUN(test);
}

TEMPLATE_TEST_CASE("agnes::table::emplace_back() and push_back()", "",
	(std::tuple<std::tuple<column_a<recorded<int>>                                      >, std::tuple<int     >>),
	(std::tuple<std::tuple<column_a<recorded<int>>, column_b<not_noexcept_moveable<int>>>, std::tuple<int, int>>))
{
	using ColumnTypes = std::tuple_element_t<0, TestType>;
	using Args        = std::tuple_element_t<1, TestType>;

	static constexpr auto test = [] { return []<typename... Columns>(std::tuple<Columns...>*)
	{
		using Types = std::tuple<agnes::column_element_t<Columns>...>;

		const auto Index = [](const std::size_t i) { return i%sizeof...(Columns); };

		const auto test = [](const auto& emplaceOrPushBack, const auto& compare)
		{
			const Types initial{filled_value<Types, Args>{0}};

			const class_action expectedReallocatingAction[]{
				(std::is_nothrow_move_constructible_v<agnes::column_element_t<Columns>> ?
					class_action::moveConstructed : class_action::copyConstructed)...};

			// [] -> [inserted]
			{
				agnes::table<Columns...> t;

				const class_action expectedInsertAction{emplaceOrPushBack(t)};

				AGNES_REQUIRE(t.size() == 1);

				AGNES_REQUIRE(compare(t[0]));

				COLUMN_A_REQUIRE(t[0].a.actions.equals({expectedInsertAction}));
				COLUMN_B_REQUIRE(t[0].b.actions.equals({expectedInsertAction}));
			}

			// [initial[0]] -> [initial[0], inserted]
			{
				agnes::table<Columns...> t{initial};

				const class_action expectedInsertAction{emplaceOrPushBack(t)};

				AGNES_REQUIRE(t.size() == 2);

				COLUMN_A_REQUIRE(t[0].a == get<0>(initial));
				COLUMN_B_REQUIRE(t[0].b == get<1>(initial));

				AGNES_REQUIRE(compare(t[1]));

				COLUMN_A_REQUIRE(t[0].a.actions.equals({expectedReallocatingAction[0]}));
				COLUMN_B_REQUIRE(t[0].b.actions.equals({expectedReallocatingAction[1]}));

				COLUMN_A_REQUIRE(t[1].a.actions.equals({expectedInsertAction         }));
				COLUMN_B_REQUIRE(t[1].b.actions.equals({expectedInsertAction         }));
			}

			// [unused] -> [inserted]
			{
				agnes::table<Columns...> t;

				t.reserve(1);

				const class_action expectedInsertAction{emplaceOrPushBack(t)};

				AGNES_REQUIRE(t.size() == 1);

				AGNES_REQUIRE(compare(t[0]));

				COLUMN_A_REQUIRE(t[0].a.actions.equals({expectedInsertAction}));
				COLUMN_B_REQUIRE(t[0].b.actions.equals({expectedInsertAction}));
			}

			// [initial[0], unused] -> [initial[0], inserted]
			{
				agnes::table<Columns...> t;

				t.reserve(2);

				t.emplace_back(initial);

				const class_action expectedInsertAction{emplaceOrPushBack(t)};

				AGNES_REQUIRE(t.size() == 2);

				COLUMN_A_REQUIRE(t[0].a == get<0>(initial));
				COLUMN_B_REQUIRE(t[0].b == get<1>(initial));

				AGNES_REQUIRE(compare(t[1]));

				COLUMN_A_REQUIRE(t[0].a.actions.equals({class_action::copyConstructed}));
				COLUMN_B_REQUIRE(t[0].b.actions.equals({class_action::copyConstructed}));

				COLUMN_A_REQUIRE(t[1].a.actions.equals({expectedInsertAction         }));
				COLUMN_B_REQUIRE(t[1].b.actions.equals({expectedInsertAction         }));
			}

			return true;
		};

		const auto emplaceBackDirect = [Index, &test]
		{
			const Args args{filled_value<Args , Args>{1}};

			const auto emplaceBackValues = [&args](auto& t)
			{
				std::apply([&t](const auto&... args) { t.emplace_back(args...); }, args);

				return class_action::directConstructed;
			};

			const auto emplaceBackReference = [&args](auto& t)
			{
				std::apply([&t](const auto&... args) { t.emplace_back(agnes::reference{args...}); }, args);

				return class_action::directConstructed;
			};

			const auto emplaceBackPiecewise = [&args](auto& t)
			{
				std::apply([&t](const auto&... args)
				{
					t.emplace_back(std::piecewise_construct, std::tuple{args}...);
				}, args);

				return class_action::directConstructed;
			};

			const auto compare = [Index, &args](const auto& row)
			{
				return COLUMN_A_COND(row.a == get<Index(0)>(args)) && COLUMN_B_COND(row.b == get<Index(1)>(args));
			};

			return test(emplaceBackValues, compare) &&
				test(emplaceBackReference, compare) &&
				test(emplaceBackPiecewise, compare);
		};

		const auto emplaceAndPushBackCopyAndMove = [Index, &test]
		{
			Types inserted{filled_value<Types, Args>{1}};

			const auto emplaceBackCopy = [&inserted](auto& t)
			{
				t.emplace_back(inserted);

				return class_action::copyConstructed;
			};

			const auto emplaceBackMove = [&inserted](auto& t)
			{
				t.emplace_back(std::move(inserted));

				return class_action::moveConstructed;
			};

			const auto pushBackCopy = [&inserted](auto& t)
			{
				std::apply([&t](const auto&... inserted) { t.push_back(inserted...); }, inserted);

				return class_action::copyConstructed;
			};

			const auto pushBackMove = [&inserted](auto& t)
			{
				std::apply([&t](auto&... inserted) { t.push_back(std::move(inserted)...); }, inserted);

				return class_action::moveConstructed;
			};

			const auto compare = [Index, &inserted](const auto& row)
			{
				return COLUMN_A_COND(row.a == get<Index(0)>(inserted)) && COLUMN_B_COND(row.b == get<Index(1)>(inserted));
			};

			return test(emplaceBackCopy, compare)
				&& test(emplaceBackMove, compare)
				&& test(pushBackCopy, compare)
				&& test(pushBackMove, compare);
		};

		return emplaceBackDirect() && emplaceAndPushBackCopyAndMove();

	}(static_cast<ColumnTypes*>(nullptr)); };

	AGNES_RUN(test);
}

TEMPLATE_TEST_CASE("agnes::table::pop_back()", "",
	(std::tuple<std::tuple<column_a<recorded<int>>                         >, std::tuple<int     >>),
	(std::tuple<std::tuple<column_a<recorded<int>>, column_b<recorded<int>>>, std::tuple<int, int>>))
{
	using ColumnTypes = std::tuple_element_t<0, TestType>;
	using Args        = std::tuple_element_t<1, TestType>;

	static constexpr auto test = [] { return []<typename... Columns>(std::tuple<Columns...>*)
	{
		using Types = std::tuple<agnes::column_element_t<Columns>...>;

		const filled_rows<Types, Args, 2> initial;

		// [initial[0], initial[1]] -> [initial[0], unused]
		{
			agnes::table<Columns...> t{{initial[0], initial[1]}};

			t.pop_back();

			AGNES_REQUIRE(t.size() == 1);

			COLUMN_A_REQUIRE(t[0].a == get<0>(initial[0]));
			COLUMN_B_REQUIRE(t[0].b == get<1>(initial[0]));
		}

		return true;

	}(static_cast<ColumnTypes*>(nullptr)); };

	AGNES_RUN(test);
}

TEMPLATE_TEST_CASE("agnes::table::emplace() and insert() single", "",
	(std::tuple<std::tuple<column_a<recorded<int>>                                      >, std::tuple<int     >>),
	(std::tuple<std::tuple<column_a<recorded<int>>, column_b<not_noexcept_moveable<int>>>, std::tuple<int, int>>))
{
	using ColumnTypes = std::tuple_element_t<0, TestType>;
	using Args        = std::tuple_element_t<1, TestType>;

	static constexpr auto test = [] { return []<std::size_t... Ti, typename... Columns>(
		std::index_sequence<Ti...>,
		std::tuple<Columns...>*)
	{
		using Types = std::tuple<agnes::column_element_t<Columns>...>;

		const auto Index = [](const std::size_t i) { return i%sizeof...(Columns); };

		const auto test = [](const auto& emplaceOrInsert, const auto& compare)
		{
			const filled_rows<Types, Args, 2> initial;

			const class_action expectedReallocatingAction[]{
				(std::is_nothrow_move_constructible_v<agnes::column_element_t<Columns>> ?
					class_action::moveConstructed : class_action::copyConstructed)...};

			// [] -> [inserted]
			{
				agnes::table<Columns...> t;

				const class_action expectedInsertAction{emplaceOrInsert(t, t.end())};

				AGNES_REQUIRE(t.size() == 1);

				AGNES_REQUIRE(compare(t[0]));

				COLUMN_A_REQUIRE(t[0].a.actions.equals({expectedInsertAction}));
				COLUMN_B_REQUIRE(t[0].b.actions.equals({expectedInsertAction}));
			}

			// [initial[0]] -> [inserted, initial[0]]
			{
				agnes::table<Columns...> t{initial[0]};

				const class_action expectedInsertAction{emplaceOrInsert(t, t.begin())};

				AGNES_REQUIRE(t.size() == 2);

				AGNES_REQUIRE(compare(t[0]));

				COLUMN_A_REQUIRE(t[1].a == get<0>(initial[0]));
				COLUMN_B_REQUIRE(t[1].b == get<1>(initial[0]));

				COLUMN_A_REQUIRE(t[0].a.actions.equals({expectedInsertAction         }));
				COLUMN_B_REQUIRE(t[0].b.actions.equals({expectedInsertAction         }));

				COLUMN_A_REQUIRE(t[1].a.actions.equals({expectedReallocatingAction[0]}));
				COLUMN_B_REQUIRE(t[1].b.actions.equals({expectedReallocatingAction[1]}));
			}

			// [initial[0], initial[1]] -> [initial[0], inserted, initial[1]]
			{
				agnes::table<Columns...> t{initial[0], initial[1]};

				const class_action expectedInsertAction{emplaceOrInsert(t, t.begin() + 1)};

				AGNES_REQUIRE(t.size() == 3);

				COLUMN_A_REQUIRE(t[0].a == get<0>(initial[0]));
				COLUMN_B_REQUIRE(t[0].b == get<1>(initial[0]));

				AGNES_REQUIRE(compare(t[1]));

				COLUMN_A_REQUIRE(t[2].a == get<0>(initial[1]));
				COLUMN_B_REQUIRE(t[2].b == get<1>(initial[1]));

				COLUMN_A_REQUIRE(t[0].a.actions.equals({expectedReallocatingAction[0]}));
				COLUMN_B_REQUIRE(t[0].b.actions.equals({expectedReallocatingAction[1]}));

				COLUMN_A_REQUIRE(t[1].a.actions.equals({expectedInsertAction         }));
				COLUMN_B_REQUIRE(t[1].b.actions.equals({expectedInsertAction         }));

				COLUMN_A_REQUIRE(t[2].a.actions.equals({expectedReallocatingAction[0]}));
				COLUMN_B_REQUIRE(t[2].b.actions.equals({expectedReallocatingAction[1]}));
			}

			// [initial[0]] -> [initial[0], inserted]
			{
				agnes::table<Columns...> t{initial[0]};

				const class_action expectedInsertAction{emplaceOrInsert(t, t.end())};

				AGNES_REQUIRE(t.size() == 2);

				COLUMN_A_REQUIRE(t[0].a == get<0>(initial[0]));
				COLUMN_B_REQUIRE(t[0].b == get<1>(initial[0]));

				AGNES_REQUIRE(compare(t[1]));

				COLUMN_A_REQUIRE(t[0].a.actions.equals({expectedReallocatingAction[0]}));
				COLUMN_B_REQUIRE(t[0].b.actions.equals({expectedReallocatingAction[1]}));

				COLUMN_A_REQUIRE(t[1].a.actions.equals({expectedInsertAction         }));
				COLUMN_B_REQUIRE(t[1].b.actions.equals({expectedInsertAction         }));
			}

			// [unused] -> [inserted]
			{
				agnes::table<Columns...> t;

				t.reserve(1);

				const class_action expectedInsertAction{emplaceOrInsert(t, t.end())};

				AGNES_REQUIRE(t.size() == 1);

				AGNES_REQUIRE(compare(t[0]));

				COLUMN_A_REQUIRE(t[0].a.actions.equals({expectedInsertAction}));
				COLUMN_B_REQUIRE(t[0].b.actions.equals({expectedInsertAction}));
			}

			// [initial[0], unused] -> [inserted, initial[0]]
			{
				agnes::table<Columns...> t;

				t.reserve(2);

				t.emplace_back(initial[0]);

				const class_action expectedInsertAction{emplaceOrInsert(t, t.begin())};

				AGNES_REQUIRE(t.size() == 2);

				AGNES_REQUIRE(compare(t[0]));

				COLUMN_A_REQUIRE(t[1].a == get<0>(initial[0]));
				COLUMN_B_REQUIRE(t[1].b == get<1>(initial[0]));

				COLUMN_A_REQUIRE(t[0].a.actions.equals({class_action::copyConstructed, class_action::moveAssigned}));
				COLUMN_B_REQUIRE(t[0].b.actions.equals({class_action::copyConstructed, class_action::moveAssigned}));

				COLUMN_A_REQUIRE(t[1].a.actions.equals({class_action::moveConstructed                            }));
				COLUMN_B_REQUIRE(t[1].b.actions.equals({class_action::moveConstructed                            }));
			}

			// [initial[0], initial[1], unused] -> [initial[0], inserted, initial[1]]
			{
				agnes::table<Columns...> t;

				t.reserve(3);

				t.emplace_back(initial[0]);
				t.emplace_back(initial[1]);

				const class_action expectedInsertAction{emplaceOrInsert(t, t.begin() + 1)};

				AGNES_REQUIRE(t.size() == 3);

				COLUMN_A_REQUIRE(t[0].a == get<0>(initial[0]));
				COLUMN_B_REQUIRE(t[0].b == get<1>(initial[0]));

				AGNES_REQUIRE(compare(t[1]));

				COLUMN_A_REQUIRE(t[2].a == get<0>(initial[1]));
				COLUMN_B_REQUIRE(t[2].b == get<1>(initial[1]));

				COLUMN_A_REQUIRE(t[0].a.actions.equals({class_action::copyConstructed                            }));
				COLUMN_B_REQUIRE(t[0].b.actions.equals({class_action::copyConstructed                            }));

				COLUMN_A_REQUIRE(t[1].a.actions.equals({class_action::copyConstructed, class_action::moveAssigned}));
				COLUMN_B_REQUIRE(t[1].b.actions.equals({class_action::copyConstructed, class_action::moveAssigned}));
	
				COLUMN_A_REQUIRE(t[2].a.actions.equals({class_action::moveConstructed                            }));
				COLUMN_B_REQUIRE(t[2].b.actions.equals({class_action::moveConstructed                            }));
			}

			// [initial[0], unused] -> [initial[0], inserted]
			{
				agnes::table<Columns...> t;

				t.reserve(2);

				t.emplace_back(initial[0]);

				const class_action expectedInsertAction{emplaceOrInsert(t, t.end())};

				AGNES_REQUIRE(t.size() == 2);

				COLUMN_A_REQUIRE(t[0].a == get<0>(initial[0]));
				COLUMN_B_REQUIRE(t[0].b == get<1>(initial[0]));

				AGNES_REQUIRE(compare(t[1]));

				COLUMN_A_REQUIRE(t[0].a.actions.equals({class_action::copyConstructed }));
				COLUMN_B_REQUIRE(t[0].b.actions.equals({class_action::copyConstructed }));

				COLUMN_A_REQUIRE(t[1].a.actions.equals({expectedInsertAction          }));
				COLUMN_B_REQUIRE(t[1].b.actions.equals({expectedInsertAction          }));
			}

			return true;
		};

		const auto emplaceDirect = [Index, &test]
		{
			const Args args{filled_value<Args , Args>{1}};

			const auto emplaceValues = [&args](auto& t, const auto& position)
			{
				std::apply([&t, &position](const auto&... args) { t.emplace(position, args...); }, args);

				return class_action::directConstructed;
			};

			const auto emplaceReference = [&args](auto& t, const auto& position)
			{
				std::apply([&t, &position](const auto&... args)
				{
					t.emplace(position, agnes::reference{args...});
				}, args);

				return class_action::directConstructed;
			};

			const auto emplacePiecewise = [&args](auto& t, const auto& position)
			{
				std::apply([&t, &position](const auto&... args)
				{
					t.emplace(position,  std::piecewise_construct, std::tuple{args}...);
				}, args);

				return class_action::directConstructed;
			};

			const auto compare = [Index, &args](const auto& row)
			{
				return COLUMN_A_COND(row.a == get<Index(0)>(args)) && COLUMN_B_COND(row.b == get<Index(1)>(args));
			};

			return test(emplaceValues, compare) && test(emplaceReference, compare) && test(emplacePiecewise, compare);
		};

		const auto emplaceAndInsertCopyAndMove = [Index, &test]
		{
			Types inserted{filled_value<Types, Args>{1}};

			const auto emplaceCopy = [&inserted](auto& t, const auto& position)
			{
				t.emplace(position, inserted);

				return class_action::copyConstructed;
			};

			const auto emplaceMove = [&inserted](auto& t, const auto& position)
			{
				t.emplace(position, std::move(inserted));

				return class_action::moveConstructed;
			};

			const auto insertCopy = [&inserted](auto& t, const auto& position)
			{
				t.insert(position, get<Ti>(inserted)...);

				return class_action::copyConstructed;
			};

			const auto insertMove = [&inserted](auto& t, const auto& position)
			{
				t.insert(position, get<Ti>(std::move(inserted))...);

				return class_action::moveConstructed;
			};

			const auto insertFill = [&inserted]<std::size_t N>()
			{
				return [&inserted](auto& t, const auto& position)
				{
					t.insert(position, get<Ti>(inserted)...);

					return class_action::copyConstructed;
				};
			};

			const auto compare = [Index, &inserted](const auto& row)
			{
				return COLUMN_A_COND(row.a == get<Index(0)>(inserted)) && COLUMN_B_COND(row.b == get<Index(1)>(inserted));
			};

			return test(emplaceCopy, compare)
				&& test(emplaceMove, compare)
				&& test(insertCopy, compare)
				&& test(insertMove, compare)
				&& test(insertFill.template operator()<0>(), compare)
				&& test(insertFill.template operator()<1>(), compare)
				&& test(insertFill.template operator()<2>(), compare);
		};

		return emplaceDirect() && emplaceAndInsertCopyAndMove();

	}(std::make_index_sequence<std::tuple_size_v<ColumnTypes>>{}, static_cast<ColumnTypes*>(nullptr)); };

	AGNES_RUN(test);
}

TEMPLATE_TEST_CASE_SIG("agnes::table::insert() multiple", "",
	((typename ColumnTypes, typename Args, std::size_t N, std::size_t M), ColumnTypes, Args, N, M),
	((std::tuple<column_a<recorded<int>>                                      >), (std::tuple<int     >), 1, 1),
	((std::tuple<column_a<recorded<int>>                                      >), (std::tuple<int     >), 2, 2),
	((std::tuple<column_a<recorded<int>>                                      >), (std::tuple<int     >), 3, 2),
	((std::tuple<column_a<recorded<int>>, column_b<not_noexcept_moveable<int>>>), (std::tuple<int, int>), 1, 1),
	((std::tuple<column_a<recorded<int>>, column_b<not_noexcept_moveable<int>>>), (std::tuple<int, int>), 2, 2),
	((std::tuple<column_a<recorded<int>>, column_b<not_noexcept_moveable<int>>>), (std::tuple<int, int>), 3, 2))
{
	static constexpr auto test = [] { return []<
		std::size_t... Ti,
		typename... Columns,
		std::size_t... I,
		std::size_t... J>(
		std::index_sequence<Ti...>,
		std::tuple<Columns...>*,
		std::index_sequence<I...>,
		std::index_sequence<J...>)
	{
		using Types = std::tuple<agnes::column_element_t<Columns>...>;

		const auto test = [](const auto& insert, const auto& compare)
		{
			const filled_rows<Types, Args, N> initial;

			const class_action expectedReallocatingAction[]{
				(std::is_nothrow_move_constructible_v<agnes::column_element_t<Columns>> ?
					class_action::moveConstructed : class_action::copyConstructed)...};

			// [] -> []
			{
				agnes::table<Columns...> t;

				insert(t, t.end(), 0);

				AGNES_REQUIRE(t.size() == 0);
			}

			// [] -> [inserted[0]...inserted[M]]
			{
				agnes::table<Columns...> t;

				insert(t, t.end(), M);

				AGNES_REQUIRE(t.size() == M);

				AGNES_REQUIRE(compare(t, 0));
			}

			// [initial[0]...initial[N]] -> [inserted[0]...inserted[M], initial[0]...initial[N]]
			{
				agnes::table<Columns...> t{initial[I]...};

				insert(t, t.begin(), M);

				AGNES_REQUIRE(t.size() == N + M);

				AGNES_REQUIRE(compare(t, 0));
			}

			// [initial[0]...initial[N]] -> [initial[0]...initial[i], inserted[0]...inserted[M], initial[i]...initial[N]]
			for (auto i = std::size_t{1}; i < N; ++i)
			{
				agnes::table<Columns...> t{initial[I]...};

				insert(t, t.begin() + i, M);

				AGNES_REQUIRE(t.size() == N + M);

				AGNES_REQUIRE(compare(t, i));
			}

			// [initial[0]...initial[N]] -> [initial[0]...initial[N], inserted[0]...inserted[M]]
			{
				agnes::table<Columns...> t{initial[I]...};

				insert(t, t.end(), M);

				AGNES_REQUIRE(t.size() == N + M);

				AGNES_REQUIRE(compare(t, N));
			}

			// [unused[0]...unused[M]] -> [inserted[0]...inserted[M]]
			{
				agnes::table<Columns...> t;

				t.reserve(M);

				insert(t, t.end(), M);

				AGNES_REQUIRE(t.size() == M);

				AGNES_REQUIRE(compare(t, 0));
			}

			// [initial[0]...initial[N], unused[0]...unused[M]] -> [inserted[0]...inserted[M], initial[0]...initial[N]]
			{
				agnes::table<Columns...> t;

				t.reserve(N + M);

				t = {initial[I]...};

				insert(t, t.begin(), M);

				AGNES_REQUIRE(t.size() == N + M);

				AGNES_REQUIRE(compare(t, 0));
			}

			// [initial[0]...initial[N], unused[0]...unused[M]] -> [initial[0]...initial[i], inserted[0]...inserted[M], initial[i]...initial[N]]
			for (auto i = std::size_t{1}; i < N; ++i)
			{
				agnes::table<Columns...> t;

				t.reserve(N + M);

				t = {initial[I]...};

				insert(t, t.begin() + i, M);

				AGNES_REQUIRE(t.size() == N + M);

				AGNES_REQUIRE(compare(t, i));
			}

			// [initial[0]...initial[N], unused[0]...unused[M]] -> [initial[0]...initial[N], inserted[0]...inserted[M]]
			{
				agnes::table<Columns...> t;

				t.reserve(N + M);

				t = {initial[I]...};

				insert(t, t.end(), M);

				AGNES_REQUIRE(t.size() == N + M);

				AGNES_REQUIRE(compare(t, N));
			}

			return true;
		};

		const auto insertIterators = [&test]
		{
			const filled_columns<Types, Args, M> iterated;

			const auto insert = [&iterated](auto& t, const auto& position, const auto count)
			{
				const agnes::pointer first{get<Ti>(iterated).data()...};
				const agnes::pointer last {first + count};

				t.insert(position, first, last);

				return class_action::copyConstructed;
			};

			const auto compare = [&iterated](const auto& t, auto firstIndex)
			{
				for (auto i = std::size_t{0}; i < M; ++i)
				{
					if (!(COLUMN_A_COND(t[firstIndex + i].a == get<0>(iterated)[i]) &&
						COLUMN_B_COND(t[firstIndex + i].b == get<1>(iterated)[i])))
					{
						return false;
					}
				}

				return true;
			};

			return test(insert, compare);
		};

		const auto insertInitializerList = [&test]
		{
			const filled_rows<Types, Args, M> inserted;

			const auto insert = [&inserted](auto& t, const auto& position, const auto count)
			{
				if (count == 0)
				{
					t.insert(position, {});
				}
				else
				{
					t.insert(position, {inserted[J]...});
				}

				return class_action::copyConstructed;
			};

			const auto compare = [&inserted](const auto& t, const auto firstIndex)
			{
				const auto compare = [&inserted, &t, firstIndex](std::size_t k)
				{
					return COLUMN_A_COND(t[firstIndex + k].a == get<0>(inserted[k])) &&
						COLUMN_B_COND(t[firstIndex + k].b == get<1>(inserted[k]));
				};

				return (compare(J) && ...);
			};

			return test(insert, compare);
		};

		return insertIterators() && insertInitializerList();

	}(std::make_index_sequence<std::tuple_size_v<ColumnTypes>>{},
		static_cast<ColumnTypes*>(nullptr),
		std::make_index_sequence<N>{},
		std::make_index_sequence<M>{}); };

	AGNES_RUN(test);
}

TEMPLATE_TEST_CASE("agnes::table::erase() single", "",
	(std::tuple<std::tuple<column_a<recorded<int, -1>>                             >, std::tuple<int     >>),
	(std::tuple<std::tuple<column_a<recorded<int, -1>>, column_b<recorded<int, -2>>>, std::tuple<int, int>>))
{
	using ColumnTypes = std::tuple_element_t<0, TestType>;
	using Args        = std::tuple_element_t<1, TestType>;

	static constexpr auto test = [] { return []<typename... Columns>(std::tuple<Columns...>*)
	{
		using Types = std::tuple<agnes::column_element_t<Columns>...>;

		const filled_rows<Types, Args, 3> initial;

		const auto test = [&initial](const auto& erase)
		{
			// [initial[0]] -> []
			{
				agnes::table<Columns...> t{initial[0]};

				erase(t, t.begin());

				AGNES_REQUIRE(t.size() == 0);
			}

			// [initial[0], initial[1]] -> [initial[1]]
			{
				agnes::table<Columns...> t{initial[0], initial[1]};

				erase(t, t.begin());

				AGNES_REQUIRE(t.size() == 1);

				COLUMN_A_REQUIRE(t[0].a == get<0>(initial[1]));
				COLUMN_B_REQUIRE(t[0].b == get<1>(initial[1]));

				COLUMN_A_REQUIRE(t[0].a.actions.equals({class_action::copyConstructed, class_action::moveAssigned}));
				COLUMN_B_REQUIRE(t[0].b.actions.equals({class_action::copyConstructed, class_action::moveAssigned}));
			}

			// [initial[0], initial[1]] -> [initial[0]]
			{
				agnes::table<Columns...> t{initial[0], initial[1]};

				erase(t, t.begin() + 1);

				AGNES_REQUIRE(t.size() == 1);

				COLUMN_A_REQUIRE(t[0].a == get<0>(initial[0]));
				COLUMN_B_REQUIRE(t[0].b == get<1>(initial[0]));

				COLUMN_A_REQUIRE(t[0].a.actions.equals({class_action::copyConstructed}));
				COLUMN_B_REQUIRE(t[0].b.actions.equals({class_action::copyConstructed}));
			}

			// [initial[0], initial[1], initial[2]] -> [initial[1], initial[2]]
			{
				agnes::table<Columns...> t{initial[0], initial[1], initial[2]};

				erase(t, t.begin());

				AGNES_REQUIRE(t.size() == 2);

				COLUMN_A_REQUIRE(t[0].a == get<0>(initial[1]));
				COLUMN_B_REQUIRE(t[0].b == get<1>(initial[1]));

				COLUMN_A_REQUIRE(t[1].a == get<0>(initial[2]));
				COLUMN_B_REQUIRE(t[1].b == get<1>(initial[2]));

				COLUMN_A_REQUIRE(t[0].a.actions.equals({class_action::copyConstructed, class_action::moveAssigned}));
				COLUMN_B_REQUIRE(t[0].b.actions.equals({class_action::copyConstructed, class_action::moveAssigned}));

				COLUMN_A_REQUIRE(t[1].a.actions.equals({class_action::copyConstructed, class_action::moveAssigned}));
				COLUMN_B_REQUIRE(t[1].b.actions.equals({class_action::copyConstructed, class_action::moveAssigned}));
			}

			// [initial[0], initial[1], initial[2]] -> [initial[0], initial[2]]
			{
				agnes::table<Columns...> t{initial[0], initial[1], initial[2]};

				erase(t, t.begin() + 1);

				AGNES_REQUIRE(t.size() == 2);

				COLUMN_A_REQUIRE(t[0].a == get<0>(initial[0]));
				COLUMN_B_REQUIRE(t[0].b == get<1>(initial[0]));

				COLUMN_A_REQUIRE(t[1].a == get<0>(initial[2]));
				COLUMN_B_REQUIRE(t[1].b == get<1>(initial[2]));

				COLUMN_A_REQUIRE(t[0].a.actions.equals({class_action::copyConstructed                            }));
				COLUMN_B_REQUIRE(t[0].b.actions.equals({class_action::copyConstructed                            }));

				COLUMN_A_REQUIRE(t[1].a.actions.equals({class_action::copyConstructed, class_action::moveAssigned}));
				COLUMN_B_REQUIRE(t[1].b.actions.equals({class_action::copyConstructed, class_action::moveAssigned}));
			}

			// [initial[0], initial[1], initial[2]] -> [initial[0], initial[1]]
			{
				agnes::table<Columns...> t{initial[0], initial[1], initial[2]};

				erase(t, t.begin() + 2);

				AGNES_REQUIRE(t.size() == 2);

				COLUMN_A_REQUIRE(t[0].a == get<0>(initial[0]));
				COLUMN_B_REQUIRE(t[0].b == get<1>(initial[0]));

				COLUMN_A_REQUIRE(t[1].a == get<0>(initial[1]));
				COLUMN_B_REQUIRE(t[1].b == get<1>(initial[1]));

				COLUMN_A_REQUIRE(t[0].a.actions.equals({class_action::copyConstructed}));
				COLUMN_B_REQUIRE(t[0].b.actions.equals({class_action::copyConstructed}));

				COLUMN_A_REQUIRE(t[1].a.actions.equals({class_action::copyConstructed}));
				COLUMN_B_REQUIRE(t[1].b.actions.equals({class_action::copyConstructed}));
			}

			return true;
		};

		const auto erase      = [](auto& t, const auto& position) { t.erase(position              ); };
		const auto eraseRange = [](auto& t, const auto& position) { t.erase(position, position + 1); };

		return test(erase) && test(eraseRange);

	}(static_cast<ColumnTypes*>(nullptr)); };

	AGNES_RUN(test);
}

TEMPLATE_TEST_CASE("agnes::table::erase() multiple", "",
	(std::tuple<std::tuple<column_a<recorded<int, -1>>                             >, std::tuple<int     >>),
	(std::tuple<std::tuple<column_a<recorded<int, -1>>, column_b<recorded<int, -2>>>, std::tuple<int, int>>))
{
	using ColumnTypes = std::tuple_element_t<0, TestType>;
	using Args        = std::tuple_element_t<1, TestType>;

	static constexpr auto test = [] { return []<typename... Columns>(std::tuple<Columns...>*)
	{
		using Types = std::tuple<agnes::column_element_t<Columns>...>;

		const filled_rows<Types, Args, 4> initial;

		// [initial[0], initial[1]] -> []
		{
			agnes::table<Columns...> t{initial[0], initial[1]};

			t.erase(t.begin(), t.end());

			AGNES_REQUIRE(t.size() == 0);
		}

		// [initial[0], initial[1], initial[2], initial[3]] -> [initial[2], initial[3]]
		{
			agnes::table<Columns...> t{initial[0], initial[1], initial[2], initial[3]};

			t.erase(t.begin(), t.begin() + 2);

			AGNES_REQUIRE(t.size() == 2);

			COLUMN_A_REQUIRE(t[0].a.actions.equals({class_action::copyConstructed, class_action::moveAssigned}));
			COLUMN_B_REQUIRE(t[0].b.actions.equals({class_action::copyConstructed, class_action::moveAssigned}));

			COLUMN_A_REQUIRE(t[1].a.actions.equals({class_action::copyConstructed, class_action::moveAssigned}));
			COLUMN_B_REQUIRE(t[1].b.actions.equals({class_action::copyConstructed, class_action::moveAssigned}));
		}

		// [initial[0], initial[1], initial[2], initial[3]] -> [initial[0], initial[3]]
		{
			agnes::table<Columns...> t{initial[0], initial[1], initial[2], initial[3]};

			t.erase(t.begin() + 1, t.begin() + 3);

			AGNES_REQUIRE(t.size() == 2);

			COLUMN_A_REQUIRE(t[0].a.actions.equals({class_action::copyConstructed                            }));
			COLUMN_B_REQUIRE(t[0].b.actions.equals({class_action::copyConstructed                            }));

			COLUMN_A_REQUIRE(t[1].a.actions.equals({class_action::copyConstructed, class_action::moveAssigned}));
			COLUMN_B_REQUIRE(t[1].b.actions.equals({class_action::copyConstructed, class_action::moveAssigned}));
		}

		// [initial[0], initial[1], initial[2], initial[3]] -> [initial[0], initial[1]]
		{
			agnes::table<Columns...> t{initial[0], initial[1], initial[2], initial[3]};

			t.erase(t.begin() + 2, t.end());

			AGNES_REQUIRE(t.size() == 2);

			COLUMN_A_REQUIRE(t[0].a.actions.equals({class_action::copyConstructed}));
			COLUMN_B_REQUIRE(t[0].b.actions.equals({class_action::copyConstructed}));

			COLUMN_A_REQUIRE(t[1].a.actions.equals({class_action::copyConstructed}));
			COLUMN_B_REQUIRE(t[1].b.actions.equals({class_action::copyConstructed}));
		}

		return true;

	}(static_cast<ColumnTypes*>(nullptr)); };

	AGNES_RUN(test);
}

TEMPLATE_TEST_CASE("agnes::table::swap()", "",
	(std::tuple<column_a<int>               >),
	(std::tuple<column_a<int>, column_b<int>>))
{
	using ColumnTypes = TestType;

	static constexpr auto test = [] { return []<typename... Columns>(std::tuple<Columns...>*)
	{
		const auto test = [](const auto& swap_)
		{
			using Types = std::tuple<agnes::column_element_t<Columns>...>;

			const filled_rows<Types, Types,2> initial0{0};
			const filled_rows<Types, Types,2> initial1{1};

			{
				agnes::table<Columns...> t0;
				agnes::table<Columns...> t1;

				const auto prevData0{t0.data()};
				const auto prevData1{t1.data()};

				swap_(t1, t0);

				AGNES_REQUIRE(t0.empty());
				AGNES_REQUIRE(t1.empty());

				AGNES_REQUIRE(t0.data() == prevData1);
				AGNES_REQUIRE(t1.data() == prevData0);
			}

			{
				agnes::table<Columns...> t0;
				agnes::table<Columns...> t1{initial1[0]};

				const auto prevData0{t0.data()};
				const auto prevData1{t1.data()};

				swap_(t1, t0);

				AGNES_REQUIRE(t0.size() == 1);
				AGNES_REQUIRE(t1.empty());

				AGNES_REQUIRE(t0.data() == prevData1);
				AGNES_REQUIRE(t1.data() == prevData0);
			}

			{
				agnes::table<Columns...> t0;
				agnes::table<Columns...> t1{initial1[0], initial1[1]};

				const auto prevData0{t0.data()};
				const auto prevData1{t1.data()};

				swap_(t1, t0);

				AGNES_REQUIRE(t0.size() == 2);
				AGNES_REQUIRE(t1.empty());

				AGNES_REQUIRE(t0.data() == prevData1);
				AGNES_REQUIRE(t1.data() == prevData0);
			}

			{
				agnes::table<Columns...> t0{initial0[0]};
				agnes::table<Columns...> t1;

				const auto prevData0{t0.data()};
				const auto prevData1{t1.data()};

				swap_(t1, t0);

				AGNES_REQUIRE(t0.empty());
				AGNES_REQUIRE(t1.size() == 1);

				AGNES_REQUIRE(t0.data() == prevData1);
				AGNES_REQUIRE(t1.data() == prevData0);
			}

			{
				agnes::table<Columns...> t0{initial0[0]};
				agnes::table<Columns...> t1{initial1[0]};

				const auto prevData0{t0.data()};
				const auto prevData1{t1.data()};

				swap_(t1, t0);

				AGNES_REQUIRE(t0.size() == 1);
				AGNES_REQUIRE(t1.size() == 1);

				AGNES_REQUIRE(t0.data() == prevData1);
				AGNES_REQUIRE(t1.data() == prevData0);
			}

			{
				agnes::table<Columns...> t0{initial0[0]};
				agnes::table<Columns...> t1{initial1[0], initial1[1]};

				const auto prevData0{t0.data()};
				const auto prevData1{t1.data()};

				swap_(t1, t0);

				AGNES_REQUIRE(t0.size() == 2);
				AGNES_REQUIRE(t1.size() == 1);

				AGNES_REQUIRE(t0.data() == prevData1);
				AGNES_REQUIRE(t1.data() == prevData0);
			}

			{
				agnes::table<Columns...> t0{initial0[0], initial0[1]};
				agnes::table<Columns...> t1;

				const auto prevData0{t0.data()};
				const auto prevData1{t1.data()};

				swap_(t1, t0);

				AGNES_REQUIRE(t0.empty());
				AGNES_REQUIRE(t1.size() == 2);

				AGNES_REQUIRE(t0.data() == prevData1);
				AGNES_REQUIRE(t1.data() == prevData0);
			}

			{
				agnes::table<Columns...> t0{initial0[0], initial0[1]};
				agnes::table<Columns...> t1{initial1[0]};

				const auto prevData0{t0.data()};
				const auto prevData1{t1.data()};

				swap_(t1, t0);

				AGNES_REQUIRE(t0.size() == 1);
				AGNES_REQUIRE(t1.size() == 2);

				AGNES_REQUIRE(t0.data() == prevData1);
				AGNES_REQUIRE(t1.data() == prevData0);
			}

			{
				agnes::table<Columns...> t0{initial0[0], initial0[1]};
				agnes::table<Columns...> t1{initial1[0], initial1[1]};

				const auto prevData0{t0.data()};
				const auto prevData1{t1.data()};

				swap_(t1, t0);

				AGNES_REQUIRE(t0.size() == 2);
				AGNES_REQUIRE(t1.size() == 2);

				AGNES_REQUIRE(t0.data() == prevData1);
				AGNES_REQUIRE(t1.data() == prevData0);
			}

			return true;
		};

		const auto memberSwap    = [](auto& x, auto& y) { x.swap(y); };
		const auto namespaceSwap = [](auto& x, auto& y) { swap(x, y); };

		return test(memberSwap) and test(namespaceSwap);

	}(static_cast<ColumnTypes*>(nullptr)); };

	AGNES_RUN(test);
}

TEMPLATE_TEST_CASE("agnes::table::clear()", "",
	(std::tuple<column_a<int>               >),
	(std::tuple<column_a<int>, column_b<int>>))
{
	using ColumnTypes = TestType;

	static constexpr auto test = [] { return []<typename... Columns>(std::tuple<Columns...>*)
	{
		using Types = std::tuple<agnes::column_element_t<Columns>...>;

		const filled_rows<Types, Types, 2> initial;

		{
			agnes::table<Columns...> t;

			t.clear();

			AGNES_REQUIRE(t.empty());
		}

		{
			agnes::table<Columns...> t{initial[0]};

			t.clear();

			AGNES_REQUIRE(t.empty());
		}

		{
			agnes::table<Columns...> t{initial[0], initial[1]};

			t.clear();

			AGNES_REQUIRE(t.empty());
		}

		return true;

	}(static_cast<ColumnTypes*>(nullptr)); };

	AGNES_RUN(test);
}

TEMPLATE_TEST_CASE("agnes::table compare", "",
	(std::tuple<column_a<int>               >),
	(std::tuple<column_a<int>, column_b<int>>))
{
	using ColumnTypes = TestType;

	static constexpr auto test = [] { return []<typename... Columns>(std::tuple<Columns...>*)
	{
		using Types = std::tuple<agnes::column_element_t<Columns>...>;

		const filled_rows<Types, Types, 2> initialLess   {0};
		const filled_rows<Types, Types, 2> initialEqual  {1};
		const filled_rows<Types, Types, 2> initialGreater{2};

		const agnes::table<Columns...> t      {initialEqual  [0], initialEqual  [1]};
		const agnes::table<Columns...> less   {initialLess   [0], initialLess   [1]};
		const agnes::table<Columns...> equal  {initialEqual  [0], initialEqual  [1]};
		const agnes::table<Columns...> greater{initialGreater[0], initialGreater[1]};

		AGNES_REQUIRE(!(t <  t));
		AGNES_REQUIRE(  t <= t );
		AGNES_REQUIRE(  t == t );
		AGNES_REQUIRE(!(t != t));
		AGNES_REQUIRE(  t >= t );
		AGNES_REQUIRE(!(t >  t));

		AGNES_REQUIRE(!(t <  less));
		AGNES_REQUIRE(!(t <= less));
		AGNES_REQUIRE(!(t == less));
		AGNES_REQUIRE(  t != less );
		AGNES_REQUIRE(  t >= less );
		AGNES_REQUIRE(  t >  less );

		AGNES_REQUIRE(!(t <  equal));
		AGNES_REQUIRE(  t <= equal );
		AGNES_REQUIRE(  t == equal );
		AGNES_REQUIRE(!(t != equal));
		AGNES_REQUIRE(  t >= equal );
		AGNES_REQUIRE(!(t >  equal));

		AGNES_REQUIRE(  t <  greater );
		AGNES_REQUIRE(  t <= greater );
		AGNES_REQUIRE(!(t == greater));
		AGNES_REQUIRE(  t != greater );
		AGNES_REQUIRE(!(t >= greater));
		AGNES_REQUIRE(!(t >  greater));

		return true;

	}(static_cast<ColumnTypes*>(nullptr)); };

	AGNES_RUN(test);
}

TEMPLATE_TEST_CASE("agnes::table namespace erase() and erase_if()", "",
	(std::tuple<column_a<int>               >),
	(std::tuple<column_a<int>, column_b<int>>))
{
	using ColumnTypes = TestType;

	static constexpr auto test = [] { return []<typename... Columns>(std::tuple<Columns...>*)
	{
		using Types = std::tuple<agnes::column_element_t<Columns>...>;

		const filled_rows<Types, Types, 2> initial;

		const auto test = [&initial](const auto& erase_)
		{
			using row = typename agnes::table<Columns...>::row;

			agnes::table<Columns...> t{initial[0], initial[1]};

			const auto count = erase_(t, t[0]);

			AGNES_REQUIRE(count == 1);

			return true;
		};

		const auto namespaceErase = [](auto& t, const auto& row)
		{
			return erase(t, row);
		};

		const auto namespaceEraseIf = [](auto& t, const auto& row)
		{
			return erase_if(t, [&row](const auto& rowElem)
			{
				return rowElem == row;
			});
		};

		return test(namespaceErase) && test(namespaceEraseIf);

	}(static_cast<ColumnTypes*>(nullptr)); };

	AGNES_RUN(test);
}
