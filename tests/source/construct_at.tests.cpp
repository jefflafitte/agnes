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

#include <memory>

#include <agnes/pointer.h>
#include <agnes/reference.h>

#include "agnes_tests/macros.h"
#include "agnes_tests/recorded.h"
#include "agnes_tests/throw_on_copy.h"

using namespace agnes_tests;

TEST_CASE("agnes::construct_at() default construct and destroy")
{
	using value_type     = recorded<int>;
	using allocator_type = std::allocator<value_type>;
	using pointer_type   = agnes::pointer<value_type*>;

	static constexpr auto test = []
	{
		allocator_type allocator;
		pointer_type   locations{allocator.allocate(1)};

		pointer_type p{agnes::construct_at(locations)};

		AGNES_REQUIRE(p == locations);
		AGNES_REQUIRE(get<0>(p)->actions.last() == class_action::defaultConstructed);

		class_actions actions;

		get<0>(p)->actions.copy_on_destruction(actions);

		agnes::destroy_at(locations);

		AGNES_REQUIRE(actions.last() == class_action::destructed);

		allocator.deallocate(get<0>(locations), 1);

		return true;
	};

	AGNES_RUN(test);
}

TEST_CASE("agnes::construct_at() direct construct and destroy")
{
	using value_type     = recorded<int>;
	using allocator_type = std::allocator<value_type>;
	using pointer_type   = agnes::pointer<value_type*>;

	static constexpr auto test = []
	{
		allocator_type allocator;
		pointer_type   locations{allocator.allocate(1)};
		int            value    {0};

		pointer_type p{agnes::construct_at(locations, value)};

		AGNES_REQUIRE(p == locations);
		AGNES_REQUIRE(get<0>(p)->actions.last() == class_action::directConstructed);
		AGNES_REQUIRE(get<0>(p)->value == value);

		class_actions actions;

		get<0>(p)->actions.copy_on_destruction(actions);

		agnes::destroy_at(locations);

		AGNES_REQUIRE(actions.last() == class_action::destructed);

		allocator.deallocate(get<0>(locations), 1);

		return true;
	};

	AGNES_RUN(test);
}

TEST_CASE("agnes::construct_at() copy construct and destroy")
{
	using value_type     = recorded<int>;
	using allocator_type = std::allocator<value_type>;
	using pointer_type   = agnes::pointer<value_type*>;

	static constexpr auto test = []
	{
		allocator_type allocator;
		pointer_type   locations{allocator.allocate(1)};
		value_type     value    {0};

		pointer_type p{agnes::construct_at(locations, value)};

		AGNES_REQUIRE(p == locations);
		AGNES_REQUIRE(get<0>(p)->actions.last() == class_action::copyConstructed);
		AGNES_REQUIRE(get<0>(p)->value == value);

		class_actions actions;

		get<0>(p)->actions.copy_on_destruction(actions);

		agnes::destroy_at(locations);

		AGNES_REQUIRE(actions.last() == class_action::destructed);

		allocator.deallocate(get<0>(locations), 1);

		return true;
	};

	AGNES_RUN(test);
}

TEST_CASE("agnes::construct_at() move construct and destroy")
{
	using value_type     = recorded<int>;
	using allocator_type = std::allocator<value_type>;
	using pointer_type   = agnes::pointer<value_type*>;

	static constexpr auto test = []
	{
		allocator_type allocator;
		pointer_type   locations{allocator.allocate(1)};
		value_type     value    {0};

		pointer_type p{agnes::construct_at(locations, std::move(value))};

		AGNES_REQUIRE(p == locations);
		AGNES_REQUIRE(get<0>(p)->actions.last() == class_action::moveConstructed);
		AGNES_REQUIRE(get<0>(p)->value == value);

		class_actions actions;

		get<0>(p)->actions.copy_on_destruction(actions);

		agnes::destroy_at(locations);

		AGNES_REQUIRE(actions.last() == class_action::destructed);

		allocator.deallocate(get<0>(locations), 1);

		return true;
	};

	AGNES_RUN(test);
}

TEST_CASE("agnes::construct_at() direct construct from reference and destroy")
{
	using value_type     = recorded<int>;
	using allocator_type = std::allocator<value_type>;
	using pointer_type   = agnes::pointer<value_type*>;
	using param_type     = int;
	using reference_type = agnes::reference<param_type&>;

	static constexpr auto test = []
	{
		allocator_type allocator;
		pointer_type   locations{allocator.allocate(1)};
		param_type     value    {0};
		reference_type r        {value};

		pointer_type p{agnes::construct_at(locations, r)};

		AGNES_REQUIRE(p == locations);
		AGNES_REQUIRE(get<0>(p)->actions.last() == class_action::directConstructed);
		AGNES_REQUIRE(get<0>(p)->value == value);

		class_actions actions;

		get<0>(p)->actions.copy_on_destruction(actions);

		agnes::destroy_at(locations);

		AGNES_REQUIRE(actions.last() == class_action::destructed);

		allocator.deallocate(get<0>(locations), 1);

		return true;
	};

	AGNES_RUN(test);
}

TEST_CASE("agnes::construct_at() copy construct from reference and destroy")
{
	using value_type     = recorded<int>;
	using allocator_type = std::allocator<value_type>;
	using pointer_type   = agnes::pointer<value_type*>;
	using reference_type = agnes::reference<value_type&>;

	static constexpr auto test = []
	{
		allocator_type allocator;
		pointer_type   locations{allocator.allocate(1)};
		value_type     value    {0};
		reference_type r        {value};

		pointer_type p{agnes::construct_at(locations, r)};

		AGNES_REQUIRE(p == locations);
		AGNES_REQUIRE(get<0>(p)->actions.last() == class_action::copyConstructed);
		AGNES_REQUIRE(get<0>(p)->value == value);

		class_actions actions;

		get<0>(p)->actions.copy_on_destruction(actions);

		agnes::destroy_at(locations);

		AGNES_REQUIRE(actions.last() == class_action::destructed);

		allocator.deallocate(get<0>(locations), 1);

		return true;
	};

	AGNES_RUN(test);
}

TEST_CASE("agnes::construct_at() move construct from reference and destroy")
{
	using value_type     = recorded<int>;
	using allocator_type = std::allocator<value_type>;
	using pointer_type   = agnes::pointer<value_type*>;
	using reference_type = agnes::reference<value_type&&>;

	static constexpr auto test = []
	{
		allocator_type allocator;
		pointer_type   locations{allocator.allocate(1)};
		value_type     value    {0};
		reference_type r        {std::move(value)};

		pointer_type p{agnes::construct_at(locations, std::move(r))};

		AGNES_REQUIRE(p == locations);
		AGNES_REQUIRE(get<0>(p)->actions.last() == class_action::moveConstructed);
		AGNES_REQUIRE(get<0>(p)->value == value);

		class_actions actions;

		get<0>(p)->actions.copy_on_destruction(actions);

		agnes::destroy_at(locations);

		AGNES_REQUIRE(actions.last() == class_action::destructed);

		allocator.deallocate(get<0>(locations), 1);

		return true;
	};

	AGNES_RUN(test);
}

TEST_CASE("agnes::construct_at() piecewise direct construct and destroy")
{
	using value_type     = recorded<int>;
	using allocator_type = std::allocator<value_type>;
	using pointer_type   = agnes::pointer<value_type*>;

	static constexpr auto test = []
	{
		allocator_type allocator;
		pointer_type   locations{allocator.allocate(1)};
		int            value    {0};

		pointer_type p{agnes::construct_at(
			locations,
			std::piecewise_construct,
			std::forward_as_tuple(value))};

		AGNES_REQUIRE(p == locations);
		AGNES_REQUIRE(get<0>(p)->actions.last() == class_action::directConstructed);
		AGNES_REQUIRE(get<0>(p)->value == value);

		class_actions actions;

		get<0>(p)->actions.copy_on_destruction(actions);

		agnes::destroy_at(locations);

		AGNES_REQUIRE(actions.last() == class_action::destructed);

		allocator.deallocate(get<0>(locations), 1);

		return true;
	};

	AGNES_RUN(test);
}

TEST_CASE("agnes::construct_at() piecewise copy construct and destroy")
{
	using value_type     = recorded<int>;
	using allocator_type = std::allocator<value_type>;
	using pointer_type   = agnes::pointer<value_type*>;

	static constexpr auto test = []
	{
		allocator_type allocator;
		pointer_type   locations{allocator.allocate(1)};
		value_type     value    {0};

		pointer_type p{agnes::construct_at(
			locations,
			std::piecewise_construct,
			std::forward_as_tuple(value))};

		AGNES_REQUIRE(p == locations);
		AGNES_REQUIRE(get<0>(p)->actions.last() == class_action::copyConstructed);
		AGNES_REQUIRE(get<0>(p)->value == value);

		class_actions actions;

		get<0>(p)->actions.copy_on_destruction(actions);

		agnes::destroy_at(locations);

		AGNES_REQUIRE(actions.last() == class_action::destructed);

		allocator.deallocate(get<0>(locations), 1);

		return true;
	};

	AGNES_RUN(test);
}

TEST_CASE("agnes::construct_at() piecewise move construct and destroy")
{
	using value_type     = recorded<int>;
	using allocator_type = std::allocator<value_type>;
	using pointer_type   = agnes::pointer<value_type*>;

	static constexpr auto test = []
	{
		allocator_type allocator;
		pointer_type   locations{allocator.allocate(1)};
		value_type     value    {0};

		pointer_type p{agnes::construct_at(
			locations,
			std::piecewise_construct,
			std::forward_as_tuple(std::move(value)))};

		AGNES_REQUIRE(p == locations);
		AGNES_REQUIRE(get<0>(p)->actions.last() == class_action::moveConstructed);
		AGNES_REQUIRE(get<0>(p)->value == value);

		class_actions actions;

		get<0>(p)->actions.copy_on_destruction(actions);

		agnes::destroy_at(locations);

		AGNES_REQUIRE(actions.last() == class_action::destructed);

		allocator.deallocate(get<0>(locations), 1);

		return true;
	};

	AGNES_RUN(test);
}

TEST_CASE("agnes::construct_at() piecewise mixed construct and destroy")
{
	using value_type     = recorded<int>;
	using allocator_type = std::allocator<value_type>;
	using pointer_type   = agnes::pointer<value_type*, value_type*>;

	static constexpr auto test = []
	{
		allocator_type allocator;
		pointer_type   locations{allocator.allocate(1), allocator.allocate(1)};
		value_type     values[] {0, 1};

		pointer_type p{agnes::construct_at(
			locations,
			std::piecewise_construct,
			std::forward_as_tuple(values[0]),
			std::forward_as_tuple(std::move(values[1])))};

		AGNES_REQUIRE(p == locations);
		AGNES_REQUIRE(get<0>(p)->actions.last() == class_action::copyConstructed);
		AGNES_REQUIRE(get<0>(p)->value == values[0]);
		AGNES_REQUIRE(get<1>(p)->actions.last() == class_action::moveConstructed);
		AGNES_REQUIRE(get<1>(p)->value == values[1]);

		bool wasDestructed[]{false, false};

		get<0>(p)->wasDestructed = &wasDestructed[0];
		get<1>(p)->wasDestructed = &wasDestructed[1];

		agnes::destroy_at(locations);

		AGNES_REQUIRE(wasDestructed[0]);
		AGNES_REQUIRE(wasDestructed[1]);

		allocator.deallocate(get<0>(locations), 1);
		allocator.deallocate(get<1>(locations), 1);

		return true;
	};

	AGNES_RUN(test);
}

TEST_CASE("agnes::construct_at() throw during copy construct")
{
	using value_type0     = recorded<int>;
	using value_type1     = throw_on_copy<int>;
	using allocator_type0 = std::allocator<value_type0>;
	using allocator_type1 = std::allocator<value_type1>;
	using pointer_type    = agnes::pointer<value_type0*, value_type1*>;

	auto test = []
	{
		allocator_type0 allocator0;
		allocator_type1 allocator1;
		pointer_type    locations{allocator0.allocate(1), allocator1.allocate(1)};
		value_type0     value0   {0};
		value_type1     value1   {1};

		value1.throwWhenCopied = true;

		pointer_type p{agnes::construct_at(locations, value0, value1)};
	};

	REQUIRE_THROWS(test());
}
