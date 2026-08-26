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

#include <agnes/vector.h>

#include <algorithm>
#include <cstddef>
#include <iterator>
#include <tuple>
#include <type_traits>
#include <utility>

#include <catch2/catch_template_test_macros.hpp>

#include "agnes_tests/filled_columns.h"
#include "agnes_tests/filled_rows.h"
#include "agnes_tests/filled_value.h"
#include "agnes_tests/macros.h"
#include "agnes_tests/not_noexcept_moveable.h"
#include "agnes_tests/recorded.h"
#include "agnes_tests/user_defined_allocator.h"

#include "agnes/memory.h"

using namespace agnes_tests;

TEMPLATE_TEST_CASE("agnes::vector member types", "",
	(std::tuple<int       >),
	(std::tuple<int, float>))
{
	static constexpr auto test = [] { return []<typename... T>(std::tuple<T...>*)
	{
		bool result = true;

		AGNES_REQUIRE(std::is_same_v<typename agnes::vector<T...>::value_type     , agnes::fields<T...>          >);
		AGNES_REQUIRE(std::is_same_v<typename agnes::vector<T...>::allocator_type , agnes::allocator<T...>       >);
		AGNES_REQUIRE(std::is_same_v<typename agnes::vector<T...>::pointer        , agnes::pointer<T*...>        >);
		AGNES_REQUIRE(std::is_same_v<typename agnes::vector<T...>::const_pointer  , agnes::pointer<const T*...>  >);
		AGNES_REQUIRE(std::is_same_v<typename agnes::vector<T...>::reference      , agnes::reference<T&...>      >);
		AGNES_REQUIRE(std::is_same_v<typename agnes::vector<T...>::const_reference, agnes::reference<const T&...>>);
		AGNES_REQUIRE(std::is_same_v<typename agnes::vector<T...>::size_type      , std::size_t                  >);
		AGNES_REQUIRE(std::is_same_v<typename agnes::vector<T...>::difference_type, std::ptrdiff_t               >);

		return result;

	}(static_cast<TestType*>(nullptr)); };

	AGNES_RUN(test);
}

TEMPLATE_TEST_CASE("agnes::vector default construction", "",
	(std::tuple<int       >),
	(std::tuple<int, float>))
{
	static constexpr auto test = [] { return []<typename... T>(std::tuple<T...>*)
	{
		const agnes::vector<T...> v;

		AGNES_REQUIRE(v.empty());

		return true;

	}(static_cast<TestType*>(nullptr)); };

	AGNES_RUN(test);
}

TEMPLATE_TEST_CASE_SIG("agnes::vector default inserted construction", "",
	((typename Types, std::size_t... Counts), Types, Counts...),
	((std::tuple<recorded<int, 0>                  >), 0, 1, 2),
	((std::tuple<recorded<int, 0>, recorded<int, 1>>), 0, 1, 2))
{
	static constexpr auto test = [] { return ([]<std::size_t... Ti, typename... T, std::size_t N>(
		std::index_sequence<Ti...>,
		std::tuple<T...>*,
		std::integral_constant<std::size_t, N>)
	{
		const agnes::vector<T...> v(N);

		AGNES_REQUIRE(v.size() == N);

		for (auto j = 0; j < N; ++j)
		{
			AGNES_REQUIRE(((get<Ti>(v[j]) == T{}) && ...));

			AGNES_REQUIRE((get<Ti>(v[j]).actions.equals({class_action::defaultConstructed}) && ...));
		}

		return true;

	}(std::make_index_sequence<std::tuple_size_v<Types>>{},
		static_cast<Types*>(nullptr),
		std::integral_constant<std::size_t, Counts>{}) && ...); };

	AGNES_RUN(test);
}

TEMPLATE_TEST_CASE_SIG("agnes::vector copy inserted construction", "",
	((typename Types, typename Args, std::size_t... Counts), Types, Args, Counts...),
	((std::tuple<recorded<int, -1>                   >), (std::tuple<int       >), 0, 1, 2),
	((std::tuple<recorded<int, -1>, recorded<int, -2>>), (std::tuple<int, short>), 0, 1, 2))
{
	static constexpr auto test = [] { return ([]<std::size_t... Ti, typename... T, std::size_t N>(
		std::index_sequence<Ti...>,
		std::tuple<T...>*,
		std::integral_constant<std::size_t, N>)
	{
		const Types inserted{filled_value<Types, Args>{}};

		{
			const agnes::vector<T...> v(N, get<Ti>(inserted)...);

			AGNES_REQUIRE(v.size() == N);

			for (auto j = 0; j < N; ++j)
			{
				AGNES_REQUIRE(v[j] == inserted);

				AGNES_REQUIRE((get<Ti>(v[j]).actions.equals({class_action::copyConstructed}) && ...));
			}
		}

		{
			const agnes::vector<T...> v(N, agnes::reference{get<Ti>(inserted)...});

			AGNES_REQUIRE(v.size() == N);

			for (auto j = 0; j < N; ++j)
			{
				AGNES_REQUIRE(v[j] == inserted);

				AGNES_REQUIRE((get<Ti>(v[j]).actions.equals({class_action::copyConstructed}) && ...));
			}
		}

		return true;

	}(std::make_index_sequence<std::tuple_size_v<Types>>{},
		static_cast<Types*>(nullptr),
		std::integral_constant<std::size_t, Counts>{}) && ...); };

	AGNES_RUN(test);
}

TEMPLATE_TEST_CASE_SIG("agnes::vector iterator inserted construction", "",
	((typename Types, typename Args, std::size_t... Counts), Types, Args, Counts...),
	((std::tuple<recorded<int, -1>                   >), (std::tuple<int       >), 0, 1, 2),
	((std::tuple<recorded<int, -1>, recorded<int, -2>>), (std::tuple<int, short>), 0, 1, 2))
{
	static constexpr auto test = [] { return ([]<std::size_t... Ti, typename... T, std::size_t N>(
		std::index_sequence<Ti...>,
		std::tuple<T...>*,
		std::integral_constant<std::size_t, N>)
	{
		const filled_columns<Types, Args, N> iterated;

		const agnes::pointer first{get<Ti>(iterated).data()...};
		const agnes::pointer last {first + N};

		const agnes::vector<T...> v{first, last};

		AGNES_REQUIRE(v.size() == N);

		for (auto j = 0; j < N; ++j)
		{
			AGNES_REQUIRE(v[j] == agnes::reference{get<Ti>(iterated)[j]...});

			AGNES_REQUIRE((get<Ti>(v[j]).actions.equals({class_action::copyConstructed}) && ...));
		}

		return true;

	}(std::make_index_sequence<std::tuple_size_v<Types>>{},
		static_cast<Types*>(nullptr),
		std::integral_constant<std::size_t, Counts>{}) && ...); };

	AGNES_RUN(test);
}

TEMPLATE_TEST_CASE_SIG("agnes::vector deduced iterator inserted construction", "",
	((typename Types, typename Args, std::size_t... Counts), Types, Args, Counts...),
	((std::tuple<recorded<int, -1>                   >), (std::tuple<int     >), 0, 1, 2),
	((std::tuple<recorded<int, -1>, recorded<int, -2>>), (std::tuple<int, int>), 0, 1, 2))
{
	static constexpr auto test = [] { return ([]<std::size_t... Ti, typename... T, std::size_t N>(
		std::index_sequence<Ti...>,
		std::tuple<T...>*,
		std::integral_constant<std::size_t, N>)
	{
		const filled_columns<Types, Args, N> iterated;

		const agnes::pointer first{get<Ti>(iterated).data()...};
		const agnes::pointer last {first + N};

		const agnes::vector v{first, last};

		AGNES_REQUIRE(v.size() == N);

		for (auto j = 0; j < N; ++j)
		{
			AGNES_REQUIRE(v[j] == agnes::reference{get<Ti>(iterated)[j]...});

			AGNES_REQUIRE((get<Ti>(v[j]).actions.equals({class_action::copyConstructed}) && ...));
		}

		return true;

	}(std::make_index_sequence<std::tuple_size_v<Types>>{},
		static_cast<Types*>(nullptr),
		std::integral_constant<std::size_t, Counts>{}) && ...); };

	AGNES_RUN(test);
}

TEMPLATE_TEST_CASE_SIG("agnes::vector copy construction", "",
	((typename Types, typename Args, std::size_t... Counts), Types, Args, Counts...),
	((std::tuple<recorded<int, -1>                   >), (std::tuple<int     >), 0, 1, 2),
	((std::tuple<recorded<int, -1>, recorded<int, -2>>), (std::tuple<int, int>), 0, 1, 2))
{
	static constexpr auto test = [] { return ([]<std::size_t... Ti, typename... T, std::size_t... I, std::size_t N>(
		std::index_sequence<Ti...>,
		std::tuple<T...>*,
		std::index_sequence<I...>,
		std::integral_constant<std::size_t, N>)
	{
		const filled_rows<Types, Args, N> otherInitial;

		const agnes::vector<T...> other{otherInitial[I]...};

		const agnes::vector<T...> v{other};

		AGNES_REQUIRE((v == other));

		for (auto j = 0; j < N; ++j)
		{
			AGNES_REQUIRE((get<Ti>(v[j]).actions.equals({class_action::copyConstructed}) && ...));
		}

		return true;

	}(std::make_index_sequence<std::tuple_size_v<Types>>{},
		static_cast<Types*>(nullptr),
		std::make_index_sequence<Counts>{},
		std::integral_constant<std::size_t, Counts>{}) && ...); };

	AGNES_RUN(test);
}

TEMPLATE_TEST_CASE_SIG("agnes::vector move construction", "",
	((typename Types, typename Args, std::size_t... Counts), Types, Args, Counts...),
	((std::tuple<recorded<int, -1>                   >), (std::tuple<int     >), 0, 1, 2),
	((std::tuple<recorded<int, -1>, recorded<int, -2>>), (std::tuple<int, int>), 0, 1, 2))
{
	static constexpr auto test = [] { return ([]<std::size_t... I, typename... T, std::size_t N>(
		std::index_sequence<I...>,
		std::tuple<T...>*,
		std::integral_constant<std::size_t, N>)
	{
		const filled_rows<Types, Args, N> otherInitial;

		agnes::vector<T...>  other   {otherInitial[I]...};
		const agnes::pointer prevData{other.data()};

		const agnes::vector<T...> v{std::move(other)};

		AGNES_REQUIRE(v.data() == prevData);

		return true;

	}(std::make_index_sequence<Counts>{},
		static_cast<Types*>(nullptr),
		std::integral_constant<std::size_t, Counts>{}) && ...); };

	AGNES_RUN(test);
}

TEMPLATE_TEST_CASE_SIG("agnes::vector construction from initializer list", "",
	((typename Types, typename Args, std::size_t... Counts), Types, Args, Counts...),
	((std::tuple<recorded<int, -1>                   >), (std::tuple<int     >), 0, 1, 2),
	((std::tuple<recorded<int, -1>, recorded<int, -2>>), (std::tuple<int, int>), 0, 1, 2))
{
	static constexpr auto test = [] { return ([]<std::size_t... Ti, typename... T, std::size_t... I, std::size_t N>(
		std::index_sequence<Ti...>,
		std::tuple<T...>*,
		std::index_sequence<I...>,
		std::integral_constant<std::size_t, N>)
	{
		const filled_rows<Types, Args, N> initializing;

		const agnes::vector<T...> v{initializing[I]...};

		for (auto j = 0; j < N; ++j)
		{
			AGNES_REQUIRE(v[j] == initializing[j]);
			AGNES_REQUIRE((get<Ti>(v[j]).actions.equals({class_action::copyConstructed}) && ...));
		}

		return true;

	}(std::make_index_sequence<std::tuple_size_v<Types>>{},
		static_cast<Types*>(nullptr),
		std::make_index_sequence<Counts>{},
		std::integral_constant<std::size_t, Counts>{}) && ...); };

	AGNES_RUN(test);
}

TEMPLATE_TEST_CASE_SIG("agnes::vector copy assignment", "",
	((typename Types, typename Args, std::size_t... Counts), Types, Args, Counts...),
	((std::tuple<recorded<int, -1>                   >), (std::tuple<int     >), 0, 1, 2),
	((std::tuple<recorded<int, -1>, recorded<int, -2>>), (std::tuple<int, int>), 0, 1, 2))
{
	static constexpr auto test = []
	{
		const auto test = []<std::size_t... Ti, typename... T, std::size_t... I, std::size_t N>(
			std::index_sequence<Ti...>,
			std::tuple<T...>*,
			std::index_sequence<I...>,
			std::integral_constant<std::size_t, N>)
		{
			// (Too much for MSVC to call this lambda anonymously.)

			const auto test = [](const auto& assign, const auto& compare)
			{
				// [] = [y[0]...y[N]]
				{
					agnes::vector<T...> v;

					assign(v);

					for (auto i = 0; i < N; ++i)
					{
						AGNES_REQUIRE(compare(v, i));
						AGNES_REQUIRE((get<Ti>(v[i]).actions.equals({class_action::copyConstructed}) && ...));
					}
				}

				// [x[0]...x[N]] = [y[0]...y[N]]
				{
					agnes::vector<T...> v(N);

					assign(v);

					for (auto i = 0; i < N; ++i)
					{
						AGNES_REQUIRE(compare(v, i));
						AGNES_REQUIRE((get<Ti>(v[i]).actions.equals(
							{class_action::defaultConstructed, class_action::copyAssigned}) && ...));
					}
				}

				// [x[0], unused[1]...unused[N]] = [y[0]...y[N]]
				{
					agnes::vector<T...> v;

					v.reserve(std::max(N, std::size_t{1}));

					v.emplace_back();

					assign(v);

					for (auto i = 0; (i < 1) && (i < N); ++i)
					{
						AGNES_REQUIRE(compare(v, i));
						AGNES_REQUIRE((get<Ti>(v[i]).actions.equals(
							{class_action::defaultConstructed, class_action::copyAssigned}) && ...));
					}

					for (auto i = 1; i < N; ++i)
					{
						AGNES_REQUIRE(compare(v, i));
						AGNES_REQUIRE((get<Ti>(v[i]).actions.equals({class_action::copyConstructed}) && ...));
					}
				}

				return true;
			};

			const auto assignToVector = [&test]
			{
				const filled_rows<Types, Args, N> otherInitial;

				if constexpr (N == 0)
				{
					const agnes::vector<T...> other;

					const auto assign  = [&other](auto& v) { v = other; };
					const auto compare = [&other](const auto& v, const auto i) { return v[i] == other[i]; };

					return test(assign, compare);
				}
				else
				{
					const agnes::vector<T...> other{otherInitial[I]...};

					const auto assign  = [&other](auto& v) { v = other; };
					const auto compare = [&other](const auto& v, const auto i) { return v[i] == other[i]; };

					return test(assign, compare);
				}
			};

			const auto assignToInitializerList = [&test]
			{
				const filled_rows<Types, Args, N> initializing;

				const auto operatorAssign = [&initializing](auto& v) { v = {initializing[I]...}; };
				const auto functionAssign = [&initializing](auto& v) { v.assign({initializing[I]...}); };
				const auto compare        = [&initializing](const auto& v, const auto i)
				{
					return v[i] == initializing[i];
				};

				return test(operatorAssign, compare) && test(functionAssign, compare);
			};

			const auto assignToIterators = [&test]
			{
				const filled_columns<Types, Args, N> iterated;

				const agnes::pointer first{get<Ti>(iterated).data()...};
				const agnes::pointer last {first + N};

				const auto assign  = [&first, &last](auto& v) { v.assign(first, last); };
				const auto compare = [&iterated](const auto& v, const auto i)
				{
					return v[i] == agnes::reference{get<Ti>(iterated)[i]...};
				};

				return test(assign, compare);
			};

			const auto assignToCopies = [&test]
			{
				const Types copied{filled_value<Types, Args>{}};

				const auto directAssign = [&copied](auto& v)
				{
					std::apply([&v](const auto&... copied) { v.assign(N, copied...); }, copied);
				};

				const auto referenceAssign = [&copied](auto& v)
				{
					std::apply([&v](const auto&... copied)
					{
						v.assign(N, agnes::reference{copied...});
					}, copied);
				};

				const auto compare = [&copied](const auto& v, const auto i) { return v[i] == copied; };

				return test(directAssign, compare) && test(referenceAssign, compare);
			};

			return assignToVector() && assignToInitializerList() && assignToIterators() && assignToCopies();
		};
		
		return (test(std::make_index_sequence<std::tuple_size_v<Types>>{},
			static_cast<Types*>(nullptr),
			std::make_index_sequence<Counts>{},
			std::integral_constant<std::size_t, Counts>{}) && ...);
	};

	AGNES_RUN(test);
}

TEMPLATE_TEST_CASE_SIG("agnes::vector move assignment", "",
	((typename Types, typename Args, std::size_t... Counts), Types, Args, Counts...),
	((std::tuple<recorded<int, -1>                   >), (std::tuple<int     >), 0, 1, 2),
	((std::tuple<recorded<int, -1>, recorded<int, -2>>), (std::tuple<int, int>), 0, 1, 2))
{
	static constexpr auto test = [] { return ([]<std::size_t... I, typename... T, std::size_t N>(
		std::index_sequence<I...>,
		std::tuple<T...>*,
		std::integral_constant<std::size_t, N>)
	{
		const filled_rows<Types, Args, N> otherInitial;

		agnes::vector<T...>  other   {otherInitial[I]...};
		const agnes::pointer prevData{other.data()};

		agnes::vector<T...> v;
		
		v = std::move(other);

		AGNES_REQUIRE(v.data() == prevData);

		return true;

	}(std::make_index_sequence<Counts>{},
		static_cast<Types*>(nullptr),
		std::integral_constant<std::size_t, Counts>{}) && ...); };

	AGNES_RUN(test);
}

TEMPLATE_TEST_CASE_SIG("agnes::vector move assignment (user-defined allocator)", "",
	((typename Allocator, typename OtherAllocator, typename Types, typename Args, std::size_t... Counts), Allocator, OtherAllocator, Types, Args, Counts...),
	((user_defined_allocator_pocma                    <recorded<int, -1>                   >), (user_defined_allocator_pocma                    <recorded<int, -1>                   >), (std::tuple<recorded<int, -1>                   >), (std::tuple<int     >), 0, 1, 2),
	((user_defined_allocator_pocma                    <recorded<int, -1>, recorded<int, -2>>), (user_defined_allocator_pocma                    <recorded<int, -1>, recorded<int, -2>>), (std::tuple<recorded<int, -1>, recorded<int, -2>>), (std::tuple<int, int>), 0, 1, 2),
	((user_defined_allocator_no_pocma_always_equal    <recorded<int, -1>                   >), (user_defined_allocator_no_pocma_always_equal    <recorded<int, -1>                   >), (std::tuple<recorded<int, -1>                   >), (std::tuple<int     >), 0, 1, 2),
	((user_defined_allocator_no_pocma_always_equal    <recorded<int, -1>, recorded<int, -2>>), (user_defined_allocator_no_pocma_always_equal    <recorded<int, -1>, recorded<int, -2>>), (std::tuple<recorded<int, -1>, recorded<int, -2>>), (std::tuple<int, int>), 0, 1, 2),
	((user_defined_allocator_no_pocma_not_always_equal<recorded<int, -1>                   >), (user_defined_allocator_no_pocma_not_always_equal<recorded<int, -1>                   >), (std::tuple<recorded<int, -1>                   >), (std::tuple<int     >), 0, 1, 2),
	((user_defined_allocator_no_pocma_not_always_equal<recorded<int, -1>, recorded<int, -2>>), (user_defined_allocator_no_pocma_not_always_equal<recorded<int, -1>, recorded<int, -2>>), (std::tuple<recorded<int, -1>, recorded<int, -2>>), (std::tuple<int, int>), 0, 1, 2))
{
	static constexpr auto test = [] { return ([]<std::size_t... Ti, typename... T, std::size_t... I, std::size_t N>(
		std::index_sequence<Ti...>,
		std::tuple<T...>*,
		std::index_sequence<I...>,
		std::integral_constant<std::size_t, N>)
	{
		const filled_rows<Types, Args, N> otherInitial;

		// [] = [y[0]...y[N]]
		{
			agnes::vector<T..., agnes::allocate_with<OtherAllocator>> other   {otherInitial[I]...};
			const agnes::pointer                                      prevData{other.data()};

			agnes::vector<T..., agnes::allocate_with<Allocator>> v;
		
			v = std::move(other);

			if (decltype(other)::allocator_type::propagate_on_container_move_assignment::value ||
				(v.get_allocator() == other.get_allocator()))
			{
				AGNES_REQUIRE(v.data() == prevData);
			}
			else
			{
				for (auto i = 0; i < N; ++i)
				{
					AGNES_REQUIRE(v[i] == other[i]);
					AGNES_REQUIRE((get<Ti>(v[i]).actions.equals({class_action::moveConstructed}) && ...));
				}
			}
		}

		// [x[0]...x[N]] = [y[0]...y[N]]
		{
			agnes::vector<T..., agnes::allocate_with<OtherAllocator>> other   {otherInitial[I]...};
			const agnes::pointer                                      prevData{other.data()};

			agnes::vector<T..., agnes::allocate_with<Allocator>> v(N);
		
			v = std::move(other);

			if (decltype(other)::allocator_type::propagate_on_container_move_assignment::value ||
				(v.get_allocator() == other.get_allocator()))
			{
				AGNES_REQUIRE(v.data() == prevData);
			}
			else
			{
				for (auto i = 0; i < N; ++i)
				{
					AGNES_REQUIRE(v[i] == other[i]);
					AGNES_REQUIRE((get<Ti>(v[i]).actions.equals(
						{class_action::defaultConstructed, class_action::moveAssigned}) && ...));
				}
			}
		}

		// [x[0], unused[1]...unused[N]] = [y[0]...y[N]]
		{
			agnes::vector<T..., agnes::allocate_with<OtherAllocator>> other   {otherInitial[I]...};
			const agnes::pointer                                      prevData{other.data()};

			agnes::vector<T..., agnes::allocate_with<Allocator>> v;

			v.reserve(std::max(N, std::size_t{1}));

			v.emplace_back();
		
			v = std::move(other);

			if (decltype(other)::allocator_type::propagate_on_container_move_assignment::value ||
				(v.get_allocator() == other.get_allocator()))
			{
				AGNES_REQUIRE(v.data() == prevData);
			}
			else
			{
				for (auto i = 0; (i < 1) && (i < N); ++i)
				{
					AGNES_REQUIRE(v[i] == other[i]);
					AGNES_REQUIRE((get<Ti>(v[i]).actions.equals(
						{class_action::defaultConstructed, class_action::moveAssigned}) && ...));
				}

				for (auto i = 1; i < N; ++i)
				{
					AGNES_REQUIRE(v[i] == other[i]);
					AGNES_REQUIRE((get<Ti>(v[i]).actions.equals({class_action::moveConstructed}) && ...));
				}
			}
		}

		return true;

	}(std::make_index_sequence<std::tuple_size_v<Types>>{},
		static_cast<Types*>(nullptr),
		std::make_index_sequence<Counts>{},
		std::integral_constant<std::size_t, Counts>{}) && ...); };

	AGNES_RUN(test);
}

TEMPLATE_TEST_CASE_SIG("agnes::vector iterators", "",
	((typename Types, typename Args, std::size_t... Subset), Types, Args, Subset...),
	((std::tuple<recorded<int, -1>                                      >), (std::tuple<int          >), 0   ),
	((std::tuple<recorded<int, -1>, recorded<int, -2>                   >), (std::tuple<int, int     >), 1   ),
	((std::tuple<recorded<int, -1>, recorded<int, -2>, recorded<int, -3>>), (std::tuple<int, int, int>), 0, 2))
{
	static constexpr auto test = [] { return []<typename... T>(std::tuple<T...>*)
	{
		{
			agnes::vector<T...> v;

			AGNES_REQUIRE(v.begin() == v.end());
			AGNES_REQUIRE(v.begin() == v.cend());
			AGNES_REQUIRE(v.cbegin() == v.cend());

			AGNES_REQUIRE(begin<Subset...>(v) == end<Subset...>(v));
			AGNES_REQUIRE(begin<Subset...>(v) == cend<Subset...>(v));
			AGNES_REQUIRE(cbegin<Subset...>(v) == cend<Subset...>(v));
		}

		{
			const filled_rows<Types, Args, 4> initial;

			agnes::vector<T...> v{initial[0], initial[1], initial[2], initial[3]};

			AGNES_REQUIRE(v.begin() + 4 == v.end());
			AGNES_REQUIRE(v.begin() + 4 == v.cend());
			AGNES_REQUIRE(v.cbegin() + 4 == v.cend());

			AGNES_REQUIRE(begin<Subset...>(v) + 4 == end<Subset...>(v));
			AGNES_REQUIRE(begin<Subset...>(v) + 4 == cend<Subset...>(v));
			AGNES_REQUIRE(cbegin<Subset...>(v) + 4 == cend<Subset...>(v));

			std::size_t i = 0;

			for (auto iter = v.begin(); iter != v.end(); ++iter)
			{
				AGNES_REQUIRE(*iter == initial[i]);

				++i;
			}

			i = 0;

			for (const auto elem : v)
			{
				AGNES_REQUIRE(elem == initial[i]);

				++i;
			}

			i = 0;

			for (auto iter = begin<Subset...>(v); iter != end<Subset...>(v); ++iter)
			{
				AGNES_REQUIRE(*iter == std::tuple{std::get<Subset>(initial[i])...});

				++i;
			}

			i = 0;

			for (const auto elem : subview<Subset...>(v))
			{
				AGNES_REQUIRE(elem == std::tuple{std::get<Subset>(initial[i])...});

				++i;
			}
		}

		return true;

	}(static_cast<Types*>(nullptr)); };

	AGNES_RUN(test);
}

TEMPLATE_TEST_CASE("agnes::vector::resize()", "",
	(std::tuple<std::tuple<recorded<int>                   >, std::tuple<int     >>),
	(std::tuple<std::tuple<recorded<int>, recorded<int, -2>>, std::tuple<int, int>>))
{
	using Types = std::tuple_element_t<0, TestType>;
	using Args  = std::tuple_element_t<1, TestType>;

	static constexpr auto test = [] { return []<std::size_t... Ti, typename... T>(
		std::index_sequence<Ti...>,
		std::tuple<T...>*)
	{
		const auto test = [](const auto& resize, const auto& compare)
		{
			const filled_rows<Types, Args, 3> initial;

			// [] -> [{}]
			{
				agnes::vector<T...> v;

				const class_action expectedInsertAction{resize(v, 1)};

				AGNES_REQUIRE(v.size() == 1);

				AGNES_REQUIRE(compare(v[0]));

				AGNES_REQUIRE((get<Ti>(v[0]).actions.equals({expectedInsertAction}) && ...));
			}

			// [initial[0]] -> [initial[0], {}]
			{
				agnes::vector<T...> v{initial[0]};

				const class_action expectedInsertAction{resize(v, 2)};

				AGNES_REQUIRE(v.size() == 2);

				AGNES_REQUIRE(v[0] == initial[0]);
				AGNES_REQUIRE(compare(v[1]));

				AGNES_REQUIRE((get<Ti>(v[0]).actions.equals({class_action::moveConstructed}) && ...));
				AGNES_REQUIRE((get<Ti>(v[1]).actions.equals({expectedInsertAction}) && ...));
			}

			// [initial[0], initial[1], initial[2]] -> [initial[0], initial[1], unused]
			{
				agnes::vector<T...> v{initial[0], initial[1], initial[2]};

				const class_action expectedInsertAction{resize(v, 2)};

				AGNES_REQUIRE(v.size() == 2);

				AGNES_REQUIRE(v[0] == initial[0]);
				AGNES_REQUIRE(v[1] == initial[1]);

				AGNES_REQUIRE((get<Ti>(v[0]).actions.equals({class_action::copyConstructed}) && ...));
				AGNES_REQUIRE((get<Ti>(v[1]).actions.equals({class_action::copyConstructed}) && ...));
			}

			// [initial[0], unused, unused] -> [initial[0], {}, unused]
			{
				agnes::vector<T...> v;

				v.reserve(3);

				v.emplace_back(initial[0]);

				const class_action expectedInsertAction{resize(v, 2)};

				AGNES_REQUIRE(v.size() == 2);

				AGNES_REQUIRE(v[0] == initial[0]);
				AGNES_REQUIRE(compare(v[1]));

				AGNES_REQUIRE((get<Ti>(v[0]).actions.equals({class_action::copyConstructed}) && ...));
				AGNES_REQUIRE((get<Ti>(v[1]).actions.equals({expectedInsertAction}) && ...));
			}

			return true;
		};

		const auto resizeFillDefault = [&test]
		{
			const auto resize = [](auto& v, const auto n)
			{
				v.resize(n);

				return class_action::defaultConstructed;
			};

			const auto compare = [](const auto& x) { return x == std::tuple{T{}...}; };

			return test(resize, compare);
		};

		const auto resizeFillCopy = [&test]
		{
			const Types inserted{filled_value<Types, Args>{3}};

			const auto resize = [&inserted](auto& v, const auto n)
			{
				v.resize(n, get<Ti>(inserted)...);

				return class_action::copyConstructed;
			};

			const auto compare = [&inserted](const auto& x) { return x == inserted; };

			return test(resize, compare);
		};

		return resizeFillDefault() && resizeFillCopy();

	}(std::make_index_sequence<std::tuple_size_v<Types>>{}, static_cast<Types*>(nullptr)); };

	AGNES_RUN(test);
}

TEMPLATE_TEST_CASE("agnes::vector::reserve()", "",
	(std::tuple<std::tuple<recorded<int>                   >, std::tuple<int     >>),
	(std::tuple<std::tuple<recorded<int>, recorded<int, -2>>, std::tuple<int, int>>))
{
	using Types = std::tuple_element_t<0, TestType>;
	using Args  = std::tuple_element_t<1, TestType>;

	static constexpr auto test = [] { return []<std::size_t... Ti, typename... T>(
		std::index_sequence<Ti...>,
		std::tuple<T...>*)
	{
		const filled_rows<Types, Args, 2> initial;

		// [] -> [unused]
		{
			agnes::vector<T...> v;

			v.reserve(1);

			AGNES_REQUIRE(v.size() == 0);
			AGNES_REQUIRE(v.capacity() == 1);
		}

		// [initial[0], initial[1]] -> [initial[0], initial[1], unused]
		{
			agnes::vector<T...> v{initial[0], initial[1]};

			v.reserve(3);

			AGNES_REQUIRE(v.size() == 2);
			AGNES_REQUIRE(v.capacity() == 3);

			AGNES_REQUIRE(v[0] == initial[0]);
			AGNES_REQUIRE(v[1] == initial[1]);

			AGNES_REQUIRE((get<Ti>(v[0]).actions.equals({class_action::moveConstructed}) && ...));
			AGNES_REQUIRE((get<Ti>(v[1]).actions.equals({class_action::moveConstructed}) && ...));
		}

		// [initial[0], initial[1]] -> [initial[0], initial[1]]
		{
			agnes::vector<T...> v{initial[0], initial[1]};

			v.reserve(2);

			AGNES_REQUIRE(v.size() == 2);
			AGNES_REQUIRE(v.capacity() == 2);

			v.reserve(0);

			AGNES_REQUIRE(v.size() == 2);
			AGNES_REQUIRE(v.capacity() == 2);
		}

		return true;

	}(std::make_index_sequence<std::tuple_size_v<Types>>{}, static_cast<Types*>(nullptr)); };

	AGNES_RUN(test);
}

TEMPLATE_TEST_CASE("agnes::vector::shrink_to_fit", "",
	(std::tuple<std::tuple<recorded<int>               >, std::tuple<int     >>),
	(std::tuple<std::tuple<recorded<int>, recorded<int>>, std::tuple<int, int>>))
{
	using Types = std::tuple_element_t<0, TestType>;
	using Args  = std::tuple_element_t<1, TestType>;

	static constexpr auto test = [] { return []<typename... T>(std::tuple<T...>*)
	{
		const Types initial{filled_value<Types, Args>{0}};

		// [initial[0], unused] -> [initial[0]]
		{
			agnes::vector<T...> v;

			v.reserve(2);

			v.emplace_back(initial);

			v.shrink_to_fit();

			AGNES_REQUIRE(v.size() == 1);
			AGNES_REQUIRE(v.capacity() == 1);
		}

		// [initial[0]] -> [initial[0]]
		{
			agnes::vector<T...> v{initial};

			v.shrink_to_fit();

			AGNES_REQUIRE(v.size() == 1);
			AGNES_REQUIRE(v.capacity() == 1);
		}

		return true;

	}(static_cast<Types*>(nullptr)); };

	AGNES_RUN(test);
}

TEMPLATE_TEST_CASE_SIG("agnes::vector element access", "",
	((typename Types, typename Args, std::size_t... Subset), Types, Args, Subset...),
	((std::tuple<recorded<int, -1>                                      >), (std::tuple<int          >), 0   ),
	((std::tuple<recorded<int, -1>, recorded<int, -2>                   >), (std::tuple<int, int     >), 1   ),
	((std::tuple<recorded<int, -1>, recorded<int, -2>, recorded<int, -3>>), (std::tuple<int, int, int>), 0, 2))
{
	static constexpr auto test = [] { return []<typename... T>(std::tuple<T...>*)
	{
		{
			agnes::vector<T...> v;

			AGNES_REQUIRE(v.data() == nullptr);
		}

		{
			const filled_rows<Types, Args, 4> initial;

			agnes::vector<T...> v{initial[0], initial[1], initial[2], initial[3]};

			AGNES_REQUIRE(&v[0]      == &*v.begin());
			AGNES_REQUIRE(&v.at(0)   == &*v.begin());
			AGNES_REQUIRE(&v.front() == &*v.begin());
			AGNES_REQUIRE(&v.back()  == &*(v.end() - 1));
			AGNES_REQUIRE(v.data()   == &*v.begin());

			for (auto i = std::size_t{1}; i < v.size(); ++i)
			{
				AGNES_REQUIRE(&v[i]          == &*(v.begin() + i));
				AGNES_REQUIRE(&v.at(i)       == &*(v.begin() + i));
				AGNES_REQUIRE((v.data() + i) == &*(v.begin() + i));
			}

			AGNES_REQUIRE(&at<Subset...>(v, 0) == &(*begin<Subset...>(v)));
			AGNES_REQUIRE(&front<Subset...>(v) == &*begin<Subset...>(v));
			AGNES_REQUIRE(&back<Subset...>(v)  == &*(end<Subset...>(v) - 1));
			AGNES_REQUIRE(data<Subset...>(v)   == &*begin<Subset...>(v));

			for (auto i = std::size_t{1}; i < v.size(); ++i)
			{
				AGNES_REQUIRE(&at<Subset...>(v, i)       == &*(begin<Subset...>(v) + i));
				AGNES_REQUIRE((data<Subset...>(v) + i) == &*(begin<Subset...>(v) + i));
			}
		}

		return true;

	}(static_cast<Types*>(nullptr)); };

	AGNES_RUN(test);
}

TEMPLATE_TEST_CASE("agnes::vector::emplace_back() and push_back()", "",
	(std::tuple<std::tuple<recorded<int>                            >, std::tuple<int     >>),
	(std::tuple<std::tuple<recorded<int>, not_noexcept_moveable<int>>, std::tuple<int, int>>))
{
	using Types = std::tuple_element_t<0, TestType>;
	using Args  = std::tuple_element_t<1, TestType>;

	static constexpr auto test = [] { return []<std::size_t... Ti, typename... T>(
		std::index_sequence<Ti...>,
		std::tuple<T...>*)
	{
		const auto test = [](const auto& emplaceOrPushBack, const auto& compare)
		{
			const Types initial{filled_value<Types, Args>{0}};

			const class_action expectedReallocatingAction[]{(std::is_nothrow_move_constructible_v<T> ?
				class_action::moveConstructed : class_action::copyConstructed)...};

			// [] -> [inserted]
			{
				agnes::vector<T...> v;

				const class_action expectedInsertAction{emplaceOrPushBack(v)};

				AGNES_REQUIRE(v.size() == 1);

				AGNES_REQUIRE(compare(v[0]));

				AGNES_REQUIRE((get<Ti>(v[0]).actions.equals({expectedInsertAction}) && ...));
			}

			// [initial[0]] -> [initial[0], inserted]
			{
				agnes::vector<T...> v{initial};

				const class_action expectedInsertAction{emplaceOrPushBack(v)};

				AGNES_REQUIRE(v.size() == 2);

				AGNES_REQUIRE(v[0] == initial);
				AGNES_REQUIRE(compare(v[1]));

				AGNES_REQUIRE((get<Ti>(v[0]).actions.equals({expectedReallocatingAction[Ti]}) && ...));
				AGNES_REQUIRE((get<Ti>(v[1]).actions.equals({expectedInsertAction         }) && ...));
			}

			// [unused] -> [inserted]
			{
				agnes::vector<T...> v;

				v.reserve(1);

				const class_action expectedInsertAction{emplaceOrPushBack(v)};

				AGNES_REQUIRE(v.size() == 1);

				AGNES_REQUIRE(compare(v[0]));

				AGNES_REQUIRE((get<Ti>(v[0]).actions.equals({expectedInsertAction}) && ...));
			}

			// [initial[0], unused] -> [initial[0], inserted]
			{
				agnes::vector<T...> v;

				v.reserve(2);

				v.emplace_back(initial);

				const class_action expectedInsertAction{emplaceOrPushBack(v)};

				AGNES_REQUIRE(v.size() == 2);

				AGNES_REQUIRE(v[0] == initial);
				AGNES_REQUIRE(compare(v[1]));

				AGNES_REQUIRE((get<Ti>(v[0]).actions.equals({class_action::copyConstructed}) && ...));
				AGNES_REQUIRE((get<Ti>(v[1]).actions.equals({expectedInsertAction         }) && ...));
			}

			return true;
		};

		const auto emplaceBackDirect = [&test]
		{
			const Args args{filled_value<Args , Args>{1}};

			const auto emplaceBackValues = [&args](auto& v)
			{
				std::apply([&v](const auto&... args) { v.emplace_back(args...); }, args);

				return class_action::directConstructed;
			};

			const auto emplaceBackReference = [&args](auto& v)
			{
				std::apply([&v](const auto&... args) { v.emplace_back(agnes::reference{args...}); }, args);

				return class_action::directConstructed;
			};

			const auto emplaceBackPiecewise = [&args](auto& v)
			{
				std::apply([&v](const auto&... args)
				{
					v.emplace_back(std::piecewise_construct, std::tuple{args}...);
				}, args);

				return class_action::directConstructed;
			};

			const auto compare = [&args](const auto& x) { return x == args; };

			return test(emplaceBackValues, compare) &&
				test(emplaceBackReference, compare) &&
				test(emplaceBackPiecewise, compare);
		};

		const auto emplaceAndPushBackCopyAndMove = [&test]
		{
			Types inserted{filled_value<Types, Args>{1}};

			const auto emplaceBackCopy = [&inserted](auto& v)
			{
				v.emplace_back(inserted);

				return class_action::copyConstructed;
			};

			const auto emplaceBackMove = [&inserted](auto& v)
			{
				v.emplace_back(std::move(inserted));

				return class_action::moveConstructed;
			};

			const auto pushBackCopy = [&inserted](auto& v)
			{
				std::apply([&v](const auto&... inserted) { v.push_back(inserted...); }, inserted);

				return class_action::copyConstructed;
			};

			const auto pushBackMove = [&inserted](auto& v)
			{
				std::apply([&v](auto&... inserted) { v.push_back(std::move(inserted)...); }, inserted);

				return class_action::moveConstructed;
			};

			const auto compare = [&inserted](const auto& x) { return x == inserted; };

			return test(emplaceBackCopy, compare)
				&& test(emplaceBackMove, compare)
				&& test(pushBackCopy, compare)
				&& test(pushBackMove, compare);
		};

		return emplaceBackDirect() && emplaceAndPushBackCopyAndMove();

	}(std::make_index_sequence<std::tuple_size_v<Types>>{}, static_cast<Types*>(nullptr)); };

	AGNES_RUN(test);
}

TEMPLATE_TEST_CASE("agnes::vector::pop_back()", "",
	(std::tuple<std::tuple<recorded<int>               >, std::tuple<int     >>),
	(std::tuple<std::tuple<recorded<int>, recorded<int>>, std::tuple<int, int>>))
{
	using Types = std::tuple_element_t<0, TestType>;
	using Args  = std::tuple_element_t<1, TestType>;

	static constexpr auto test = [] { return []<typename... T>(std::tuple<T...>*)
	{
		const filled_rows<Types, Args, 2> initial;

		// [initial[0], initial[1]] -> [initial[0], unused]
		{
			agnes::vector<T...> v{{initial[0], initial[1]}};

			v.pop_back();

			AGNES_REQUIRE(v.size() == 1);

			AGNES_REQUIRE(v[0] == initial[0]);
		}

		return true;

	}(static_cast<Types*>(nullptr)); };

	AGNES_RUN(test);
}

TEMPLATE_TEST_CASE("agnes::vector::emplace() and insert() single", "",
	(std::tuple<std::tuple<recorded<int>                            >, std::tuple<int     >>),
	(std::tuple<std::tuple<recorded<int>, not_noexcept_moveable<int>>, std::tuple<int, int>>))
{
	using Types = std::tuple_element_t<0, TestType>;
	using Args  = std::tuple_element_t<1, TestType>;

	static constexpr auto test = [] { return []<std::size_t... Ti, typename... T>(
		std::index_sequence<Ti...>,
		std::tuple<T...>*)
	{
		const auto test = [](const auto& emplaceOrInsert, const auto& compare)
		{
			const filled_rows<Types, Args, 2> initial;

			const class_action expectedReallocatingAction[]{(std::is_nothrow_move_constructible_v<T> ?
				class_action::moveConstructed : class_action::copyConstructed)...};

			// [] -> [inserted]
			{
				agnes::vector<T...> v;

				const class_action expectedInsertAction{emplaceOrInsert(v, v.end())};

				AGNES_REQUIRE(v.size() == 1);

				AGNES_REQUIRE(compare(v[0]));

				AGNES_REQUIRE((get<Ti>(v[0]).actions.equals({expectedInsertAction}) && ...));
			}

			// [initial[0]] -> [inserted, initial[0]]
			{
				agnes::vector<T...> v{initial[0]};

				const class_action expectedInsertAction{emplaceOrInsert(v, v.begin())};

				AGNES_REQUIRE(v.size() == 2);

				AGNES_REQUIRE(compare(v[0]));
				AGNES_REQUIRE(v[1] == initial[0]);

				AGNES_REQUIRE((get<Ti>(v[0]).actions.equals({expectedInsertAction         }) && ...));
				AGNES_REQUIRE((get<Ti>(v[1]).actions.equals({expectedReallocatingAction[Ti]}) && ...));
			}

			// [initial[0], initial[1]] -> [initial[0], inserted, initial[1]]
			{
				agnes::vector<T...> v{initial[0], initial[1]};

				const class_action expectedInsertAction{emplaceOrInsert(v, v.begin() + 1)};

				AGNES_REQUIRE(v.size() == 3);

				AGNES_REQUIRE(v[0] == initial[0]);
				AGNES_REQUIRE(compare(v[1]));
				AGNES_REQUIRE(v[2] == initial[1]);

				AGNES_REQUIRE((get<Ti>(v[0]).actions.equals({expectedReallocatingAction[Ti]}) && ...));
				AGNES_REQUIRE((get<Ti>(v[1]).actions.equals({expectedInsertAction         }) && ...));
				AGNES_REQUIRE((get<Ti>(v[2]).actions.equals({expectedReallocatingAction[Ti]}) && ...));
			}

			// [initial[0]] -> [initial[0], inserted]
			{
				agnes::vector<T...> v{initial[0]};

				const class_action expectedInsertAction{emplaceOrInsert(v, v.end())};

				AGNES_REQUIRE(v.size() == 2);

				AGNES_REQUIRE(v[0] == initial[0]);
				AGNES_REQUIRE(compare(v[1]));

				AGNES_REQUIRE((get<Ti>(v[0]).actions.equals({expectedReallocatingAction[Ti]}) && ...));
				AGNES_REQUIRE((get<Ti>(v[1]).actions.equals({expectedInsertAction         }) && ...));
			}

			// [unused] -> [inserted]
			{
				agnes::vector<T...> v;

				v.reserve(1);

				const class_action expectedInsertAction{emplaceOrInsert(v, v.end())};

				AGNES_REQUIRE(v.size() == 1);

				AGNES_REQUIRE(compare(v[0]));

				AGNES_REQUIRE((get<Ti>(v[0]).actions.equals({expectedInsertAction}) && ...));
			}

			// [initial[0], unused] -> [inserted, initial[0]]
			{
				agnes::vector<T...> v;

				v.reserve(2);

				v.emplace_back(initial[0]);

				const class_action expectedInsertAction{emplaceOrInsert(v, v.begin())};

				AGNES_REQUIRE(v.size() == 2);

				AGNES_REQUIRE(compare(v[0]));
				AGNES_REQUIRE(v[1] == initial[0]);

				AGNES_REQUIRE((get<Ti>(v[0]).actions.equals({class_action::copyConstructed, class_action::moveAssigned}) && ...));
				AGNES_REQUIRE((get<Ti>(v[1]).actions.equals({class_action::moveConstructed                            }) && ...));
			}

			// [initial[0], initial[1], unused] -> [initial[0], inserted, initial[1]]
			{
				agnes::vector<T...> v;

				v.reserve(3);

				v.emplace_back(initial[0]);
				v.emplace_back(initial[1]);

				const class_action expectedInsertAction{emplaceOrInsert(v, v.begin() + 1)};

				AGNES_REQUIRE(v.size() == 3);

				AGNES_REQUIRE(v[0] == initial[0]);
				AGNES_REQUIRE(compare(v[1]));
				AGNES_REQUIRE(v[2] == initial[1]);

				AGNES_REQUIRE((get<Ti>(v[0]).actions.equals({class_action::copyConstructed                            }) && ...));
				AGNES_REQUIRE((get<Ti>(v[1]).actions.equals({class_action::copyConstructed, class_action::moveAssigned}) && ...));
				AGNES_REQUIRE((get<Ti>(v[2]).actions.equals({class_action::moveConstructed                            }) && ...));
			}

			// [initial[0], unused] -> [initial[0], inserted]
			{
				agnes::vector<T...> v;

				v.reserve(2);

				v.emplace_back(initial[0]);

				const class_action expectedInsertAction{emplaceOrInsert(v, v.end())};

				AGNES_REQUIRE(v.size() == 2);

				AGNES_REQUIRE(v[0] == initial[0]);
				AGNES_REQUIRE(compare(v[1]));

				AGNES_REQUIRE((get<Ti>(v[0]).actions.equals({class_action::copyConstructed }) && ...));
				AGNES_REQUIRE((get<Ti>(v[1]).actions.equals({expectedInsertAction          }) && ...));
			}

			return true;
		};

		const auto emplaceDirect = [&test]
		{
			const Args args{filled_value<Args , Args>{1}};

			const auto emplaceValues = [&args](auto& v, const auto& position)
			{
				std::apply([&v, &position](const auto&... args) { v.emplace(position, args...); }, args);

				return class_action::directConstructed;
			};

			const auto emplaceReference = [&args](auto& v, const auto& position)
			{
				std::apply([&v, &position](const auto&... args)
				{
					v.emplace(position, agnes::reference{args...});
				}, args);

				return class_action::directConstructed;
			};

			const auto emplacePiecewise = [&args](auto& v, const auto& position)
			{
				std::apply([&v, &position](const auto&... args)
				{
					v.emplace(position,  std::piecewise_construct, std::tuple{args}...);
				}, args);

				return class_action::directConstructed;
			};

			const auto compare = [&args](const auto& x) { return x == args; };

			return test(emplaceValues, compare) && test(emplaceReference, compare) && test(emplacePiecewise, compare);
		};

		const auto emplaceAndInsertCopyAndMove = [&test]
		{
			Types inserted{filled_value<Types, Args>{1}};

			const auto emplaceCopy = [&inserted](auto& v, const auto& position)
			{
				v.emplace(position, inserted);

				return class_action::copyConstructed;
			};

			const auto emplaceMove = [&inserted](auto& v, const auto& position)
			{
				v.emplace(position, std::move(inserted));

				return class_action::moveConstructed;
			};

			const auto insertCopy = [&inserted](auto& v, const auto& position)
			{
				v.insert(position, get<Ti>(inserted)...);

				return class_action::copyConstructed;
			};

			const auto insertMove = [&inserted](auto& v, const auto& position)
			{
				v.insert(position, get<Ti>(std::move(inserted))...);

				return class_action::moveConstructed;
			};

			const auto insertFill = [&inserted]<std::size_t N>()
			{
				return [&inserted](auto& v, const auto& position)
				{
					v.insert(position, get<Ti>(inserted)...);

					return class_action::copyConstructed;
				};
			};

			const auto compare = [&inserted](const auto& x) { return x == inserted; };

			return test(emplaceCopy, compare)
				&& test(emplaceMove, compare)
				&& test(insertCopy, compare)
				&& test(insertMove, compare)
				&& test(insertFill.template operator()<0>(), compare)
				&& test(insertFill.template operator()<1>(), compare)
				&& test(insertFill.template operator()<2>(), compare);
		};

		return emplaceDirect() && emplaceAndInsertCopyAndMove();

	}(std::make_index_sequence<std::tuple_size_v<Types>>{}, static_cast<Types*>(nullptr)); };

	AGNES_RUN(test);
}

TEMPLATE_TEST_CASE_SIG("agnes::vector::insert() multiple", "",
	((typename Types, typename Args, std::size_t N, std::size_t M), Types, Args, N, M),
	((std::tuple<recorded<int>                            >), (std::tuple<int     >), 1, 1),
	((std::tuple<recorded<int>                            >), (std::tuple<int     >), 2, 2),
	((std::tuple<recorded<int>                            >), (std::tuple<int     >), 3, 2),
	((std::tuple<recorded<int>, not_noexcept_moveable<int>>), (std::tuple<int, int>), 1, 1),
	((std::tuple<recorded<int>, not_noexcept_moveable<int>>), (std::tuple<int, int>), 2, 2),
	((std::tuple<recorded<int>, not_noexcept_moveable<int>>), (std::tuple<int, int>), 3, 2))
{
	static constexpr auto test = [] { return []<std::size_t... Ti, typename... T, std::size_t... I, std::size_t... J>(
		std::index_sequence<Ti...>,
		std::tuple<T...>*,
		std::index_sequence<I...>,
		std::index_sequence<J...>)
	{
		const auto test = [](const auto& insert, const auto& compare)
		{
			const filled_rows<Types, Args, N> initial;

			const class_action expectedReallocatingAction[]{(std::is_nothrow_move_constructible_v<T> ?
				class_action::moveConstructed : class_action::copyConstructed)...};

			// [] -> []
			{
				agnes::vector<T...> v;

				insert(v, v.end(), 0);

				AGNES_REQUIRE(v.size() == 0);
			}

			// [] -> [inserted[0]...inserted[M]]
			{
				agnes::vector<T...> v;

				insert(v, v.end(), M);

				AGNES_REQUIRE(v.size() == M);

				AGNES_REQUIRE(compare(v, 0));
			}

			// [initial[0]...initial[N]] -> [inserted[0]...inserted[M], initial[0]...initial[N]]
			{
				agnes::vector<T...> v{initial[I]...};

				insert(v, v.begin(), M);

				AGNES_REQUIRE(v.size() == N + M);

				AGNES_REQUIRE(compare(v, 0));
			}

			// [initial[0]...initial[N]] -> [initial[0]...initial[i], inserted[0]...inserted[M], initial[i]...initial[N]]
			for (auto i = std::size_t{1}; i < N; ++i)
			{
				agnes::vector<T...> v{initial[I]...};

				insert(v, v.begin() + i, M);

				AGNES_REQUIRE(v.size() == N + M);

				AGNES_REQUIRE(compare(v, i));
			}

			// [initial[0]...initial[N]] -> [initial[0]...initial[N], inserted[0]...inserted[M]]
			{
				agnes::vector<T...> v{initial[I]...};

				insert(v, v.end(), M);

				AGNES_REQUIRE(v.size() == N + M);

				AGNES_REQUIRE(compare(v, N));
			}

			// [unused[0]...unused[M]] -> [inserted[0]...inserted[M]]
			{
				agnes::vector<T...> v;

				v.reserve(M);

				insert(v, v.end(), M);

				AGNES_REQUIRE(v.size() == M);

				AGNES_REQUIRE(compare(v, 0));
			}

			// [initial[0]...initial[N], unused[0]...unused[M]] -> [inserted[0]...inserted[M], initial[0]...initial[N]]
			{
				agnes::vector<T...> v;

				v.reserve(N + M);

				v = {initial[I]...};

				insert(v, v.begin(), M);

				AGNES_REQUIRE(v.size() == N + M);

				AGNES_REQUIRE(compare(v, 0));
			}

			// [initial[0]...initial[N], unused[0]...unused[M]] -> [initial[0]...initial[i], inserted[0]...inserted[M], initial[i]...initial[N]]
			for (auto i = std::size_t{1}; i < N; ++i)
			{
				agnes::vector<T...> v;

				v.reserve(N + M);

				v = {initial[I]...};

				insert(v, v.begin() + i, M);

				AGNES_REQUIRE(v.size() == N + M);

				AGNES_REQUIRE(compare(v, i));
			}

			// [initial[0]...initial[N], unused[0]...unused[M]] -> [initial[0]...initial[N], inserted[0]...inserted[M]]
			{
				agnes::vector<T...> v;

				v.reserve(N + M);

				v = {initial[I]...};

				insert(v, v.end(), M);

				AGNES_REQUIRE(v.size() == N + M);

				AGNES_REQUIRE(compare(v, N));
			}

			return true;
		};

		const auto insertIterators = [&test]
		{
			const filled_columns<Types, Args, M> iterated;

			const auto insert = [&iterated](auto& v, const auto& position, const auto count)
			{
				const agnes::pointer first{get<Ti>(iterated).data()...};
				const agnes::pointer last {first + count};

				v.insert(position, first, last);

				return class_action::copyConstructed;
			};

			const auto compare = [&iterated](const auto& v, auto firstIndex)
			{
				for (auto i = std::size_t{0}; i < M; ++i)
				{
					if (v[firstIndex + i] != std::tuple{get<Ti>(iterated)[i]...})
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

			const auto insert = [&inserted](auto& v, const auto& position, const auto count)
			{
				if (count == 0)
				{
					v.insert(position, {});
				}
				else
				{
					v.insert(position, {inserted[J]...});
				}

				return class_action::copyConstructed;
			};

			const auto compare = [&inserted](const auto& v, const auto firstIndex)
			{
				return ((v[firstIndex + J] == inserted[J]) && ...);
			};

			return test(insert, compare);
		};

		return insertIterators() && insertInitializerList();

	}(std::make_index_sequence<std::tuple_size_v<Types>>{},
		static_cast<Types*>(nullptr),
		std::make_index_sequence<N>{},
		std::make_index_sequence<M>{}); };

	AGNES_RUN(test);
}

TEMPLATE_TEST_CASE("agnes::vector::erase() single", "",
	(std::tuple<std::tuple<recorded<int, -1>                   >, std::tuple<int     >>),
	(std::tuple<std::tuple<recorded<int, -1>, recorded<int, -2>>, std::tuple<int, int>>))
{
	using Types = std::tuple_element_t<0, TestType>;
	using Args  = std::tuple_element_t<1, TestType>;

	static constexpr auto test = [] { return []<std::size_t... Ti, typename... T>(
		std::index_sequence<Ti...>,
		std::tuple<T...>*)
	{
		const filled_rows<Types, Args, 3> initial;

		const auto test = [&initial](const auto& erase)
		{
			// [initial[0]] -> []
			{
				agnes::vector<T...> v{initial[0]};

				erase(v, v.begin());

				AGNES_REQUIRE(v.size() == 0);
			}

			// [initial[0], initial[1]] -> [initial[1]]
			{
				agnes::vector<T...> v{initial[0], initial[1]};

				erase(v, v.begin());

				AGNES_REQUIRE(v.size() == 1);

				AGNES_REQUIRE(v[0] == initial[1]);

				AGNES_REQUIRE((get<Ti>(v[0]).actions.equals({class_action::copyConstructed, class_action::moveAssigned}) && ...));
			}

			// [initial[0], initial[1]] -> [initial[0]]
			{
				agnes::vector<T...> v{initial[0], initial[1]};

				erase(v, v.begin() + 1);

				AGNES_REQUIRE(v.size() == 1);

				AGNES_REQUIRE(v[0] == initial[0]);

				AGNES_REQUIRE((get<Ti>(v[0]).actions.equals({class_action::copyConstructed}) && ...));
			}

			// [initial[0], initial[1], initial[2]] -> [initial[1], initial[2]]
			{
				agnes::vector<T...> v{initial[0], initial[1], initial[2]};

				erase(v, v.begin());

				AGNES_REQUIRE(v.size() == 2);

				AGNES_REQUIRE(v[0] == initial[1]);
				AGNES_REQUIRE(v[1] == initial[2]);

				AGNES_REQUIRE((get<Ti>(v[0]).actions.equals({class_action::copyConstructed, class_action::moveAssigned}) && ...));
				AGNES_REQUIRE((get<Ti>(v[1]).actions.equals({class_action::copyConstructed, class_action::moveAssigned}) && ...));
			}

			// [initial[0], initial[1], initial[2]] -> [initial[0], initial[2]]
			{
				agnes::vector<T...> v{initial[0], initial[1], initial[2]};

				erase(v, v.begin() + 1);

				AGNES_REQUIRE(v.size() == 2);

				AGNES_REQUIRE(v[0] == initial[0]);
				AGNES_REQUIRE(v[1] == initial[2]);

				AGNES_REQUIRE((get<Ti>(v[0]).actions.equals({class_action::copyConstructed                            }) && ...));
				AGNES_REQUIRE((get<Ti>(v[1]).actions.equals({class_action::copyConstructed, class_action::moveAssigned}) && ...));
			}

			// [initial[0], initial[1], initial[2]] -> [initial[0], initial[1]]
			{
				agnes::vector<T...> v{initial[0], initial[1], initial[2]};

				erase(v, v.begin() + 2);

				AGNES_REQUIRE(v.size() == 2);

				AGNES_REQUIRE(v[0] == initial[0]);
				AGNES_REQUIRE(v[1] == initial[1]);

				AGNES_REQUIRE((get<Ti>(v[0]).actions.equals({class_action::copyConstructed}) && ...));
				AGNES_REQUIRE((get<Ti>(v[1]).actions.equals({class_action::copyConstructed}) && ...));
			}

			return true;
		};

		const auto erase      = [](auto& v, const auto& position) { v.erase(position              ); };
		const auto eraseRange = [](auto& v, const auto& position) { v.erase(position, position + 1); };

		return test(erase) && test(eraseRange);

	}(std::make_index_sequence<std::tuple_size_v<Types>>{}, static_cast<Types*>(nullptr)); };

	AGNES_RUN(test);
}

TEMPLATE_TEST_CASE("agnes::vector::erase() multiple", "",
	(std::tuple<std::tuple<recorded<int, -1>                   >, std::tuple<int     >>),
	(std::tuple<std::tuple<recorded<int, -1>, recorded<int, -2>>, std::tuple<int, int>>))
{
	using Types = std::tuple_element_t<0, TestType>;
	using Args  = std::tuple_element_t<1, TestType>;

	static constexpr auto test = [] { return []<std::size_t... Ti, typename... T>(
		std::index_sequence<Ti...>,
		std::tuple<T...>*)
	{
		const filled_rows<Types, Args, 4> initial;

		// [initial[0], initial[1]] -> []
		{
			agnes::vector<T...> v{initial[0], initial[1]};

			v.erase(v.begin(), v.end());

			AGNES_REQUIRE(v.size() == 0);
		}

		// [initial[0], initial[1], initial[2], initial[3]] -> [initial[2], initial[3]]
		{
			agnes::vector<T...> v{initial[0], initial[1], initial[2], initial[3]};

			v.erase(v.begin(), v.begin() + 2);

			AGNES_REQUIRE(v.size() == 2);

			AGNES_REQUIRE((get<Ti>(v[0]).actions.equals({class_action::copyConstructed, class_action::moveAssigned}) && ...));
			AGNES_REQUIRE((get<Ti>(v[1]).actions.equals({class_action::copyConstructed, class_action::moveAssigned}) && ...));
		}

		// [initial[0], initial[1], initial[2], initial[3]] -> [initial[0], initial[3]]
		{
			agnes::vector<T...> v{initial[0], initial[1], initial[2], initial[3]};

			v.erase(v.begin() + 1, v.begin() + 3);

			AGNES_REQUIRE(v.size() == 2);

			AGNES_REQUIRE((get<Ti>(v[0]).actions.equals({class_action::copyConstructed                            }) && ...));
			AGNES_REQUIRE((get<Ti>(v[1]).actions.equals({class_action::copyConstructed, class_action::moveAssigned}) && ...));
		}

		// [initial[0], initial[1], initial[2], initial[3]] -> [initial[0], initial[1]]
		{
			agnes::vector<T...> v{initial[0], initial[1], initial[2], initial[3]};

			v.erase(v.begin() + 2, v.end());

			AGNES_REQUIRE(v.size() == 2);

			AGNES_REQUIRE((get<Ti>(v[0]).actions.equals({class_action::copyConstructed}) && ...));
			AGNES_REQUIRE((get<Ti>(v[1]).actions.equals({class_action::copyConstructed}) && ...));
		}

		return true;

	}(std::make_index_sequence<std::tuple_size_v<Types>>{}, static_cast<Types*>(nullptr)); };

	AGNES_RUN(test);
}

TEMPLATE_TEST_CASE("agnes::vector::swap()", "",
	(std::tuple<int     >),
	(std::tuple<int, int>))
{
	using Types = TestType;

	static constexpr auto test = [] { return []<typename... T>(std::tuple<T...>*)
	{
		const auto test = [](const auto& swap_)
		{
			const filled_rows<Types, Types,2> initial0{0};
			const filled_rows<Types, Types,2> initial1{1};

			{
				agnes::vector<T...> v0;
				agnes::vector<T...> v1;

				const auto prevData0{v0.data()};
				const auto prevData1{v1.data()};

				swap_(v1, v0);

				AGNES_REQUIRE(v0.empty());
				AGNES_REQUIRE(v1.empty());

				AGNES_REQUIRE(v0.data() == prevData1);
				AGNES_REQUIRE(v1.data() == prevData0);
			}

			{
				agnes::vector<T...> v0;
				agnes::vector<T...> v1{initial1[0]};

				const auto prevData0{v0.data()};
				const auto prevData1{v1.data()};

				swap_(v1, v0);

				AGNES_REQUIRE(v0.size() == 1);
				AGNES_REQUIRE(v1.empty());

				AGNES_REQUIRE(v0.data() == prevData1);
				AGNES_REQUIRE(v1.data() == prevData0);
			}

			{
				agnes::vector<T...> v0;
				agnes::vector<T...> v1{initial1[0], initial1[1]};

				const auto prevData0{v0.data()};
				const auto prevData1{v1.data()};

				swap_(v1, v0);

				AGNES_REQUIRE(v0.size() == 2);
				AGNES_REQUIRE(v1.empty());

				AGNES_REQUIRE(v0.data() == prevData1);
				AGNES_REQUIRE(v1.data() == prevData0);
			}

			{
				agnes::vector<T...> v0{initial0[0]};
				agnes::vector<T...> v1;

				const auto prevData0{v0.data()};
				const auto prevData1{v1.data()};

				swap_(v1, v0);

				AGNES_REQUIRE(v0.empty());
				AGNES_REQUIRE(v1.size() == 1);

				AGNES_REQUIRE(v0.data() == prevData1);
				AGNES_REQUIRE(v1.data() == prevData0);
			}

			{
				agnes::vector<T...> v0{initial0[0]};
				agnes::vector<T...> v1{initial1[0]};

				const auto prevData0{v0.data()};
				const auto prevData1{v1.data()};

				swap_(v1, v0);

				AGNES_REQUIRE(v0.size() == 1);
				AGNES_REQUIRE(v1.size() == 1);

				AGNES_REQUIRE(v0.data() == prevData1);
				AGNES_REQUIRE(v1.data() == prevData0);
			}

			{
				agnes::vector<T...> v0{initial0[0]};
				agnes::vector<T...> v1{initial1[0], initial1[1]};

				const auto prevData0{v0.data()};
				const auto prevData1{v1.data()};

				swap_(v1, v0);

				AGNES_REQUIRE(v0.size() == 2);
				AGNES_REQUIRE(v1.size() == 1);

				AGNES_REQUIRE(v0.data() == prevData1);
				AGNES_REQUIRE(v1.data() == prevData0);
			}

			{
				agnes::vector<T...> v0{initial0[0], initial0[1]};
				agnes::vector<T...> v1;

				const auto prevData0{v0.data()};
				const auto prevData1{v1.data()};

				swap_(v1, v0);

				AGNES_REQUIRE(v0.empty());
				AGNES_REQUIRE(v1.size() == 2);

				AGNES_REQUIRE(v0.data() == prevData1);
				AGNES_REQUIRE(v1.data() == prevData0);
			}

			{
				agnes::vector<T...> v0{initial0[0], initial0[1]};
				agnes::vector<T...> v1{initial1[0]};

				const auto prevData0{v0.data()};
				const auto prevData1{v1.data()};

				swap_(v1, v0);

				AGNES_REQUIRE(v0.size() == 1);
				AGNES_REQUIRE(v1.size() == 2);

				AGNES_REQUIRE(v0.data() == prevData1);
				AGNES_REQUIRE(v1.data() == prevData0);
			}

			{
				agnes::vector<T...> v0{initial0[0], initial0[1]};
				agnes::vector<T...> v1{initial1[0], initial1[1]};

				const auto prevData0{v0.data()};
				const auto prevData1{v1.data()};

				swap_(v1, v0);

				AGNES_REQUIRE(v0.size() == 2);
				AGNES_REQUIRE(v1.size() == 2);

				AGNES_REQUIRE(v0.data() == prevData1);
				AGNES_REQUIRE(v1.data() == prevData0);
			}

			return true;
		};

		const auto memberSwap    = [](auto& x, auto& y) { x.swap(y); };
		const auto namespaceSwap = [](auto& x, auto& y) { swap(x, y); };

		return test(memberSwap) and test(namespaceSwap);

	}(static_cast<Types*>(nullptr)); };

	AGNES_RUN(test);
}

TEMPLATE_TEST_CASE("agnes::vector::clear()", "",
	(std::tuple<int     >),
	(std::tuple<int, int>))
{
	using Types = TestType;

	static constexpr auto test = [] { return []<typename... T>(std::tuple<T...>*)
	{
		const filled_rows<Types, Types, 2> initial;

		{
			agnes::vector<T...> v;

			v.clear();

			AGNES_REQUIRE(v.empty());
		}

		{
			agnes::vector<T...> v{initial[0]};

			v.clear();

			AGNES_REQUIRE(v.empty());
		}

		{
			agnes::vector<T...> v{initial[0], initial[1]};

			v.clear();

			AGNES_REQUIRE(v.empty());
		}

		return true;

	}(static_cast<Types*>(nullptr)); };

	AGNES_RUN(test);
}

TEMPLATE_TEST_CASE("agnes::vector compare", "",
	(std::tuple<int     >),
	(std::tuple<int, int>))
{
	using Types = TestType;

	static constexpr auto test = [] { return []<typename... T>(std::tuple<T...>*)
	{
		const filled_rows<Types, Types, 2> initialLess   {0};
		const filled_rows<Types, Types, 2> initialEqual  {1};
		const filled_rows<Types, Types, 2> initialGreater{2};

		const agnes::vector<T...> v      {initialEqual  [0], initialEqual  [1]};
		const agnes::vector<T...> less   {initialLess   [0], initialLess   [1]};
		const agnes::vector<T...> equal  {initialEqual  [0], initialEqual  [1]};
		const agnes::vector<T...> greater{initialGreater[0], initialGreater[1]};

		AGNES_REQUIRE(!(v <  v));
		AGNES_REQUIRE(  v <= v );
		AGNES_REQUIRE(  v == v );
		AGNES_REQUIRE(!(v != v));
		AGNES_REQUIRE(  v >= v );
		AGNES_REQUIRE(!(v >  v));

		AGNES_REQUIRE(!(v <  less));
		AGNES_REQUIRE(!(v <= less));
		AGNES_REQUIRE(!(v == less));
		AGNES_REQUIRE(  v != less );
		AGNES_REQUIRE(  v >= less );
		AGNES_REQUIRE(  v >  less );

		AGNES_REQUIRE(!(v <  equal));
		AGNES_REQUIRE(  v <= equal );
		AGNES_REQUIRE(  v == equal );
		AGNES_REQUIRE(!(v != equal));
		AGNES_REQUIRE(  v >= equal );
		AGNES_REQUIRE(!(v >  equal));

		AGNES_REQUIRE(  v <  greater );
		AGNES_REQUIRE(  v <= greater );
		AGNES_REQUIRE(!(v == greater));
		AGNES_REQUIRE(  v != greater );
		AGNES_REQUIRE(!(v >= greater));
		AGNES_REQUIRE(!(v >  greater));

		return true;

	}(static_cast<Types*>(nullptr)); };

	AGNES_RUN(test);
}

TEMPLATE_TEST_CASE("agnes::vector namespace erase() and erase_if()", "",
	(std::tuple<int     >),
	(std::tuple<int, int>))
{
	using Types = TestType;

	static constexpr auto test = [] { return []<std::size_t... Ti, typename... T>(
		std::index_sequence<Ti...>, std::tuple<T...>*)
	{
		const filled_rows<Types, Types, 2> initial;

		const auto test = [&initial](const auto& erase_)
		{
			agnes::vector<T...> v{initial[0], initial[1]};

			const auto count = erase_(v, agnes::reference{std::get<Ti>(initial[0])...});

			AGNES_REQUIRE(count == 1);
			AGNES_REQUIRE(v[0] == initial[1]);

			return true;
		};

		const auto namespaceErase = [](auto& v, const auto& value)
		{
			return erase(v, value);
		};

		const auto namespaceEraseIf = [](auto& v, const auto& value)
		{
			return erase_if(v, [&value](const auto& elem) { return ((get<Ti>(elem) == get<Ti>(value)) && ...); });
		};

		return test(namespaceErase) && test(namespaceEraseIf);

	}(std::make_index_sequence<std::tuple_size_v<Types>>{}, static_cast<Types*>(nullptr)); };

	AGNES_RUN(test);
}
