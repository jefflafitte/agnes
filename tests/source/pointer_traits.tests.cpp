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

#include <type_traits>

#include <agnes/fields.h>
#include <agnes/pointer.h>
#include <agnes/reference.h>

#include "agnes_tests/macros.h"
#include "agnes_tests/user_defined_pointer.h"

using namespace agnes_tests;

static_assert(std::is_same_v<agnes::pointer_traits<agnes::pointer<int*>             >::difference_type, std::ptrdiff_t>);
static_assert(std::is_same_v<agnes::pointer_traits<user_defined_pointer<int>        >::difference_type, std::ptrdiff_t>);
static_assert(std::is_same_v<agnes::pointer_traits<user_defined_pointer_no_defs<int>>::difference_type, std::ptrdiff_t>);

static_assert(std::is_same_v<agnes::pointer_traits<agnes::pointer<int*>             >::element_type, agnes::fields<int>>);
static_assert(std::is_same_v<agnes::pointer_traits<user_defined_pointer<int>        >::element_type, agnes::fields<int>>);
static_assert(std::is_same_v<agnes::pointer_traits<user_defined_pointer_no_defs<int>>::element_type, agnes::fields<int>>);

static_assert(std::is_same_v<agnes::pointer_traits<agnes::pointer<int*>             >::rebind<float>, agnes::pointer              <float*>>);
static_assert(std::is_same_v<agnes::pointer_traits<user_defined_pointer<int>        >::rebind<float>, user_defined_pointer        <float >>);
static_assert(std::is_same_v<agnes::pointer_traits<user_defined_pointer_no_defs<int>>::rebind<float>, user_defined_pointer_no_defs<float >>);

TEST_CASE("agnes::pointer_traits<agnes::pointer> pointer_to")
{
	{
		static constexpr int                          value{0};
		static constexpr agnes::reference<const int&> r    {value};
		static constexpr agnes::pointer<const int*>   p    {&value};

		static_assert(agnes::pointer_traits<agnes::pointer<const int*>>::pointer_to(r) == p);
	}

	static constexpr auto test = []
	{
		int                    value{0};
		agnes::reference<int&> r    {value};
		agnes::pointer<int*>   p    {&value};

		AGNES_REQUIRE(agnes::pointer_traits<agnes::pointer<const int*>>::pointer_to(r) == p);

		return true;
	};

	AGNES_RUN(test);
}

TEST_CASE("agnes::pointer_traits<user-defined pointer> pointer_to")
{
	int                       value{0};
	agnes::reference<int&>    r    {value};
	user_defined_pointer<int> p    {&value};

	REQUIRE(agnes::pointer_traits<user_defined_pointer<int>>::pointer_to(r) == p);
}

TEST_CASE("agnes::pointer_traits<user-defined pointer (no defs)> pointer_to")
{
	int                               value{0};
	agnes::reference<int&>            r    {value};
	user_defined_pointer_no_defs<int> p    {&value};

	REQUIRE(agnes::pointer_traits<user_defined_pointer_no_defs<int>>::pointer_to(r) == p);
}
