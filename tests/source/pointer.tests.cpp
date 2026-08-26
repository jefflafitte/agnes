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

#include <agnes/pointer.h>

#include <cstddef>
#include <type_traits>
#include <utility>

#include "agnes_tests/base.h"
#include "agnes_tests/derived.h"
#include "agnes_tests/filled_value.h"
#include "agnes_tests/macros.h"

template <typename Pointer, typename Fill, std::size_t Count>
struct fill_agnes_pointer;

template <typename Fill, typename... T>
struct fill_agnes_pointer<agnes::pointer<T...>, Fill, 0> :
	std::type_identity<agnes::pointer<T...>> {};

template <typename Fill, std::size_t Count, typename... T>
struct fill_agnes_pointer<agnes::pointer<T...>, Fill, Count> :
	fill_agnes_pointer<agnes::pointer<T..., Fill>, Fill, Count - 1> {};

template <typename Pointer, typename Fill, std::size_t Count>
using fill_agnes_pointer_t = typename fill_agnes_pointer<Pointer, Fill, Count>::type;

using namespace agnes_tests;

TEMPLATE_TEST_CASE("agnes::pointer<T*...> direct construction", "",
	(std::tuple<      int ,       int    >),
	(std::tuple<const int , const int    >),
	(std::tuple<const int ,       int    >),
	(std::tuple<      base,       derived>),
	(std::tuple<const base, const derived>),
	(std::tuple<const base,       derived>))
{
	using PointerValueType = std::tuple_element_t<0, TestType>;
	using OtherValueType   = std::tuple_element_t<1, TestType>;

	if constexpr (std::is_const_v<PointerValueType> && std::is_const_v<OtherValueType>)
	{
		static constexpr OtherValueType value{0};

		static constexpr PointerValueType*                 cppPtr  {&value};
		static constexpr agnes::pointer<PointerValueType*> agnesPtr{&value};

		static_assert(cppPtr           == &value);
		static_assert(get<0>(agnesPtr) == &value);
	}

	static constexpr auto test = []
	{
		OtherValueType value{0};

		PointerValueType*                 cppPtr  {&value};
		agnes::pointer<PointerValueType*> agnesPtr{&value};

		AGNES_REQUIRE(cppPtr           == &value);
		AGNES_REQUIRE(get<0>(agnesPtr) == &value);

		return true;
	};

	AGNES_RUN(test);
}

TEMPLATE_TEST_CASE("agnes::pointer<T*...> copy construction", "",
	(std::tuple<      int ,       int    >),
	(std::tuple<const int , const int    >),
	(std::tuple<const int ,       int    >),
	(std::tuple<      base,       derived>),
	(std::tuple<const base, const derived>),
	(std::tuple<const base,       derived>))
{
	using PointerValueType = std::tuple_element_t<0, TestType>;
	using OtherValueType   = std::tuple_element_t<1, TestType>;

	if constexpr (std::is_const_v<PointerValueType> && std::is_const_v<OtherValueType>)
	{
		static constexpr OtherValueType                  value     {0};
		static constexpr OtherValueType*                 cppOther  {&value};
		static constexpr agnes::pointer<OtherValueType*> agnesOther{&value};

		static constexpr PointerValueType*                 cppPtr  {cppOther};
		static constexpr agnes::pointer<PointerValueType*> agnesPtr{agnesOther};

		static_assert(cppPtr           == &value);
		static_assert(get<0>(agnesPtr) == &value);
	}

	static constexpr auto test = []
	{
		OtherValueType                  value     {0};
		OtherValueType*                 cppOther  {&value};
		agnes::pointer<OtherValueType*> agnesOther{&value};

		PointerValueType*                 cppPtr  {cppOther};
		agnes::pointer<PointerValueType*> agnesPtr{agnesOther};

		AGNES_REQUIRE(cppPtr           == &value);
		AGNES_REQUIRE(get<0>(agnesPtr) == &value);

		return true;
	};

	AGNES_RUN(test);
}

TEMPLATE_TEST_CASE("agnes::pointer<T*...> move construction", "",
	(std::tuple<      int ,       int    >),
	(std::tuple<const int , const int    >),
	(std::tuple<const int ,       int    >),
	(std::tuple<      base,       derived>),
	(std::tuple<const base, const derived>),
	(std::tuple<const base,       derived>))
{
	using PointerValueType = std::tuple_element_t<0, TestType>;
	using OtherValueType   = std::tuple_element_t<1, TestType>;

	if constexpr (std::is_const_v<PointerValueType> && std::is_const_v<OtherValueType>)
	{
		static constexpr OtherValueType                  value     {0};
		static constexpr OtherValueType*                 cppOther  {&value};
		static constexpr agnes::pointer<OtherValueType*> agnesOther{&value};

		static constexpr PointerValueType*                 cppPtr  {std::move(cppOther)};
		static constexpr agnes::pointer<PointerValueType*> agnesPtr{std::move(agnesOther)};

		static_assert(cppPtr           == &value);
		static_assert(get<0>(agnesPtr) == &value);
	}

	static constexpr auto test = []
	{
		OtherValueType                  value     {0};
		OtherValueType*                 cppOther  {&value};
		agnes::pointer<OtherValueType*> agnesOther{&value};

		PointerValueType*                 cppPtr  {std::move(cppOther)};
		agnes::pointer<PointerValueType*> agnesPtr{std::move(agnesOther)};

		AGNES_REQUIRE(cppPtr           == &value);
		AGNES_REQUIRE(get<0>(agnesPtr) == &value);

		return true;
	};

	AGNES_RUN(test);
}

TEMPLATE_TEST_CASE("agnes::pointer<T*...> copy assignment", "",
	(std::tuple<      int ,       int    >),
	(std::tuple<const int , const int    >),
	(std::tuple<const int ,       int    >),
	(std::tuple<      base,       derived>),
	(std::tuple<const base, const derived>),
	(std::tuple<const base,       derived>))
{
	using PointerValueType = std::tuple_element_t<0, TestType>;
	using OtherValueType   = std::tuple_element_t<1, TestType>;

	static constexpr auto test = []
	{
		PointerValueType                value     {0};
		OtherValueType                  otherValue{1};
		OtherValueType*                 cppOther  {&otherValue};
		agnes::pointer<OtherValueType*> agnesOther{&otherValue};

		PointerValueType*                 cppPtr  {&value};
		agnes::pointer<PointerValueType*> agnesPtr{&value};

		cppPtr   = cppOther;
		agnesPtr = agnesOther;

		AGNES_REQUIRE(cppPtr           == &otherValue);
		AGNES_REQUIRE(get<0>(agnesPtr) == &otherValue);

		return true;
	};

	AGNES_RUN(test);
}

TEMPLATE_TEST_CASE("agnes::pointer<T*...> move assignment", "",
	(std::tuple<      int ,       int    >),
	(std::tuple<const int , const int    >),
	(std::tuple<const int ,       int    >),
	(std::tuple<      base,       derived>),
	(std::tuple<const base, const derived>),
	(std::tuple<const base,       derived>))
{
	using PointerValueType = std::tuple_element_t<0, TestType>;
	using OtherValueType   = std::tuple_element_t<1, TestType>;

	static constexpr auto test = []
	{
		PointerValueType                value     {0};
		OtherValueType                  otherValue{1};
		OtherValueType*                 cppOther  {&otherValue};
		agnes::pointer<OtherValueType*> agnesOther{&otherValue};

		PointerValueType*                 cppPtr  {&value};
		agnes::pointer<PointerValueType*> agnesPtr{&value};

		cppPtr   = std::move(cppOther);
		agnesPtr = std::move(agnesOther);

		AGNES_REQUIRE(cppPtr           == &otherValue);
		AGNES_REQUIRE(get<0>(agnesPtr) == &otherValue);

		return true;
	};

	AGNES_RUN(test);
}

TEST_CASE("agnes::pointer<T*...> increment/decrement")
{
	static constexpr auto test = []
	{
		int values[]{0, 1};

		int*                 cppPtr  {&values[0]};
		agnes::pointer<int*> agnesPtr{&values[0]};

		{
			int*                 cppResult   = cppPtr++;
			agnes::pointer<int*> agnesResult = agnesPtr++;

			AGNES_REQUIRE(cppResult           == &values[0]);
			AGNES_REQUIRE(get<0>(agnesResult) == &values[0]);

			AGNES_REQUIRE(cppPtr           == &values[1]);
			AGNES_REQUIRE(get<0>(agnesPtr) == &values[1]);
		}

		{
			int*                 cppResult   = cppPtr--;
			agnes::pointer<int*> agnesResult = agnesPtr--;

			AGNES_REQUIRE(cppResult           == &values[1]);
			AGNES_REQUIRE(get<0>(agnesResult) == &values[1]);

			AGNES_REQUIRE(cppPtr           == &values[0]);
			AGNES_REQUIRE(get<0>(agnesPtr) == &values[0]);
		}

		{
			int*&                 cppResult   = ++cppPtr;
			agnes::pointer<int*>& agnesResult = ++agnesPtr;

			AGNES_REQUIRE(&cppResult   == &cppResult);
			AGNES_REQUIRE(&agnesResult == &agnesResult);

			AGNES_REQUIRE(cppPtr           == &values[1]);
			AGNES_REQUIRE(get<0>(agnesPtr) == &values[1]);
		}

		{
			int*&                 cppResult   = --cppPtr;
			agnes::pointer<int*>& agnesResult = --agnesPtr;

			AGNES_REQUIRE(&cppResult   == &cppResult);
			AGNES_REQUIRE(&agnesResult == &agnesResult);

			AGNES_REQUIRE(cppPtr           == &values[0]);
			AGNES_REQUIRE(get<0>(agnesPtr) == &values[0]);
		}

		return true;
	};

	AGNES_RUN(test);
}

TEST_CASE("agnes::pointer<T*...> bool conversion")
{
	{
		static constexpr int value{0};

		static constexpr const int*                 cppPtr      {&value};
		static constexpr agnes::pointer<const int*> agnesPtr    {&value};
		static constexpr const int*                 cppNullPtr  {nullptr};
		static constexpr agnes::pointer<const int*> agnesNullPtr{nullptr};

		static_assert(cppPtr);
		static_assert(agnesPtr);
		static_assert(!cppNullPtr);
		static_assert(!agnesNullPtr);
		static_assert(!(!cppPtr));
		static_assert(!(!agnesPtr));
		static_assert(!cppNullPtr);
		static_assert(!cppNullPtr);
	}

	static constexpr auto test = []
	{
		int value{0};

		int*                 cppPtr      {&value};
		agnes::pointer<int*> agnesPtr    {&value};
		int*                 cppNullPtr  {nullptr};
		agnes::pointer<int*> agnesNullPtr{nullptr};

		AGNES_REQUIRE(cppPtr);
		AGNES_REQUIRE(agnesPtr);
		AGNES_REQUIRE(!cppNullPtr);
		AGNES_REQUIRE(!agnesNullPtr);
		AGNES_REQUIRE(!(!cppPtr));
		AGNES_REQUIRE(!(!agnesPtr));
		AGNES_REQUIRE(!cppNullPtr);
		AGNES_REQUIRE(!cppNullPtr);

		return true;
	};

	AGNES_RUN(test);
}

TEST_CASE("agnes::pointer<T*...> subscript")
{
	{
		static constexpr int values[]{0, 1};

		static constexpr const int*                 cppPtr      {&values[0]};
		static constexpr agnes::pointer<const int*> agnesPtr    {&values[0]};
		static constexpr const int*                 cppNextPtr  {&values[1]};
		static constexpr agnes::pointer<const int*> agnesNextPtr{&values[1]};

		static_assert(&cppPtr[0]           == &values[0]);
		static_assert(&get<0>(agnesPtr[0]) == &values[0]);

		static_assert(&cppPtr[1]           == &values[1]);
		static_assert(&get<0>(agnesPtr[1]) == &values[1]);

		static_assert(&cppNextPtr[-1]           == &values[0]);
		static_assert(&get<0>(agnesNextPtr[-1]) == &values[0]);
	}

	static constexpr auto test = []
	{
		int values[]{0, 1};

		int*                 cppPtr      {&values[0]};
		agnes::pointer<int*> agnesPtr    {&values[0]};
		int*                 cppNextPtr  {&values[1]};
		agnes::pointer<int*> agnesNextPtr{&values[1]};

		AGNES_REQUIRE(&cppPtr[0]           == &values[0]);
		AGNES_REQUIRE(&get<0>(agnesPtr[0]) == &values[0]);

		AGNES_REQUIRE(&cppPtr[1]           == &values[1]);
		AGNES_REQUIRE(&get<0>(agnesPtr[1]) == &values[1]);

		AGNES_REQUIRE(&cppNextPtr[-1]           == &values[0]);
		AGNES_REQUIRE(&get<0>(agnesNextPtr[-1]) == &values[0]);

		return true;
	};

	AGNES_RUN(test);
}

TEST_CASE("agnes::pointer<T*...> indirection")
{
	{
		static constexpr int value{0};

		static constexpr const int*                 cppPtr  {&value};
		static constexpr agnes::pointer<const int*> agnesPtr{&value};

		static_assert(&(*cppPtr)           == &value);
		static_assert(&(*get<0>(agnesPtr)) == &value);
	}

	static constexpr auto test = []
	{
		int value{0};

		int*                 cppPtr  {&value};
		agnes::pointer<int*> agnesPtr{&value};

		AGNES_REQUIRE(&(*cppPtr)           == &value);
		AGNES_REQUIRE(&(get<0>(*agnesPtr)) == &value);

		return true;
	};

	AGNES_RUN(test);
}

TEST_CASE("agnes::pointer<T*...> swap")
{
	static constexpr auto test = []
	{
		int lhsValue{0};
		int rhsValue{1};

		int*                 cppLhsPtr  {&lhsValue};
		agnes::pointer<int*> agnesLhsPtr{&lhsValue};
		int*                 cppRhsPtr  {&rhsValue};
		agnes::pointer<int*> agnesRhsPtr{&rhsValue};

		agnesLhsPtr.swap(agnesRhsPtr);

		AGNES_REQUIRE(get<0>(agnesLhsPtr) == &rhsValue);
		AGNES_REQUIRE(get<0>(agnesRhsPtr) == &lhsValue);

		agnesLhsPtr.swap(agnesRhsPtr);

		using std::swap;

		swap(cppLhsPtr, cppRhsPtr);
		swap(agnesLhsPtr, agnesRhsPtr);

		AGNES_REQUIRE(get<0>(agnesLhsPtr) == &rhsValue);
		AGNES_REQUIRE(get<0>(agnesRhsPtr) == &lhsValue);

		return true;
	};

	AGNES_RUN(test);
}

TEST_CASE("agnes::pointer<T*...> add/subtract")
{
	{
		static constexpr int values[]{0, 1};

		static constexpr const int*                 cppPtr      {&values[0]};
		static constexpr agnes::pointer<const int*> agnesPtr    {&values[0]};
		static constexpr const int*                 cppNextPtr  {&values[1]};
		static constexpr agnes::pointer<const int*> agnesNextPtr{&values[1]};

		static_assert((cppPtr + 1)   == cppNextPtr);
		static_assert((agnesPtr + 1) == agnesNextPtr);

		static_assert((1 + cppPtr)   == cppNextPtr);
		static_assert((1 + agnesPtr) == agnesNextPtr);

		static_assert((cppNextPtr - 1)   == cppPtr);
		static_assert((agnesNextPtr - 1) == agnesPtr);

		static_assert((cppNextPtr - cppPtr)     == 1);
		static_assert((agnesNextPtr - agnesPtr) == 1);

		static_assert((cppPtr - cppNextPtr)     == -1);
		static_assert((agnesPtr - agnesNextPtr) == -1);
	}

	static constexpr auto test = []
	{
		int values[]{0, 1};

		int*                 cppPtr      {&values[0]};
		agnes::pointer<int*> agnesPtr    {&values[0]};
		int*                 cppNextPtr  {&values[1]};
		agnes::pointer<int*> agnesNextPtr{&values[1]};

		AGNES_REQUIRE((cppPtr + 1)   == cppNextPtr);
		AGNES_REQUIRE((agnesPtr + 1) == agnesNextPtr);

		AGNES_REQUIRE((1 + cppPtr)   == cppNextPtr);
		AGNES_REQUIRE((1 + agnesPtr) == agnesNextPtr);

		AGNES_REQUIRE((cppNextPtr - 1)   == cppPtr);
		AGNES_REQUIRE((agnesNextPtr - 1) == agnesPtr);

		AGNES_REQUIRE((cppNextPtr - cppPtr)     == 1);
		AGNES_REQUIRE((agnesNextPtr - agnesPtr) == 1);

		AGNES_REQUIRE((cppPtr - cppNextPtr)     == -1);
		AGNES_REQUIRE((agnesPtr - agnesNextPtr) == -1);

		return true;
	};

	AGNES_RUN(test);
}

TEST_CASE("agnes::pointer<T*...> compare")
{
	{
		static constexpr int values[]{0, 1};

		static constexpr agnes::pointer<const int*> lhs0{&values[0]};
		static constexpr agnes::pointer<const int*> lhs1{&values[1]};
		static constexpr agnes::pointer<const int*> rhs0{&values[0]};
		static constexpr agnes::pointer<const int*> rhs1{&values[1]};

		static_assert(lhs0 <= lhs0);
		static_assert(lhs0 == lhs0);
		static_assert(lhs0 >= lhs0);

		static_assert(lhs0 <  rhs1);
		static_assert(lhs0 <= rhs1);
		static_assert(lhs0 != rhs1);

		static_assert(lhs1 >  rhs0);
		static_assert(lhs1 >= rhs0);
	}

	static constexpr auto test = []
	{
		int values[]{0, 1};

		agnes::pointer<int*> lhs0{&values[0]};
		agnes::pointer<int*> lhs1{&values[1]};
		agnes::pointer<int*> rhs0{&values[0]};
		agnes::pointer<int*> rhs1{&values[1]};

		AGNES_REQUIRE(lhs0 != nullptr);

		AGNES_REQUIRE(lhs0 <= lhs0);
		AGNES_REQUIRE(lhs0 == lhs0);
		AGNES_REQUIRE(lhs0 >= lhs0);

		AGNES_REQUIRE(lhs0 <  rhs1);
		AGNES_REQUIRE(lhs0 <= rhs1);
		AGNES_REQUIRE(lhs0 != rhs1);

		AGNES_REQUIRE(lhs1 >  rhs0);
		AGNES_REQUIRE(lhs1 >= rhs0);

		return true;
	};

	AGNES_RUN(test);
}

TEMPLATE_TEST_CASE("agnes::pointer<T*...> deduction guides", "", int, const int)
{
	if constexpr (std::is_const_v<TestType>)
	{
		static constexpr TestType                        value {0};
		static constexpr agnes::pointer<TestType*>       other {&value};
		static constexpr const agnes::pointer<TestType*> otherC{&value};

		static constexpr agnes::pointer agnesPtr0{&value};
		static constexpr agnes::pointer agnesPtr1{other};
		static constexpr agnes::pointer agnesPtr2{otherC};

		static_assert(std::is_same_v<decltype(agnesPtr0), const agnes::pointer<TestType*>>);
		static_assert(std::is_same_v<decltype(agnesPtr1), const agnes::pointer<TestType*>>);
		static_assert(std::is_same_v<decltype(agnesPtr2), const agnes::pointer<TestType*>>);
	}

	static constexpr auto test = []
	{
		TestType                        value {0};
		agnes::pointer<TestType*>       other {&value};
		const agnes::pointer<TestType*> otherC{&value};

		agnes::pointer agnesPtr0{&value};
		agnes::pointer agnesPtr1{other};
		agnes::pointer agnesPtr2{otherC};

		static_assert(std::is_same_v<decltype(agnesPtr0), agnes::pointer<TestType*>>);
		static_assert(std::is_same_v<decltype(agnesPtr1), agnes::pointer<TestType*>>);
		static_assert(std::is_same_v<decltype(agnesPtr2), agnes::pointer<TestType*>>);

		return true;
	};

	AGNES_RUN(test);
}

TEST_CASE("agnes::pointer<T*...> apply")
{
	static constexpr auto test = []
	{
		int  values[]{0, 1};
		bool applied {false};
		auto f       {[&applied](int*, int*) { applied = true; }};

		agnes::pointer<int*, int*> agnesPtr{&values[0], &values[1]};

		agnes::apply(f, agnesPtr);

		AGNES_REQUIRE(applied);

		return true;
	};

	AGNES_RUN(test);
}

TEMPLATE_TEST_CASE_SIG("agnes::pointer<T*...> subset", "",
	((typename Types, std::size_t... Subset), Types, Subset...),
	((std::tuple<int               >), 0   ),
	((std::tuple<int, float        >), 1   ),
	((std::tuple<int, float, double>), 0, 2))
{
	static constexpr auto test = [] { return []<std::size_t... Ti, typename... T, std::size_t... I>(
		std::index_sequence<Ti...>,
		std::tuple<T...>*,
		std::index_sequence<I...>)
	{
		Types values{filled_value<Types>{}};

		const agnes::pointer<T*...> p{&get<Ti>(values)...};

		const agnes::pointer_subset_t<agnes::pointer<T*...>, Subset...> sub{subset<Subset...>(p)};

		AGNES_REQUIRE(((get<Subset>(p) == get<I>(sub)) && ...));

		return true;

	}(std::make_index_sequence<std::tuple_size_v<Types>>{},
		static_cast<Types*>(nullptr),
		std::make_index_sequence<sizeof...(Subset)>{}); };

	AGNES_RUN(test);
}

TEST_CASE("agnes::pointer<T*...> type composition")
{
	std::tuple<int, float> t;

	using const_homogeneous_pointer1    =                      agnes::pointer<const int*>;
	using const_homogeneous_pointer2    = fill_agnes_pointer_t<agnes::pointer<const int*>  , const int*, 1>;
	using const_homogeneous_pointer4    = fill_agnes_pointer_t<agnes::pointer<const int*>  , const int*, 3>;
	using const_homogeneous_pointer8    = fill_agnes_pointer_t<agnes::pointer<const int*>  , const int*, 7>;
	using const_homogeneous_pointer16   = fill_agnes_pointer_t<agnes::pointer<const int*>  , const int*, 15>;
	using const_homogeneous_pointer17   = fill_agnes_pointer_t<agnes::pointer<const int*>  , const int*, 16>;

	using const_heterogeneous_pointer2  = fill_agnes_pointer_t<agnes::pointer<const float*>, const int*, 1>;
	using const_heterogeneous_pointer4  = fill_agnes_pointer_t<agnes::pointer<const float*>, const int*, 3>;
	using const_heterogeneous_pointer8  = fill_agnes_pointer_t<agnes::pointer<const float*>, const int*, 7>;
	using const_heterogeneous_pointer16 = fill_agnes_pointer_t<agnes::pointer<const float*>, const int*, 15>;
	using const_heterogeneous_pointer17 = fill_agnes_pointer_t<agnes::pointer<const float*>, const int*, 16>;

	using homogeneous_pointer1          =                      agnes::pointer<      int*>;
	using homogeneous_pointer2          = fill_agnes_pointer_t<agnes::pointer<      int*>  ,       int*, 1>;
	using homogeneous_pointer4          = fill_agnes_pointer_t<agnes::pointer<      int*>  ,       int*, 3>;
	using homogeneous_pointer8          = fill_agnes_pointer_t<agnes::pointer<      int*>  ,       int*, 7>;
	using homogeneous_pointer16         = fill_agnes_pointer_t<agnes::pointer<      int*>  ,       int*, 15>;
	using homogeneous_pointer17         = fill_agnes_pointer_t<agnes::pointer<      int*>  ,       int*, 16>;

	using heterogeneous_pointer2        = fill_agnes_pointer_t<agnes::pointer<      float*>,       int*, 1>;
	using heterogeneous_pointer4        = fill_agnes_pointer_t<agnes::pointer<      float*>,       int*, 3>;
	using heterogeneous_pointer8        = fill_agnes_pointer_t<agnes::pointer<      float*>,       int*, 7>;
	using heterogeneous_pointer16       = fill_agnes_pointer_t<agnes::pointer<      float*>,       int*, 15>;
	using heterogeneous_pointer17       = fill_agnes_pointer_t<agnes::pointer<      float*>,       int*, 16>;

	static constexpr int   c_i[]{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16};
	static constexpr float c_f  {0.0f};

	static constexpr const_homogeneous_pointer1    c_homogeneousNull01;
	static constexpr const_homogeneous_pointer2    c_homogeneousNull02;
	static constexpr const_homogeneous_pointer4    c_homogeneousNull04;
	static constexpr const_homogeneous_pointer8    c_homogeneousNull08;
	static constexpr const_homogeneous_pointer16   c_homogeneousNull16;
	static constexpr const_homogeneous_pointer17   c_homogeneousNull17;
	static constexpr const_homogeneous_pointer1    c_homogeneous01      {c_i};
	static constexpr const_homogeneous_pointer2    c_homogeneous02      {c_i, c_i+1};
	static constexpr const_homogeneous_pointer4    c_homogeneous04      {c_i, c_i+1, c_i+2, c_i+3};
	static constexpr const_homogeneous_pointer8    c_homogeneous08      {c_i, c_i+1, c_i+2, c_i+3, c_i+4, c_i+5, c_i+6, c_i+7};
	static constexpr const_homogeneous_pointer16   c_homogeneous16      {c_i, c_i+1, c_i+2, c_i+3, c_i+4, c_i+5, c_i+6, c_i+7, c_i+8, c_i+9, c_i+10, c_i+11, c_i+12, c_i+13, c_i+14, c_i+15};
	static constexpr const_homogeneous_pointer17   c_homogeneous17      {c_i, c_i+1, c_i+2, c_i+3, c_i+4, c_i+5, c_i+6, c_i+7, c_i+8, c_i+9, c_i+10, c_i+11, c_i+12, c_i+13, c_i+14, c_i+15, c_i+16};
#if !AGNES_POINTER_NO_CONST_EVAL
	static constexpr const_heterogeneous_pointer2  c_heterogeneousNull02;
	static constexpr const_heterogeneous_pointer4  c_heterogeneousNull04;
	static constexpr const_heterogeneous_pointer8  c_heterogeneousNull08;
	static constexpr const_heterogeneous_pointer16 c_heterogeneousNull16;
	static constexpr const_heterogeneous_pointer17 c_heterogeneousNull17;
	static constexpr const_heterogeneous_pointer2  c_heterogeneous02    {&c_f, c_i+1};
	static constexpr const_heterogeneous_pointer4  c_heterogeneous04    {&c_f, c_i+1, c_i+2, c_i+3};
	static constexpr const_heterogeneous_pointer8  c_heterogeneous08    {&c_f, c_i+1, c_i+2, c_i+3, c_i+4, c_i+5, c_i+6, c_i+7};
	static constexpr const_heterogeneous_pointer16 c_heterogeneous16    {&c_f, c_i+1, c_i+2, c_i+3, c_i+4, c_i+5, c_i+6, c_i+7, c_i+8, c_i+9, c_i+10, c_i+11, c_i+12, c_i+13, c_i+14, c_i+15};
	static constexpr const_heterogeneous_pointer17 c_heterogeneous17    {&c_f, c_i+1, c_i+2, c_i+3, c_i+4, c_i+5, c_i+6, c_i+7, c_i+8, c_i+9, c_i+10, c_i+11, c_i+12, c_i+13, c_i+14, c_i+15, c_i+16};
#endif

	const_homogeneous_pointer1    homogeneousNull01Cop   {c_homogeneousNull01  };
	const_homogeneous_pointer2    homogeneousNull02Cop   {c_homogeneousNull02  };
	const_homogeneous_pointer4    homogeneousNull04Cop   {c_homogeneousNull04  };
	const_homogeneous_pointer8    homogeneousNull08Cop   {c_homogeneousNull08  };
	const_homogeneous_pointer16   homogeneousNull16Cop   {c_homogeneousNull16  };
	const_homogeneous_pointer17   homogeneousNull17Cop   {c_homogeneousNull17  };
	const_homogeneous_pointer1    homogeneous01Copy      {c_homogeneous01      };
	const_homogeneous_pointer2    homogeneous02Copy      {c_homogeneous02      };
	const_homogeneous_pointer4    homogeneous04Copy      {c_homogeneous04      };
	const_homogeneous_pointer8    homogeneous08Copy      {c_homogeneous08      };
	const_homogeneous_pointer16   homogeneous16Copy      {c_homogeneous16      };
	const_homogeneous_pointer17   homogeneous17Copy      {c_homogeneous17      };
#if !AGNES_POINTER_NO_CONST_EVAL
	const_heterogeneous_pointer2  heterogeneousNull02Copy{c_heterogeneousNull02};
	const_heterogeneous_pointer4  heterogeneousNull04Copy{c_heterogeneousNull04};
	const_heterogeneous_pointer8  heterogeneousNull08Copy{c_heterogeneousNull08};
	const_heterogeneous_pointer16 heterogeneousNull16Copy{c_heterogeneousNull16};
	const_heterogeneous_pointer17 heterogeneousNull17Copy{c_heterogeneousNull17};
	const_heterogeneous_pointer2  heterogeneous02Copy    {c_heterogeneous02    };
	const_heterogeneous_pointer4  heterogeneous04Copy    {c_heterogeneous04    };
	const_heterogeneous_pointer8  heterogeneous08Copy    {c_heterogeneous08    };
	const_heterogeneous_pointer16 heterogeneous16Copy    {c_heterogeneous16    };
	const_heterogeneous_pointer17 heterogeneous17Copy    {c_heterogeneous17    };
#endif

	int   i[]{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16};
	float f  {0.0f};

	homogeneous_pointer1    homogeneousNull01;
	homogeneous_pointer2    homogeneousNull02;
	homogeneous_pointer4    homogeneousNull04;
	homogeneous_pointer8    homogeneousNull08;
	homogeneous_pointer16   homogeneousNull16;
	homogeneous_pointer17   homogeneousNull17;
	homogeneous_pointer1    homogeneous01{i};
	homogeneous_pointer2    homogeneous02{i, i+1};
	homogeneous_pointer4    homogeneous04{i, i+1, i+2, i+3};
	homogeneous_pointer8    homogeneous08{i, i+1, i+2, i+3, i+4, i+5, i+6, i+7};
	homogeneous_pointer16   homogeneous16{i, i+1, i+2, i+3, i+4, i+5, i+6, i+7, i+8, i+9, i+10, i+11, i+12, i+13, i+14, i+15};
	homogeneous_pointer17   homogeneous17{i, i+1, i+2, i+3, i+4, i+5, i+6, i+7, i+8, i+9, i+10, i+11, i+12, i+13, i+14, i+15, i+16};
	heterogeneous_pointer2  heterogeneousNull02;
	heterogeneous_pointer4  heterogeneousNull04;
	heterogeneous_pointer8  heterogeneousNull08;
	heterogeneous_pointer16 heterogeneousNull16;
	heterogeneous_pointer17 heterogeneousNull17;
	heterogeneous_pointer2  heterogeneous02{&f, i+1};
	heterogeneous_pointer4  heterogeneous04{&f, i+1, i+2, i+3};
	heterogeneous_pointer8  heterogeneous08{&f, i+1, i+2, i+3, i+4, i+5, i+6, i+7};
	heterogeneous_pointer16 heterogeneous16{&f, i+1, i+2, i+3, i+4, i+5, i+6, i+7, i+8, i+9, i+10, i+11, i+12, i+13, i+14, i+15};
	heterogeneous_pointer17 heterogeneous17{&f, i+1, i+2, i+3, i+4, i+5, i+6, i+7, i+8, i+9, i+10, i+11, i+12, i+13, i+14, i+15, i+16};
}

