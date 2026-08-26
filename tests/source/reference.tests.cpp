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

#include <agnes/reference.h>

#include <tuple>
#include <type_traits>
#include <utility>

#include <agnes/utility.h>

#include "agnes_tests/base.h"
#include "agnes_tests/derived.h"
#include "agnes_tests/filled_value.h"
#include "agnes_tests/macros.h"
#include "agnes_tests/maybe_throw.h"
#include "agnes_tests/recorded.h"
#include "agnes_tests/type_properties.h"

using namespace agnes_tests;

// noexcept(true) operations

// Copy construction.
static_assert(is_reference_consistently_implicitly_nothrow_constructible_v<      int& ,       int& , true >); //       int& x{ r};
static_assert(is_reference_consistently_implicitly_nothrow_constructible_v<      int& , const int& , false>); //       int& x{cr};
static_assert(is_reference_consistently_implicitly_nothrow_constructible_v<const int& ,       int& , true >); // const int& x{ r};
static_assert(is_reference_consistently_implicitly_nothrow_constructible_v<const int& , const int& , true >); // const int& x{cr};

static_assert(is_reference_consistently_implicitly_nothrow_constructible_v<      int&&,       int& , false>); //       int&& x{ r};
static_assert(is_reference_consistently_implicitly_nothrow_constructible_v<      int&&, const int& , false>); //       int&& x{cr};
static_assert(is_reference_consistently_implicitly_nothrow_constructible_v<const int&&,       int& , false>); // const int&& x{ r};
static_assert(is_reference_consistently_implicitly_nothrow_constructible_v<const int&&, const int& , false>); // const int&& x{cr};

// Move construction.
static_assert(is_reference_consistently_implicitly_nothrow_constructible_v<      int& ,       int&&, false>); //       int& x{move( r)};
static_assert(is_reference_consistently_implicitly_nothrow_constructible_v<      int& , const int&&, false>); //       int& x{move(cr)};
static_assert(is_reference_consistently_implicitly_nothrow_constructible_v<const int& ,       int&&, true >); // const int& x{move( r)};
static_assert(is_reference_consistently_implicitly_nothrow_constructible_v<const int& , const int&&, true >); // const int& x{move(cr)};

static_assert(is_reference_consistently_implicitly_nothrow_constructible_v<      int&&,       int&&, true >); //       int&& x{move( r)};
static_assert(is_reference_consistently_implicitly_nothrow_constructible_v<      int&&, const int&&, false>); //       int&& x{move(cr)};
static_assert(is_reference_consistently_implicitly_nothrow_constructible_v<const int&&,       int&&, true >); // const int&& x{move( r)};
static_assert(is_reference_consistently_implicitly_nothrow_constructible_v<const int&&, const int&&, true >); // const int&& x{move(cr)};

// Copy assignment.
static_assert(is_reference_consistently_assignable_v                      <      int& ,       int& , true >); // x =  r ;
static_assert(is_reference_consistently_assignable_v                      <      int& , const int& , true >); // x = cr ;
static_assert(is_reference_consistently_assignable_v                      <const int& ,       int& , false>); // x =  r ;
static_assert(is_reference_consistently_assignable_v                      <const int& , const int& , false>); // x = cr ;

static_assert(is_reference_consistently_nothrow_assignable_v              <      int& ,       int& , true >); // x =  r ;
static_assert(is_reference_consistently_nothrow_assignable_v              <      int& , const int& , true >); // x = cr ;
static_assert(is_reference_consistently_nothrow_assignable_v              <const int& ,       int& , false>); // x =  r ;
static_assert(is_reference_consistently_nothrow_assignable_v              <const int& , const int& , false>); // x = cr ;

static_assert(is_reference_consistently_assignable_v                      <      int&&,       int& , true >); // x =  r ;
static_assert(is_reference_consistently_assignable_v                      <      int&&, const int& , true >); // x = cr ;
static_assert(is_reference_consistently_assignable_v                      <const int&&,       int& , false>); // x =  r ;
static_assert(is_reference_consistently_assignable_v                      <const int&&, const int& , false>); // x = cr ;

static_assert(is_reference_consistently_nothrow_assignable_v              <      int&&,       int& , true >); // x =  r ;
static_assert(is_reference_consistently_nothrow_assignable_v              <      int&&, const int& , true >); // x = cr ;
static_assert(is_reference_consistently_nothrow_assignable_v              <const int&&,       int& , false>); // x =  r ;
static_assert(is_reference_consistently_nothrow_assignable_v              <const int&&, const int& , false>); // x = cr ;

// Move assignment.
static_assert(is_reference_consistently_assignable_v                      <      int& ,       int&&, true >); // x = move( r);
static_assert(is_reference_consistently_assignable_v                      <      int& , const int&&, true >); // x = move(cr);
static_assert(is_reference_consistently_assignable_v                      <const int& ,       int&&, false>); // x = move( r);
static_assert(is_reference_consistently_assignable_v                      <const int& , const int&&, false>); // x = move(cr);

static_assert(is_reference_consistently_nothrow_assignable_v              <      int& ,       int&&, true >); // x = move( r);
static_assert(is_reference_consistently_nothrow_assignable_v              <      int& , const int&&, true >); // x = move(cr);
static_assert(is_reference_consistently_nothrow_assignable_v              <const int& ,       int&&, false>); // x = move( r);
static_assert(is_reference_consistently_nothrow_assignable_v              <const int& , const int&&, false>); // x = move(cr);

static_assert(is_reference_consistently_assignable_v                      <      int&&,       int&&, true >); // x = move( r);
static_assert(is_reference_consistently_assignable_v                      <      int&&, const int&&, true >); // x = move(cr);
static_assert(is_reference_consistently_assignable_v                      <const int&&,       int&&, false>); // x = move( r);
static_assert(is_reference_consistently_assignable_v                      <const int&&, const int&&, false>); // x = move(cr);

static_assert(is_reference_consistently_nothrow_assignable_v              <      int&&,       int&&, true >); // x = move( r);
static_assert(is_reference_consistently_nothrow_assignable_v              <      int&&, const int&&, true >); // x = move(cr);
static_assert(is_reference_consistently_nothrow_assignable_v              <const int&&,       int&&, false>); // x = move( r);
static_assert(is_reference_consistently_nothrow_assignable_v              <const int&&, const int&&, false>); // x = move(cr);

// noexcept(false) operations

// Copy assignment.
static_assert(is_reference_consistently_assignable_v        <      maybe_throw_copy_assignable& ,       int& , true >); // x =  r ;
static_assert(is_reference_consistently_assignable_v        <      maybe_throw_copy_assignable& , const int& , true >); // x = cr ;
static_assert(is_reference_consistently_assignable_v        <const maybe_throw_copy_assignable& ,       int& , false>); // x =  r ;
static_assert(is_reference_consistently_assignable_v        <const maybe_throw_copy_assignable& , const int& , false>); // x = cr ;

static_assert(is_reference_consistently_nothrow_assignable_v<      maybe_throw_copy_assignable& ,       int& , false>); // x =  r ;
static_assert(is_reference_consistently_nothrow_assignable_v<      maybe_throw_copy_assignable& , const int& , false>); // x = cr ;
static_assert(is_reference_consistently_nothrow_assignable_v<const maybe_throw_copy_assignable& ,       int& , false>); // x =  r ;
static_assert(is_reference_consistently_nothrow_assignable_v<const maybe_throw_copy_assignable& , const int& , false>); // x = cr ;

static_assert(is_reference_consistently_assignable_v        <      maybe_throw_copy_assignable&&,       int& , true >); // x =  r ;
static_assert(is_reference_consistently_assignable_v        <      maybe_throw_copy_assignable&&, const int& , true >); // x = cr ;
static_assert(is_reference_consistently_assignable_v        <const maybe_throw_copy_assignable&&,       int& , false>); // x =  r ;
static_assert(is_reference_consistently_assignable_v        <const maybe_throw_copy_assignable&&, const int& , false>); // x = cr ;

static_assert(is_reference_consistently_nothrow_assignable_v<      maybe_throw_copy_assignable&&,       int& , false>); // x =  r ;
static_assert(is_reference_consistently_nothrow_assignable_v<      maybe_throw_copy_assignable&&, const int& , false>); // x = cr ;
static_assert(is_reference_consistently_nothrow_assignable_v<const maybe_throw_copy_assignable&&,       int& , false>); // x =  r ;
static_assert(is_reference_consistently_nothrow_assignable_v<const maybe_throw_copy_assignable&&, const int& , false>); // x = cr ;

// Move assignment.
static_assert(is_reference_consistently_assignable_v        <      maybe_throw_move_assignable& ,       int&&, true >); // x = move( r);
static_assert(is_reference_consistently_assignable_v        <      maybe_throw_move_assignable& , const int&&, true >); // x = move(cr);
static_assert(is_reference_consistently_assignable_v        <const maybe_throw_move_assignable& ,       int&&, false>); // x = move( r);
static_assert(is_reference_consistently_assignable_v        <const maybe_throw_move_assignable& , const int&&, false>); // x = move(cr);

static_assert(is_reference_consistently_nothrow_assignable_v<      maybe_throw_move_assignable& ,       int&&, false>); // x = move( r);
static_assert(is_reference_consistently_nothrow_assignable_v<      maybe_throw_move_assignable& , const int&&, false>); // x = move(cr);
static_assert(is_reference_consistently_nothrow_assignable_v<const maybe_throw_move_assignable& ,       int&&, false>); // x = move( r);
static_assert(is_reference_consistently_nothrow_assignable_v<const maybe_throw_move_assignable& , const int&&, false>); // x = move(cr);

static_assert(is_reference_consistently_assignable_v        <      maybe_throw_move_assignable&&,       int&&, true >); // x = move( r);
static_assert(is_reference_consistently_assignable_v        <      maybe_throw_move_assignable&&, const int&&, true >); // x = move(cr);
static_assert(is_reference_consistently_assignable_v        <const maybe_throw_move_assignable&&,       int&&, false>); // x = move( r);
static_assert(is_reference_consistently_assignable_v        <const maybe_throw_move_assignable&&, const int&&, false>); // x = move(cr);

static_assert(is_reference_consistently_nothrow_assignable_v<      maybe_throw_move_assignable&&,       int&&, false>); // x = move( r);
static_assert(is_reference_consistently_nothrow_assignable_v<      maybe_throw_move_assignable&&, const int&&, false>); // x = move(cr);
static_assert(is_reference_consistently_nothrow_assignable_v<const maybe_throw_move_assignable&&,       int&&, false>); // x = move( r);
static_assert(is_reference_consistently_nothrow_assignable_v<const maybe_throw_move_assignable&&, const int&&, false>); // x = move(cr);

TEMPLATE_TEST_CASE("agnes::reference<T&...> direct construction from lvalue", "",
	(std::tuple<      int ,       int    >),
	(std::tuple<const int , const int    >),
	(std::tuple<const int ,       int    >),
	(std::tuple<      base,       derived>),
	(std::tuple<const base, const derived>),
	(std::tuple<const base,       derived>))
{
	using ReferenceValueType = std::tuple_element_t<0, TestType>;
	using OtherValueType     = std::tuple_element_t<1, TestType>;

	if constexpr (std::is_const_v<ReferenceValueType> && std::is_const_v<OtherValueType>)
	{
		static constexpr OtherValueType value{0};

		static constexpr ReferenceValueType&                   cppRef  {value};
		static constexpr agnes::reference<ReferenceValueType&> agnesRef{value};

		static_assert(&cppRef           == &value);
		static_assert(&get<0>(agnesRef) == &value);
	}

	static constexpr auto test = []
	{
		OtherValueType value{0};

		ReferenceValueType&                   cppRef  {value};
		agnes::reference<ReferenceValueType&> agnesRef{value};

		AGNES_REQUIRE(&cppRef           == &value);
		AGNES_REQUIRE(&get<0>(agnesRef) == &value);

		return true;
	};

	AGNES_RUN(test);
}

TEMPLATE_TEST_CASE("agnes::reference<T&...> direct construction from lvalue reference", "",
	(std::tuple<      int ,       int    >),
	(std::tuple<const int , const int    >),
	(std::tuple<const int ,       int    >),
	(std::tuple<      base,       derived>),
	(std::tuple<const base, const derived>),
	(std::tuple<const base,       derived>))
{
	using ReferenceValueType = std::tuple_element_t<0, TestType>;
	using OtherValueType     = std::tuple_element_t<1, TestType>;

	if constexpr (std::is_const_v<ReferenceValueType> && std::is_const_v<OtherValueType>)
	{
		static constexpr OtherValueType  value   {0};
		static constexpr OtherValueType& valueRef{value};

		static constexpr ReferenceValueType&                   cppRef  {valueRef};
		static constexpr agnes::reference<ReferenceValueType&> agnesRef{valueRef};

		static_assert(&cppRef           == &value);
		static_assert(&get<0>(agnesRef) == &value);
	}

	static constexpr auto test = []
	{
		OtherValueType  value   {0};
		OtherValueType& valueRef{value};

		ReferenceValueType&                   cppRef  {valueRef};
		agnes::reference<ReferenceValueType&> agnesRef{valueRef};

		AGNES_REQUIRE(&cppRef           == &value);
		AGNES_REQUIRE(&get<0>(agnesRef) == &value);

		return true;
	};

	AGNES_RUN(test);
}

TEMPLATE_TEST_CASE("agnes::reference<T&...> direct construction from rvalue reference", "",
	(std::tuple<const int ,       int    >),
	(std::tuple<const int , const int    >),
	(std::tuple<const base,       derived>),
	(std::tuple<const base, const derived>))
{
	using ReferenceValueType = std::tuple_element_t<0, TestType>;
	using OtherValueType     = std::tuple_element_t<1, TestType>;

	if constexpr (std::is_const_v<ReferenceValueType> && std::is_const_v<OtherValueType>)
	{
		static constexpr OtherValueType   value   {0};
		static constexpr OtherValueType&& valueRef{std::move(value)};

		static constexpr ReferenceValueType&                   cppRef  {std::move(valueRef)};
		static constexpr agnes::reference<ReferenceValueType&> agnesRef{std::move(valueRef)};

		static_assert(&cppRef           == &value);
		static_assert(&get<0>(agnesRef) == &value);
	}

	static constexpr auto test = []
	{
		OtherValueType   value   {0};
		OtherValueType&& valueRef{std::move(value)};

		ReferenceValueType&                   cppRef  {std::move(valueRef)};
		agnes::reference<ReferenceValueType&> agnesRef{std::move(valueRef)};

		AGNES_REQUIRE(&cppRef           == &value);
		AGNES_REQUIRE(&get<0>(agnesRef) == &value);

		return true;
	};

	AGNES_RUN(test);
}

TEMPLATE_TEST_CASE("agnes::reference<T&...> copy construction", "",
	(std::tuple<      int ,       int    >),
	(std::tuple<const int ,       int    >),
	(std::tuple<const int , const int    >),
	(std::tuple<      base,       derived>),
	(std::tuple<const base,       derived>),
	(std::tuple<const base, const derived>))
{
	using ReferenceValueType = std::tuple_element_t<0, TestType>;
	using OtherValueType     = std::tuple_element_t<1, TestType>;

	if constexpr (std::is_const_v<ReferenceValueType> && std::is_const_v<OtherValueType>)
	{
		static constexpr OtherValueType value{0};

		{
			static constexpr OtherValueType& cppOther{value};

			static constexpr ReferenceValueType& cppRef{cppOther};

			static_assert(&cppRef == &value);
		}

		{
			static constexpr OtherValueType&& cppOther{std::move(value)};

			static constexpr ReferenceValueType& cppRef{cppOther};

			static_assert(&cppRef == &value);
		}

		{
			static constexpr agnes::reference<OtherValueType&> agnesOther{value};

			{
				static constexpr agnes::reference<ReferenceValueType&> agnesRef{agnesOther};

				static_assert(&get<0>(agnesRef) == &value);
			}

			{
				static constexpr agnes::reference<ReferenceValueType&> agnesRef{std::move(agnesOther)};

				static_assert(&get<0>(agnesRef) == &value);
			}
		}

		{
			static constexpr agnes::reference<OtherValueType&&> agnesOther{std::move(value)};

			static constexpr agnes::reference<ReferenceValueType&> agnesRef{agnesOther};

			static_assert(&get<0>(agnesRef) == &value);
		}
	}

	static constexpr auto test = []
	{
		OtherValueType value{0};

		{
			OtherValueType& cppOther{value};

			ReferenceValueType& cppRef{cppOther};

			AGNES_REQUIRE(&cppRef == &value);
		}

		{
			OtherValueType&& cppOther{std::move(value)};

			ReferenceValueType& cppRef{cppOther};

			AGNES_REQUIRE(&cppRef == &value);
		}

		{
			agnes::reference<OtherValueType&> agnesOther{value};

			{
				agnes::reference<ReferenceValueType&> agnesRef{agnesOther};

				AGNES_REQUIRE(&get<0>(agnesRef) == &value);
			}

			{
				agnes::reference<ReferenceValueType&> agnesRef{[&value]() -> agnes::reference<OtherValueType&>
				{
					return agnes::reference<OtherValueType&>{value};
				}()};

				AGNES_REQUIRE(&get<0>(agnesRef) == &value);
			}
		}

		{
			const agnes::reference<OtherValueType&> agnesOther{value};

			{
				agnes::reference<ReferenceValueType&> agnesRef{agnesOther};

				AGNES_REQUIRE(&get<0>(agnesRef) == &value);
			}

			{
				agnes::reference<ReferenceValueType&> agnesRef{[&value]() -> const agnes::reference<OtherValueType&>
				{
					return agnes::reference<OtherValueType&>{value};
				}()};

				AGNES_REQUIRE(&get<0>(agnesRef) == &value);
			}
		}

		{
			agnes::reference<OtherValueType&&> agnesOther{std::move(value)};

			agnes::reference<ReferenceValueType&> agnesRef{agnesOther};

			AGNES_REQUIRE(&get<0>(agnesRef) == &value);
		}

		{
			const agnes::reference<OtherValueType&&> agnesOther{std::move(value)};

			agnes::reference<ReferenceValueType&> agnesRef{agnesOther};

			AGNES_REQUIRE(&get<0>(agnesRef) == &value);
		}

		return true;
	};

	AGNES_RUN(test);
}

TEMPLATE_TEST_CASE("agnes::reference<T&...> move construction", "",
	(std::tuple<const int ,       int    >),
	(std::tuple<const int , const int    >),
	(std::tuple<const base,       derived>),
	(std::tuple<const base, const derived>))
{
	using ReferenceValueType = std::tuple_element_t<0, TestType>;
	using OtherValueType     = std::tuple_element_t<1, TestType>;

	if constexpr (std::is_const_v<ReferenceValueType> && std::is_const_v<OtherValueType>)
	{
		static constexpr OtherValueType value{0};

		{
			static constexpr OtherValueType&& cppOther{std::move(value)};

			static constexpr ReferenceValueType& cppRef{std::move(cppOther)};

			static_assert(&cppRef == &value);
		}

		{
			static constexpr agnes::reference<OtherValueType&&> agnesOther{std::move(value)};

			static constexpr agnes::reference<ReferenceValueType&> agnesRef{std::move(agnesOther)};

			static_assert(&get<0>(agnesRef) == &value);
		}
	}

	static constexpr auto test = []
	{
		OtherValueType value{0};

		{
			OtherValueType&& cppOther{std::move(value)};

			ReferenceValueType& cppRef{std::move(cppOther)};

			AGNES_REQUIRE(&cppRef == &value);
		}

		{
			agnes::reference<OtherValueType&&> agnesOther{std::move(value)};

			agnes::reference<ReferenceValueType&> agnesRef{std::move(agnesOther)};

			AGNES_REQUIRE(&get<0>(agnesRef) == &value);
		}

		{
			const agnes::reference<OtherValueType&&> agnesOther{std::move(value)};

			agnes::reference<ReferenceValueType&> agnesRef{std::move(agnesOther)};

			AGNES_REQUIRE(&get<0>(agnesRef) == &value);
		}

		return true;
	};

	AGNES_RUN(test);
}

TEMPLATE_TEST_CASE("agnes::reference<T&...> copy assignment", "",
	(std::tuple<int ,       int    >),
	(std::tuple<int , const int    >),
	(std::tuple<base,       derived>),
	(std::tuple<base, const derived>))
{
	using ReferenceValueType = std::tuple_element_t<0, TestType>;
	using OtherValueType     = std::tuple_element_t<1, TestType>;

	static constexpr auto test = []
	{
		OtherValueType otherValue{1};

		{
			ReferenceValueType value   {0};
			OtherValueType&    cppOther{otherValue};

			ReferenceValueType& cppRef{value};

			cppRef = cppOther;

			AGNES_REQUIRE(value == otherValue);
		}

		{
			ReferenceValueType value   {0};
			OtherValueType&&   cppOther{std::move(otherValue)};

			ReferenceValueType& cppRef{value};

			cppRef = cppOther;

			AGNES_REQUIRE(value == otherValue);
		}

		{
			ReferenceValueType                value     {0};
			agnes::reference<OtherValueType&> agnesOther{otherValue};

			agnes::reference<ReferenceValueType&> agnesRef{value};

			agnesRef = agnesOther;

			AGNES_REQUIRE(value == otherValue);

			agnesRef = std::move(agnesOther);

			AGNES_REQUIRE(value == otherValue);
		}

		{
			ReferenceValueType                 value     {0};
			agnes::reference<OtherValueType&&> agnesOther{std::move(otherValue)};

			agnes::reference<ReferenceValueType&> agnesRef{value};

			agnesRef = agnesOther;

			AGNES_REQUIRE(value == otherValue);
		}

		{
			ReferenceValueType         value     {0};
			std::tuple<OtherValueType> tupleOther{otherValue};

			agnes::reference<ReferenceValueType&> agnesRef{value};

			agnesRef = tupleOther;

			AGNES_REQUIRE(value == otherValue);

			agnesRef = std::move(tupleOther);

			AGNES_REQUIRE(value == otherValue);
		}

		{
			ReferenceValueType          value     {0};
			std::tuple<OtherValueType&> tupleOther{otherValue};

			agnes::reference<ReferenceValueType&> agnesRef{value};

			agnesRef = tupleOther;

			AGNES_REQUIRE(value == otherValue);

			agnesRef = std::move(tupleOther);

			AGNES_REQUIRE(value == otherValue);
		}

		return true;
	};

	AGNES_RUN(test);
}

TEMPLATE_TEST_CASE("agnes::reference<T&...> move assignment", "",
	(std::tuple<recorded<int>,                int >),
	(std::tuple<recorded<int>,          const int >),
	(std::tuple<recorded<int>, recorded<      int>>))
{
	using ReferenceValueType = std::tuple_element_t<0, TestType>;
	using OtherValueType     = std::tuple_element_t<1, TestType>;

	static constexpr auto test = []
	{
		OtherValueType otherValue{1};

		{
			ReferenceValueType value   {0};
			OtherValueType&    cppOther{otherValue};

			ReferenceValueType& cppRef{value};

			cppRef = std::move(cppOther);

			AGNES_REQUIRE(value == otherValue);
			AGNES_REQUIRE(value.actions.last() == class_action::moveAssigned);
		}

		{
			ReferenceValueType value   {0};
			OtherValueType&&   cppOther{std::move(otherValue)};

			ReferenceValueType& cppRef{value};

			cppRef = std::move(cppOther);

			AGNES_REQUIRE(value == otherValue);
			AGNES_REQUIRE(value.actions.last() == class_action::moveAssigned);
		}

		{
			ReferenceValueType                value     {0};
			agnes::reference<OtherValueType&> agnesOther{otherValue};

			agnes::reference<ReferenceValueType&> agnesRef{value};

			agnesRef = agnes::as_rvalue(agnesOther);

			AGNES_REQUIRE(value == otherValue);
			AGNES_REQUIRE(value.actions.last() == class_action::moveAssigned);
		}

		{
			ReferenceValueType                      value     {0};
			const agnes::reference<OtherValueType&> agnesOther{otherValue};

			agnes::reference<ReferenceValueType&> agnesRef{value};

			agnesRef = agnes::as_rvalue(agnesOther);

			AGNES_REQUIRE(value == otherValue);
			AGNES_REQUIRE(value.actions.last() == class_action::moveAssigned);
		}

		{
			ReferenceValueType                 value     {0};
			agnes::reference<OtherValueType&&> agnesOther{std::move(otherValue)};

			agnes::reference<ReferenceValueType&> agnesRef{value};

			agnesRef = agnes::as_rvalue(agnesOther);

			AGNES_REQUIRE(value == otherValue);
			AGNES_REQUIRE(value.actions.last() == class_action::moveAssigned);
		}

		{
			ReferenceValueType                       value     {0};
			const agnes::reference<OtherValueType&&> agnesOther{std::move(otherValue)};

			agnes::reference<ReferenceValueType&> agnesRef{value};

			agnesRef = agnes::as_rvalue(agnesOther);

			AGNES_REQUIRE(value == otherValue);
			AGNES_REQUIRE(value.actions.last() == class_action::moveAssigned);
		}

		{
			ReferenceValueType           value     {0};
			std::tuple<OtherValueType&&> tupleOther{std::move(otherValue)};

			agnes::reference<ReferenceValueType&> agnesRef{value};

			agnesRef = std::move(tupleOther);

			AGNES_REQUIRE(value == otherValue);
			AGNES_REQUIRE(value.actions.last() == class_action::moveAssigned);
		}

		{
			ReferenceValueType                 value     {0};
			const std::tuple<OtherValueType&&> tupleOther{std::move(otherValue)};

			agnes::reference<ReferenceValueType&> agnesRef{value};

			agnesRef = std::move(tupleOther);

			AGNES_REQUIRE(value == otherValue);
			AGNES_REQUIRE(value.actions.last() == class_action::moveAssigned);
		}

		return true;
	};

	AGNES_RUN(test);
}

TEST_CASE("agnes::reference<T&...> address-of")
{
	static constexpr auto test = []
	{
		int values[]{0, 1};

		agnes::reference<int&, int&> agnesRef{values[0], values[1]};

		agnes::pointer<int*, int*> agnesPointer{&agnesRef};

		AGNES_REQUIRE(get<0>(agnesPointer) == &values[0]);
		AGNES_REQUIRE(get<1>(agnesPointer) == &values[1]);

		return true;
	};

	AGNES_RUN(test);
}

TEMPLATE_TEST_CASE("agnes::reference<T&...> swap", "",
	(std::tuple<int , int>))
{
	using LhsValueType = std::tuple_element_t<0, TestType>;
	using RhsValueType = std::tuple_element_t<1, TestType>;

	static constexpr auto test = []
	{
		{
			LhsValueType lhsValue{0};
			RhsValueType rhsValue{1};

			LhsValueType& lhs{lhsValue};
			RhsValueType& rhs{rhsValue};

			using std::swap;

			swap(lhs, rhs);

			AGNES_REQUIRE(lhsValue == 1);
			AGNES_REQUIRE(rhsValue == 0);

			swap(rhs, lhs);

			AGNES_REQUIRE(lhsValue == 0);
			AGNES_REQUIRE(rhsValue == 1);
		}

		{
			LhsValueType lhsValue{0};
			RhsValueType rhsValue{1};

			agnes::reference<LhsValueType&> lhs{lhsValue};
			agnes::reference<RhsValueType&> rhs{rhsValue};

			lhs.swap(rhs);

			AGNES_REQUIRE(lhsValue == 1);
			AGNES_REQUIRE(rhsValue == 0);

			using std::swap;

			swap(lhs, rhs);

			AGNES_REQUIRE(lhsValue == 0);
			AGNES_REQUIRE(rhsValue == 1);
		}

		{
			LhsValueType lhsValue{0};
			RhsValueType rhsValue{1};

			agnes::reference<LhsValueType&>  lhs{lhsValue};
			agnes::reference<RhsValueType&&> rhs{std::move(rhsValue)};

			lhs.swap(rhs);

			AGNES_REQUIRE(lhsValue == 1);
			AGNES_REQUIRE(rhsValue == 0);

			using std::swap;

			swap(lhs, rhs);

			AGNES_REQUIRE(lhsValue == 0);
			AGNES_REQUIRE(rhsValue == 1);
		}

		return true;
	};

	AGNES_RUN(test);
}

TEMPLATE_TEST_CASE("agnes::reference<T&...> comparison", "",
	(std::tuple<      int ,       int    >),
	(std::tuple<      int , const int    >),
	(std::tuple<const int ,       int    >),
	(std::tuple<const int , const int    >),
	(std::tuple<      base,       derived>),
	(std::tuple<      base, const derived>),
	(std::tuple<const base,       derived>),
	(std::tuple<const base, const derived>))
{
	using LhsValueType = std::tuple_element_t<0, TestType>;
	using RhsValueType = std::tuple_element_t<1, TestType>;

	if constexpr (std::is_const_v<LhsValueType> && std::is_const_v<RhsValueType>)
	{
		static constexpr LhsValueType lhsLvrValue[]{0, 1};
		static constexpr RhsValueType rhsLvrValue[]{0, 1};
		static constexpr RhsValueType rhsRvrValue[]{0, 1};

		static constexpr agnes::reference<LhsValueType&>  lhsLvr0{lhsLvrValue[0]};
		static constexpr agnes::reference<LhsValueType&>  lhsLvr1{lhsLvrValue[1]};
		static constexpr agnes::reference<RhsValueType&>  rhsLvr0{rhsLvrValue[0]};
		static constexpr agnes::reference<RhsValueType&>  rhsLvr1{rhsLvrValue[1]};

		static constexpr agnes::reference<RhsValueType&&> rhsRvr0{std::move(rhsRvrValue[0])};
		static constexpr agnes::reference<RhsValueType&&> rhsRvr1{std::move(rhsRvrValue[1])};

		static_assert(lhsLvr0 <= lhsLvr0);
		static_assert(lhsLvr0 == lhsLvr0);
		static_assert(lhsLvr0 >= lhsLvr0);

		static_assert(lhsLvr1 <= lhsLvr1);
		static_assert(lhsLvr1 == lhsLvr1);
		static_assert(lhsLvr1 >= lhsLvr1);

		static_assert(rhsLvr0 <= rhsLvr0);
		static_assert(rhsLvr0 == rhsLvr0);
		static_assert(rhsLvr0 >= rhsLvr0);

		static_assert(rhsLvr1 <= rhsLvr1);
		static_assert(rhsLvr1 == rhsLvr1);
		static_assert(rhsLvr1 >= rhsLvr1);

		static_assert(lhsLvr0 <  rhsLvr1);
		static_assert(lhsLvr0 <= rhsLvr1);
		static_assert(lhsLvr0 <= rhsLvr0);
		static_assert(lhsLvr0 == rhsLvr0);
		static_assert(lhsLvr0 >= rhsLvr0);

		static_assert(lhsLvr1 <= rhsLvr1);
		static_assert(lhsLvr1 == rhsLvr1);
		static_assert(lhsLvr1 >= rhsLvr1);
		static_assert(lhsLvr1 >= rhsLvr0);
		static_assert(lhsLvr1 >  rhsLvr0);

		static_assert(lhsLvr0 <  rhsRvr1);
		static_assert(lhsLvr0 <= rhsRvr1);
		static_assert(lhsLvr0 <= rhsRvr0);
		static_assert(lhsLvr0 == rhsRvr0);
		static_assert(lhsLvr0 >= rhsRvr0);

		static_assert(lhsLvr1 <= rhsRvr1);
		static_assert(lhsLvr1 == rhsRvr1);
		static_assert(lhsLvr1 >= rhsRvr1);
		static_assert(lhsLvr1 >= rhsRvr0);
		static_assert(lhsLvr1 >  rhsRvr0);
	}

	static constexpr auto test = []
	{
		LhsValueType lhsLvrValue[]{0, 1};
		RhsValueType rhsLvrValue[]{0, 1};
		RhsValueType rhsRvrValue[]{0, 1};

		agnes::reference<LhsValueType&> lhsLvr0{lhsLvrValue[0]};
		agnes::reference<LhsValueType&> lhsLvr1{lhsLvrValue[1]};
		agnes::reference<RhsValueType&> rhsLvr0{rhsLvrValue[0]};
		agnes::reference<RhsValueType&> rhsLvr1{rhsLvrValue[1]};

		agnes::reference<RhsValueType&&> rhsRvr0{std::move(rhsRvrValue[0])};
		agnes::reference<RhsValueType&&> rhsRvr1{std::move(rhsRvrValue[1])};

		AGNES_REQUIRE(lhsLvr0 <= lhsLvr0);
		AGNES_REQUIRE(lhsLvr0 == lhsLvr0);
		AGNES_REQUIRE(lhsLvr0 >= lhsLvr0);

		AGNES_REQUIRE(lhsLvr1 <= lhsLvr1);
		AGNES_REQUIRE(lhsLvr1 == lhsLvr1);
		AGNES_REQUIRE(lhsLvr1 >= lhsLvr1);

		AGNES_REQUIRE(rhsLvr0 <= rhsLvr0);
		AGNES_REQUIRE(rhsLvr0 == rhsLvr0);
		AGNES_REQUIRE(rhsLvr0 >= rhsLvr0);

		AGNES_REQUIRE(rhsLvr1 <= rhsLvr1);
		AGNES_REQUIRE(rhsLvr1 == rhsLvr1);
		AGNES_REQUIRE(rhsLvr1 >= rhsLvr1);

		AGNES_REQUIRE(lhsLvr0 <  rhsLvr1);
		AGNES_REQUIRE(lhsLvr0 <= rhsLvr1);
		AGNES_REQUIRE(lhsLvr0 <= rhsLvr0);
		AGNES_REQUIRE(lhsLvr0 == rhsLvr0);
		AGNES_REQUIRE(lhsLvr0 >= rhsLvr0);

		AGNES_REQUIRE(lhsLvr1 <= rhsLvr1);
		AGNES_REQUIRE(lhsLvr1 == rhsLvr1);
		AGNES_REQUIRE(lhsLvr1 >= rhsLvr1);
		AGNES_REQUIRE(lhsLvr1 >= rhsLvr0);
		AGNES_REQUIRE(lhsLvr1 >  rhsLvr0);

		AGNES_REQUIRE(lhsLvr0 <  rhsRvr1);
		AGNES_REQUIRE(lhsLvr0 <= rhsRvr1);
		AGNES_REQUIRE(lhsLvr0 <= rhsRvr0);
		AGNES_REQUIRE(lhsLvr0 == rhsRvr0);
		AGNES_REQUIRE(lhsLvr0 >= rhsRvr0);

		AGNES_REQUIRE(lhsLvr1 <= rhsRvr1);
		AGNES_REQUIRE(lhsLvr1 == rhsRvr1);
		AGNES_REQUIRE(lhsLvr1 >= rhsRvr1);
		AGNES_REQUIRE(lhsLvr1 >= rhsRvr0);
		AGNES_REQUIRE(lhsLvr1 >  rhsRvr0);

		return true;
	};

	AGNES_RUN(test);
}

TEMPLATE_TEST_CASE("agnes::reference<T&...> deduction guides", "", int, const int)
{
	if constexpr (std::is_const_v<TestType>)
	{
		static constexpr TestType                     value   {0};
		static constexpr agnes::reference<TestType&>  agnesLvr{value};
		static constexpr agnes::reference<TestType&&> agnesRvr{std::move(value)};

		static constexpr agnes::reference agnesRef0{value};
		static constexpr agnes::reference agnesRef1{agnesLvr};
		static constexpr agnes::reference agnesRef2{agnes::reference<TestType&>{value}};
		static constexpr agnes::reference agnesRef3{agnesRvr};

		static_assert(std::is_same_v<decltype(agnesRef0), const agnes::reference<TestType&>>);
		static_assert(std::is_same_v<decltype(agnesRef1), const agnes::reference<TestType&>>);
		static_assert(std::is_same_v<decltype(agnesRef2), const agnes::reference<TestType&>>);
		static_assert(std::is_same_v<decltype(agnesRef3), const agnes::reference<TestType&>>);
	}

	static constexpr auto test = []
	{
		TestType                           value    {0};
		agnes::reference<TestType&>        agnesLvr {value};
		const agnes::reference<TestType&>  agnesClvr{value};
		agnes::reference<TestType&&>       agnesRvr {std::move(value)};
		const agnes::reference<TestType&&> agnesCrvr{std::move(value)};

		agnes::reference agnesRef0{value};
		agnes::reference agnesRef1{agnesLvr};
		agnes::reference agnesRef2{agnesClvr};
		agnes::reference agnesRef3{agnes::reference<TestType&>{value}};
		agnes::reference agnesRef4{agnesRvr};
		agnes::reference agnesRef5{agnesCrvr};

		static_assert(std::is_same_v<decltype(agnesRef0), agnes::reference<TestType&>>);
		static_assert(std::is_same_v<decltype(agnesRef1), agnes::reference<TestType&>>);
		static_assert(std::is_same_v<decltype(agnesRef2), agnes::reference<TestType&>>);
		static_assert(std::is_same_v<decltype(agnesRef3), agnes::reference<TestType&>>);
		static_assert(std::is_same_v<decltype(agnesRef4), agnes::reference<TestType&>>);
		static_assert(std::is_same_v<decltype(agnesRef5), agnes::reference<TestType&>>);

		return true;
	};

	AGNES_RUN(test);
}

TEST_CASE("agnes::reference<T&...> apply")
{
	static constexpr auto test = []
	{
		int  values[]{0, 1};
		bool applied {false};
		auto f       {[&applied](int&, int&) { applied = true; }};

		agnes::reference<int&, int&> agnesRef{values[0], values[1]};

		agnes::apply(f, agnesRef);

		AGNES_REQUIRE(applied);

		return true;
	};

	AGNES_RUN(test);
}

TEMPLATE_TEST_CASE_SIG("agnes::reference<T&...> subset", "",
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

		const agnes::reference<T&...> r{get<Ti>(values)...};

		const agnes::reference_subset_t<agnes::reference<T&...>, Subset...> sub{subset<Subset...>(r)};

		AGNES_REQUIRE(((&get<Subset>(r) == &get<I>(sub)) && ...));

		return true;

	}(std::make_index_sequence<std::tuple_size_v<Types>>{},
		static_cast<Types*>(nullptr),
		std::make_index_sequence<sizeof...(Subset)>{}); };

	AGNES_RUN(test);
}

TEMPLATE_TEST_CASE("agnes::reference<T&&...> direct construction from rvalue reference", "",
	(std::tuple<      int ,       int    >),
	(std::tuple<const int ,       int    >),
	(std::tuple<const int , const int    >),
	(std::tuple<      base,       derived>),
	(std::tuple<const base,       derived>),
	(std::tuple<const base, const derived>))
{
	using ReferenceValueType = std::tuple_element_t<0, TestType>;
	using OtherValueType     = std::tuple_element_t<1, TestType>;

	if constexpr (std::is_const_v<ReferenceValueType> && std::is_const_v<OtherValueType>)
	{
		static constexpr OtherValueType   value   {0};
		static constexpr OtherValueType&& valueRef{std::move(value)};

		static constexpr ReferenceValueType&&                   cppRef  {std::move(valueRef)};
		static constexpr agnes::reference<ReferenceValueType&&> agnesRef{std::move(valueRef)};

		static_assert(&cppRef           == &value);
		static_assert(&get<0>(agnesRef) == &value);
	}

	static constexpr auto test = []
	{
		OtherValueType   value   {0};
		OtherValueType&& valueRef{std::move(value)};

		ReferenceValueType&&                   cppRef  {std::move(valueRef)};
		agnes::reference<ReferenceValueType&&> agnesRef{std::move(valueRef)};

		AGNES_REQUIRE(&cppRef           == &value);
		AGNES_REQUIRE(&get<0>(agnesRef) == &value);

		return true;
	};

	AGNES_RUN(test);
}

TEMPLATE_TEST_CASE("agnes::reference<T&&...> move construction", "",
	(std::tuple<      int ,       int    >),
	(std::tuple<const int ,       int    >),
	(std::tuple<const int , const int    >),
	(std::tuple<      base,       derived>),
	(std::tuple<const base,       derived>),
	(std::tuple<const base, const derived>))
{
	using ReferenceValueType = std::tuple_element_t<0, TestType>;
	using OtherValueType     = std::tuple_element_t<1, TestType>;

	if constexpr (std::is_const_v<ReferenceValueType> && std::is_const_v<OtherValueType>)
	{
		static constexpr OtherValueType                     value     {0};
		static constexpr OtherValueType&&                   cppOther  {std::move(value)};
		static constexpr agnes::reference<OtherValueType&&> agnesOther{std::move(value)};

		static constexpr ReferenceValueType&&                   cppRef  {std::move(cppOther)};
		static constexpr agnes::reference<ReferenceValueType&&> agnesRef{std::move(agnesOther)};

		static_assert(&cppRef           == &value);
		static_assert(&get<0>(agnesRef) == &value);
	}

	static constexpr auto test = []
	{
		OtherValueType                     value     {0};
		OtherValueType&&                   cppOther  {std::move(value)};
		agnes::reference<OtherValueType&&> agnesOther{std::move(value)};

		ReferenceValueType&&                   cppRef  {std::move(cppOther)};
		agnes::reference<ReferenceValueType&&> agnesRef{std::move(agnesOther)};

		AGNES_REQUIRE(&cppRef           == &value);
		AGNES_REQUIRE(&get<0>(agnesRef) == &value);

		return true;
	};

	AGNES_RUN(test);
}

TEMPLATE_TEST_CASE("agnes::reference<T&&...> move assignment", "",
	(std::tuple<int ,       int    >),
	(std::tuple<int , const int    >),
	(std::tuple<base,       derived>),
	(std::tuple<base, const derived>))
{
	using ReferenceValueType = std::tuple_element_t<0, TestType>;
	using OtherValueType     = std::tuple_element_t<1, TestType>;

	static constexpr auto test = []
	{
		OtherValueType otherValue{1};

		{
			ReferenceValueType value   {0};
			OtherValueType&&   cppOther{std::move(otherValue)};

			ReferenceValueType&& cppRef{std::move(value)};

			cppRef = std::move(cppOther);

			AGNES_REQUIRE(value == otherValue);
		}

		{
			ReferenceValueType value   {0};
			OtherValueType&    cppOther{otherValue};

			ReferenceValueType&& cppRef{std::move(value)};

			cppRef = std::move(cppOther);

			AGNES_REQUIRE(value == otherValue);
		}

		{
			ReferenceValueType                 value     {0};
			agnes::reference<OtherValueType&&> agnesOther{std::move(otherValue)};

			agnes::reference<ReferenceValueType&&> agnesRef{std::move(value)};

			agnesRef = std::move(agnesOther);

			AGNES_REQUIRE(value == otherValue);
		}

		{
			ReferenceValueType                       value     {0};
			const agnes::reference<OtherValueType&&> agnesOther{std::move(otherValue)};

			agnes::reference<ReferenceValueType&&> agnesRef{std::move(value)};

			agnesRef = std::move(agnesOther);

			AGNES_REQUIRE(value == otherValue);
		}

		{
			ReferenceValueType                value     {0};
			agnes::reference<OtherValueType&> agnesOther{otherValue};

			agnes::reference<ReferenceValueType&&> agnesRef{std::move(value)};

			agnesRef = std::move(agnesOther);

			AGNES_REQUIRE(value == otherValue);
		}

		{
			ReferenceValueType                      value     {0};
			const agnes::reference<OtherValueType&> agnesOther{otherValue};

			agnes::reference<ReferenceValueType&&> agnesRef{std::move(value)};

			agnesRef = std::move(agnesOther);

			AGNES_REQUIRE(value == otherValue);
		}

		return true;
	};

	AGNES_RUN(test);
}

TEST_CASE("agnes::reference<T&&...> address-of")
{
	static constexpr auto test = []
	{
		int values[]{0, 1};

		agnes::reference<int&&, int&&> agnesRef{std::move(values[0]), std::move(values[1])};

		agnes::pointer<int*, int*> agnesPointer{&agnesRef};

		AGNES_REQUIRE(get<0>(agnesPointer) == &values[0]);
		AGNES_REQUIRE(get<1>(agnesPointer) == &values[1]);

		return true;
	};

	AGNES_RUN(test);
}

TEMPLATE_TEST_CASE("agnes::reference<T&&...> swap", "",
	(std::tuple<int , int>))
{
	using LhsValueType = std::tuple_element_t<0, TestType>;
	using RhsValueType = std::tuple_element_t<1, TestType>;

	static constexpr auto test = []
	{
		{
			LhsValueType lhsValue{0};
			RhsValueType rhsValue{1};

			LhsValueType&& lhs{std::move(lhsValue)};
			RhsValueType&  rhs{rhsValue};

			using std::swap;

			swap(lhs, rhs);

			AGNES_REQUIRE(lhsValue == 1);
			AGNES_REQUIRE(rhsValue == 0);

			swap(rhs, lhs);

			AGNES_REQUIRE(lhsValue == 0);
			AGNES_REQUIRE(rhsValue == 1);
		}

		{
			LhsValueType lhsValue{0};
			RhsValueType rhsValue{1};

			agnes::reference<LhsValueType&&> lhs{std::move(lhsValue)};
			agnes::reference<RhsValueType&&> rhs{std::move(rhsValue)};

			lhs.swap(rhs);

			AGNES_REQUIRE(lhsValue == 1);
			AGNES_REQUIRE(rhsValue == 0);

			using std::swap;

			swap(lhs, rhs);

			AGNES_REQUIRE(lhsValue == 0);
			AGNES_REQUIRE(rhsValue == 1);
		}

		{
			LhsValueType lhsValue{0};
			RhsValueType rhsValue{1};

			agnes::reference<LhsValueType&&> lhs{std::move(lhsValue)};
			agnes::reference<RhsValueType&>  rhs{rhsValue};

			lhs.swap(rhs);

			AGNES_REQUIRE(lhsValue == 1);
			AGNES_REQUIRE(rhsValue == 0);

			using std::swap;

			swap(lhs, rhs);

			AGNES_REQUIRE(lhsValue == 0);
			AGNES_REQUIRE(rhsValue == 1);
		}

		return true;
	};

	AGNES_RUN(test);
}

TEMPLATE_TEST_CASE("agnes::reference<T&&...> comparison", "",
	(std::tuple<      int ,       int    >),
	(std::tuple<      int , const int    >),
	(std::tuple<const int ,       int    >),
	(std::tuple<const int , const int    >),
	(std::tuple<      base,       derived>),
	(std::tuple<      base, const derived>),
	(std::tuple<const base,       derived>),
	(std::tuple<const base, const derived>))
{
	using LhsValueType = std::tuple_element_t<0, TestType>;
	using RhsValueType = std::tuple_element_t<1, TestType>;

	if constexpr (std::is_const_v<LhsValueType> && std::is_const_v<RhsValueType>)
	{
		static constexpr LhsValueType lhsRvrValue[]{0, 1};
		static constexpr RhsValueType rhsRvrValue[]{0, 1};
		static constexpr RhsValueType rhsLvrValue[]{0, 1};

		static constexpr agnes::reference<LhsValueType&&> lhsRvr0{std::move(lhsRvrValue[0])};
		static constexpr agnes::reference<LhsValueType&&> lhsRvr1{std::move(lhsRvrValue[1])};
		static constexpr agnes::reference<RhsValueType&&> rhsRvr0{std::move(rhsRvrValue[0])};
		static constexpr agnes::reference<RhsValueType&&> rhsRvr1{std::move(rhsRvrValue[1])};

		static constexpr agnes::reference<RhsValueType&>  rhsLvr0{rhsLvrValue[0]};
		static constexpr agnes::reference<RhsValueType&>  rhsLvr1{rhsLvrValue[1]};

		static_assert(lhsRvr0 <= lhsRvr0);
		static_assert(lhsRvr0 == lhsRvr0);
		static_assert(lhsRvr0 >= lhsRvr0);

		static_assert(lhsRvr1 <= lhsRvr1);
		static_assert(lhsRvr1 == lhsRvr1);
		static_assert(lhsRvr1 >= lhsRvr1);

		static_assert(rhsRvr0 <= rhsRvr0);
		static_assert(rhsRvr0 == rhsRvr0);
		static_assert(rhsRvr0 >= rhsRvr0);

		static_assert(rhsRvr1 <= rhsRvr1);
		static_assert(rhsRvr1 == rhsRvr1);
		static_assert(rhsRvr1 >= rhsRvr1);

		static_assert(lhsRvr0 <  rhsRvr1);
		static_assert(lhsRvr0 <= rhsRvr1);
		static_assert(lhsRvr0 <= rhsRvr0);
		static_assert(lhsRvr0 == rhsRvr0);
		static_assert(lhsRvr0 >= rhsRvr0);

		static_assert(lhsRvr1 <= rhsRvr1);
		static_assert(lhsRvr1 == rhsRvr1);
		static_assert(lhsRvr1 >= rhsRvr1);
		static_assert(lhsRvr1 >= rhsRvr0);
		static_assert(lhsRvr1 >  rhsRvr0);

		static_assert(lhsRvr0 <  rhsLvr1);
		static_assert(lhsRvr0 <= rhsLvr1);
		static_assert(lhsRvr0 <= rhsLvr0);
		static_assert(lhsRvr0 == rhsLvr0);
		static_assert(lhsRvr0 >= rhsLvr0);

		static_assert(lhsRvr1 <= rhsLvr1);
		static_assert(lhsRvr1 == rhsLvr1);
		static_assert(lhsRvr1 >= rhsLvr1);
		static_assert(lhsRvr1 >= rhsLvr0);
		static_assert(lhsRvr1 >  rhsLvr0);
	}

	static constexpr auto test = []
	{
		LhsValueType lhsRvrValue[]{0, 1};
		RhsValueType rhsRvrValue[]{0, 1};
		RhsValueType rhsLvrValue[]{0, 1};

		agnes::reference<LhsValueType&> lhsRvr0{lhsRvrValue[0]};
		agnes::reference<LhsValueType&> lhsRvr1{lhsRvrValue[1]};
		agnes::reference<RhsValueType&> rhsRvr0{rhsRvrValue[0]};
		agnes::reference<RhsValueType&> rhsRvr1{rhsRvrValue[1]};

		agnes::reference<RhsValueType&&> rhsLvr0{std::move(rhsLvrValue[0])};
		agnes::reference<RhsValueType&&> rhsLvr1{std::move(rhsLvrValue[1])};

		AGNES_REQUIRE(lhsRvr0 <= lhsRvr0);
		AGNES_REQUIRE(lhsRvr0 == lhsRvr0);
		AGNES_REQUIRE(lhsRvr0 >= lhsRvr0);

		AGNES_REQUIRE(lhsRvr1 <= lhsRvr1);
		AGNES_REQUIRE(lhsRvr1 == lhsRvr1);
		AGNES_REQUIRE(lhsRvr1 >= lhsRvr1);

		AGNES_REQUIRE(rhsRvr0 <= rhsRvr0);
		AGNES_REQUIRE(rhsRvr0 == rhsRvr0);
		AGNES_REQUIRE(rhsRvr0 >= rhsRvr0);

		AGNES_REQUIRE(rhsRvr1 <= rhsRvr1);
		AGNES_REQUIRE(rhsRvr1 == rhsRvr1);
		AGNES_REQUIRE(rhsRvr1 >= rhsRvr1);

		AGNES_REQUIRE(lhsRvr0 <  rhsRvr1);
		AGNES_REQUIRE(lhsRvr0 <= rhsRvr1);
		AGNES_REQUIRE(lhsRvr0 <= rhsRvr0);
		AGNES_REQUIRE(lhsRvr0 == rhsRvr0);
		AGNES_REQUIRE(lhsRvr0 >= rhsRvr0);

		AGNES_REQUIRE(lhsRvr1 <= rhsRvr1);
		AGNES_REQUIRE(lhsRvr1 == rhsRvr1);
		AGNES_REQUIRE(lhsRvr1 >= rhsRvr1);
		AGNES_REQUIRE(lhsRvr1 >= rhsRvr0);
		AGNES_REQUIRE(lhsRvr1 >  rhsRvr0);

		AGNES_REQUIRE(lhsRvr0 <  rhsLvr1);
		AGNES_REQUIRE(lhsRvr0 <= rhsLvr1);
		AGNES_REQUIRE(lhsRvr0 <= rhsLvr0);
		AGNES_REQUIRE(lhsRvr0 == rhsLvr0);
		AGNES_REQUIRE(lhsRvr0 >= rhsLvr0);

		AGNES_REQUIRE(lhsRvr1 <= rhsLvr1);
		AGNES_REQUIRE(lhsRvr1 == rhsLvr1);
		AGNES_REQUIRE(lhsRvr1 >= rhsLvr1);
		AGNES_REQUIRE(lhsRvr1 >= rhsLvr0);
		AGNES_REQUIRE(lhsRvr1 >  rhsLvr0);

		return true;
	};

	AGNES_RUN(test);
}

TEMPLATE_TEST_CASE("agnes::reference<T&&...> deduction guides", "", int, const int)
{
	if constexpr (std::is_const_v<TestType>)
	{
		static constexpr TestType                     value   {0};
		static constexpr agnes::reference<TestType&&> agnesRvr{std::move(value)};

		static constexpr agnes::reference agnesRef0{std::move(value)};
		static constexpr agnes::reference agnesRef1{std::move(agnesRvr)};

		static_assert(std::is_same_v<decltype(agnesRef0), const agnes::reference<TestType&&>>);
		static_assert(std::is_same_v<decltype(agnesRef1), const agnes::reference<TestType&&>>);
	}

	static constexpr auto test = []
	{
		TestType                     value   {0};
		agnes::reference<TestType&&> agnesRvr{std::move(value)};

		agnes::reference agnesRef0{std::move(value)};
		agnes::reference agnesRef1{std::move(agnesRvr)};

		static_assert(std::is_same_v<decltype(agnesRef0), agnes::reference<TestType&&>>);
		static_assert(std::is_same_v<decltype(agnesRef1), agnes::reference<TestType&&>>);

		return true;
	};

	AGNES_RUN(test);
}

TEST_CASE("agnes::reference<T&&...> apply")
{
	static constexpr auto test = []
	{
		int  values[]{0, 1};
		bool applied {false};
		auto f0      {[&applied](int&, int&) { applied = true; }};
		auto f1      {[&applied](int&&, int&&) { applied = true; }};

		agnes::reference<int&&, int&&> agnesRef{std::move(values[0]), std::move(values[1])};

		agnes::apply(f0, agnesRef);

		AGNES_REQUIRE(applied);

		applied = false;

		agnes::apply(f1, std::move(agnesRef));

		AGNES_REQUIRE(applied);

		return true;
	};

	AGNES_RUN(test);
}

TEMPLATE_TEST_CASE_SIG("agnes::reference<T&&...> subset", "",
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

		const agnes::reference<T&&...> r{std::move(get<Ti>(values))...};

		const agnes::reference_subset_t<agnes::reference<T&&...>, Subset...> sub{subset<Subset...>(r)};

		AGNES_REQUIRE(((&get<Subset>(r) == &get<I>(sub)) && ...));

		return true;

	}(std::make_index_sequence<std::tuple_size_v<Types>>{},
		static_cast<Types*>(nullptr),
		std::make_index_sequence<sizeof...(Subset)>{}); };

	AGNES_RUN(test);
}
