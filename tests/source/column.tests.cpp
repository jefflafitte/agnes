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

#include <agnes/column.h>

#include <type_traits>

#include "agnes_tests/columns.h"
#include "agnes_tests/macros.h"
#include "agnes_tests/recorded.h"

using namespace agnes_tests;

static_assert(valid_column<column_two_arg, int>);
static_assert(std::is_same_v<decltype(column_two_arg::two_arg), int&>);
static_assert(std::is_same_v<decltype(const_column_two_arg::two_arg), const int&>);

static_assert(valid_column<column_three_arg, int>);
static_assert(std::is_same_v<decltype(column_three_arg::three_arg), int&>);
static_assert(std::is_same_v<decltype(const_column_three_arg::three_arg), const int&>);

static_assert(valid_column<column_x, float>);
static_assert(std::is_same_v<decltype(column_x::x), float&>);
static_assert(std::is_same_v<decltype(const_column_x::x), const float&>);

static_assert(valid_column<column_y, float>);
static_assert(std::is_same_v<decltype(column_y::y), float&>);
static_assert(std::is_same_v<decltype(const_column_y::y), const float&>);

static_assert(valid_column<column_z, float>);
static_assert(std::is_same_v<decltype(column_z::z), float&>);
static_assert(std::is_same_v<decltype(const_column_z::z), const float&>);

static_assert(valid_column<column_one_arg<int>, int>);
static_assert(std::is_same_v<decltype(column_one_arg<int>::one_arg), int&>);
static_assert(std::is_same_v<decltype(column_one_arg<const int>::one_arg), const int&>);

static_assert(valid_column<column_a<int>, int>);
static_assert(std::is_same_v<decltype(column_a<int>::a), int&>);
static_assert(std::is_same_v<decltype(column_a<const int>::a), const int&>);

static_assert(valid_column<column_b<int>, int>);
static_assert(std::is_same_v<decltype(column_b<int>::b), int&>);
static_assert(std::is_same_v<decltype(column_b<const int>::b), const int&>);

static_assert(valid_column<column_c<int>, int>);
static_assert(std::is_same_v<decltype(column_c<int>::c), int&>);
static_assert(std::is_same_v<decltype(column_c<const int>::c), const int&>);

static_assert(valid_column<column_i, recorded<int>>);
static_assert(std::is_same_v<decltype(column_i::i), recorded<int>&>);
static_assert(std::is_same_v<decltype(const_column_i::i), const recorded<int>&>);

static_assert(valid_column<column_j, recorded<int>>);
static_assert(std::is_same_v<decltype(column_j::i), recorded<int>&>);
static_assert(std::is_same_v<decltype(const_column_j::i), const recorded<int>&>);

TEMPLATE_TEST_CASE("agnes column direct construction", "",
	(column_one_arg<int>),
	(column_two_arg),
	(column_three_arg),
	(column_a<int>),
	(column_b<int>),
	(column_c<int>),
	(column_i),
	(column_j),
	(column_x),
	(column_y),
	(column_z))
{
	using Column           = TestType;
	using ConstColumn      = agnes::const_column_t<Column>;
	using ElementType      = agnes::column_element_t<Column>;
	using ConstElementType = agnes::column_element_t<ConstColumn>;

	static constexpr auto test = []
	{
		{
			ElementType value{static_cast<ElementType>(0)};

			Column column{value};

			AGNES_REQUIRE(&column.get() == &value);
		}

		{
			ElementType value{static_cast<ElementType>(0)};

			ConstColumn column{value};

			AGNES_REQUIRE(&column.get() == &value);
		}

		{
			ConstElementType value{static_cast<ElementType>(0)};

			ConstColumn column{value};

			AGNES_REQUIRE(&column.get() == &value);
		}

		return true;
	};

	AGNES_RUN(test);
}

TEMPLATE_TEST_CASE("agnes column copy construction", "",
	(column_one_arg<int>),
	(column_two_arg),
	(column_three_arg),
	(column_a<int>),
	(column_b<int>),
	(column_c<int>),
	(column_i),
	(column_j),
	(column_x),
	(column_y),
	(column_z))
{
	using Column      = TestType;
	using ConstColumn = agnes::const_column_t<Column>;
	using ElementType = agnes::column_element_t<Column>;

	static constexpr auto test = []
	{
		{
			ElementType value{static_cast<ElementType>(0)};

			const Column other{value};

			Column column{other};

			AGNES_REQUIRE(&column.get() == &value);
		}

		{
			ElementType value{static_cast<ElementType>(0)};

			const Column other{value};

			ConstColumn column{other};

			AGNES_REQUIRE(&column.get() == &value);
		}

		{
			ElementType value{static_cast<ElementType>(0)};

			const ConstColumn other{value};

			ConstColumn column{other};

			AGNES_REQUIRE(&column.get() == &value);
		}

		return true;
	};

	AGNES_RUN(test);
}

TEMPLATE_TEST_CASE("agnes column move construction", "",
	(column_one_arg<int>),
	(column_two_arg),
	(column_three_arg),
	(column_a<int>),
	(column_b<int>),
	(column_c<int>),
	(column_i),
	(column_j),
	(column_x),
	(column_y),
	(column_z))
{
	using Column      = TestType;
	using ConstColumn = agnes::const_column_t<Column>;
	using ElementType = agnes::column_element_t<Column>;

	static constexpr auto test = []
	{
		{
			ElementType value{static_cast<ElementType>(0)};

			Column other{value};

			Column column{std::move(other)};

			AGNES_REQUIRE(&column.get() == &value);
		}

		{
			ElementType value{static_cast<ElementType>(0)};

			Column other{value};

			ConstColumn column{std::move(other)};

			AGNES_REQUIRE(&column.get() == &value);
		}

		{
			ElementType value{static_cast<ElementType>(0)};

			ConstColumn other{value};

			ConstColumn column{std::move(other)};

			AGNES_REQUIRE(&column.get() == &value);
		}

		return true;
	};

	AGNES_RUN(test);
}

TEMPLATE_TEST_CASE("agnes column copy assignment", "",
	(column_one_arg<int>),
	(column_two_arg),
	(column_three_arg),
	(column_a<int>),
	(column_b<int>),
	(column_c<int>),
	(column_i),
	(column_j),
	(column_x),
	(column_y),
	(column_z),
	column_a<recorded<int>>)
{
	using Column      = TestType;
	using ConstColumn = agnes::const_column_t<Column>;
	using ElementType = agnes::column_element_t<Column>;

	static constexpr auto test = []
	{
		{
			ElementType value     {static_cast<ElementType>(0)};
			ElementType otherValue{static_cast<ElementType>(1)};

			const Column other{otherValue};

			Column column{value};

			column = other;

			AGNES_REQUIRE(column.get() == otherValue);

			if constexpr (std::is_same_v<ElementType, recorded<int>>)
			{
				AGNES_REQUIRE(column.get().actions.last() ==  class_action::copyAssigned);
			}
		}

		{
			ElementType value     {static_cast<ElementType>(0)};
			ElementType otherValue{static_cast<ElementType>(1)};

			const ConstColumn other{otherValue};

			Column column{value};

			column = other;

			AGNES_REQUIRE(column.get() == otherValue);

			if constexpr (std::is_same_v<ElementType, recorded<int>>)
			{
				AGNES_REQUIRE(column.get().actions.last() ==  class_action::copyAssigned);
			}
		}

		return true;
	};

	AGNES_RUN(test);
}

TEMPLATE_TEST_CASE("agnes column move assignment", "",
	(column_one_arg<int>),
	(column_two_arg),
	(column_three_arg),
	(column_a<int>),
	(column_b<int>),
	(column_c<int>),
	(column_i),
	(column_j),
	(column_x),
	(column_y),
	(column_z),
	column_a<recorded<int>>)
{
	using Column      = TestType;
	using ElementType = agnes::column_element_t<Column>;

	static constexpr auto test = []
	{
		ElementType value     {static_cast<ElementType>(0)};
		ElementType otherValue{static_cast<ElementType>(1)};

		Column other{otherValue};

		Column column{value};

		column = std::move(other);

		AGNES_REQUIRE(column.get() == otherValue);

		if constexpr (std::is_same_v<ElementType, recorded<int>>)
		{
			AGNES_REQUIRE(column.get().actions.last() ==  class_action::moveAssigned);
		}

		return true;
	};

	AGNES_RUN(test);
}

TEMPLATE_TEST_CASE("agnes column compare", "",
	(column_one_arg<int>),
	(column_two_arg),
	(column_three_arg),
	(column_a<int>),
	(column_b<int>),
	(column_c<int>),
	(column_i),
	(column_j),
	(column_x),
	(column_y),
	(column_z))
{
	using Column      = TestType;
	using ConstColumn = agnes::const_column_t<Column>;
	using ElementType = agnes::column_element_t<Column>;

	static constexpr auto test = []
	{
		ElementType value       {static_cast<ElementType>(1)};
		ElementType lessValue   {static_cast<ElementType>(0)};
		ElementType greaterValue{static_cast<ElementType>(2)};

		Column      column       {value};
		ConstColumn constColumn  {value};
		Column      less         {lessValue};
		ConstColumn constLess    {lessValue};
		Column      equal        {value};
		ConstColumn constEqual   {value};
		Column      greater      {greaterValue};
		ConstColumn constGreater {greaterValue};

		AGNES_REQUIRE(!(column <  column));
		AGNES_REQUIRE(  column <= column );
		AGNES_REQUIRE(  column == column );
		AGNES_REQUIRE(  column >= column );
		AGNES_REQUIRE(!(column >  column));

		AGNES_REQUIRE(!(column <  constColumn));
		AGNES_REQUIRE(  column <= constColumn );
		AGNES_REQUIRE(  column == constColumn );
		AGNES_REQUIRE(  column >= constColumn );
		AGNES_REQUIRE(!(column >  constColumn));

		AGNES_REQUIRE(!(constColumn <  constColumn));
		AGNES_REQUIRE(  constColumn <= constColumn );
		AGNES_REQUIRE(  constColumn == constColumn );
		AGNES_REQUIRE(  constColumn >= constColumn );
		AGNES_REQUIRE(!(constColumn >  constColumn));

		AGNES_REQUIRE(!(constColumn <  column));
		AGNES_REQUIRE(  constColumn <= column );
		AGNES_REQUIRE(  constColumn == column );
		AGNES_REQUIRE(  constColumn >= column );
		AGNES_REQUIRE(!(constColumn >  column));

		AGNES_REQUIRE(!(column <  less));
		AGNES_REQUIRE(!(column <= less));
		AGNES_REQUIRE(!(column == less));
		AGNES_REQUIRE(  column >= less );
		AGNES_REQUIRE(  column >  less );

		AGNES_REQUIRE(!(column <  constLess));
		AGNES_REQUIRE(!(column <= constLess));
		AGNES_REQUIRE(!(column == constLess));
		AGNES_REQUIRE(  column >= constLess );
		AGNES_REQUIRE(  column >  constLess );

		AGNES_REQUIRE(!(constColumn <  constLess));
		AGNES_REQUIRE(!(constColumn <= constLess));
		AGNES_REQUIRE(!(constColumn == constLess));
		AGNES_REQUIRE(  constColumn >= constLess );
		AGNES_REQUIRE(  constColumn >  constLess );

		AGNES_REQUIRE(!(constColumn <  less));
		AGNES_REQUIRE(!(constColumn <= less));
		AGNES_REQUIRE(!(constColumn == less));
		AGNES_REQUIRE(  constColumn >= less );
		AGNES_REQUIRE(  constColumn >  less );

		AGNES_REQUIRE(!(column <  equal));
		AGNES_REQUIRE(  column <= equal );
		AGNES_REQUIRE(  column == equal );
		AGNES_REQUIRE(  column >= equal );
		AGNES_REQUIRE(!(column >  equal));

		AGNES_REQUIRE(!(column <  constEqual));
		AGNES_REQUIRE(  column <= constEqual );
		AGNES_REQUIRE(  column == constEqual );
		AGNES_REQUIRE(  column >= constEqual );
		AGNES_REQUIRE(!(column >  constEqual));

		AGNES_REQUIRE(!(constColumn <  constEqual));
		AGNES_REQUIRE(  constColumn <= constEqual );
		AGNES_REQUIRE(  constColumn == constEqual );
		AGNES_REQUIRE(  constColumn >= constEqual );
		AGNES_REQUIRE(!(constColumn >  constEqual));

		AGNES_REQUIRE(!(constColumn <  equal));
		AGNES_REQUIRE(  constColumn <= equal );
		AGNES_REQUIRE(  constColumn == equal );
		AGNES_REQUIRE(  constColumn >= equal );
		AGNES_REQUIRE(!(constColumn >  equal));

		AGNES_REQUIRE(  column <  greater );
		AGNES_REQUIRE(  column <= greater );
		AGNES_REQUIRE(!(column == greater));
		AGNES_REQUIRE(!(column >= greater));
		AGNES_REQUIRE(!(column >  greater));

		AGNES_REQUIRE(  column <  constGreater );
		AGNES_REQUIRE(  column <= constGreater );
		AGNES_REQUIRE(!(column == constGreater));
		AGNES_REQUIRE(!(column >= constGreater));
		AGNES_REQUIRE(!(column >  constGreater));

		AGNES_REQUIRE(  constColumn <  constGreater );
		AGNES_REQUIRE(  constColumn <= constGreater );
		AGNES_REQUIRE(!(constColumn == constGreater));
		AGNES_REQUIRE(!(constColumn >= constGreater));
		AGNES_REQUIRE(!(constColumn >  constGreater));

		AGNES_REQUIRE(  constColumn <  greater );
		AGNES_REQUIRE(  constColumn <= greater );
		AGNES_REQUIRE(!(constColumn == greater));
		AGNES_REQUIRE(!(constColumn >= greater));
		AGNES_REQUIRE(!(constColumn >  greater));

		return true;
	};

	AGNES_RUN(test);
}
