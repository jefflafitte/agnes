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

#include <agnes/pointer.h>
#include <agnes/reference.h>

#include "agnes_tests/macros.h"
#include "agnes_tests/recorded.h"

using namespace agnes_tests;

static_assert(std::is_same_v<agnes::allocator_traits<agnes::allocator<int>>::value_type        , agnes::fields <      int  >>);
static_assert(std::is_same_v<agnes::allocator_traits<agnes::allocator<int>>::pointer           , agnes::pointer<      int* >>);
static_assert(std::is_same_v<agnes::allocator_traits<agnes::allocator<int>>::const_pointer     , agnes::pointer<const int* >>);
static_assert(std::is_same_v<agnes::allocator_traits<agnes::allocator<int>>::void_pointer      , agnes::pointer<      void*>>);
static_assert(std::is_same_v<agnes::allocator_traits<agnes::allocator<int>>::const_void_pointer, agnes::pointer<const void*>>);

static_assert(std::is_same_v<agnes::allocator_traits<agnes::allocator<int>>::difference_type, std::ptrdiff_t>);
static_assert(std::is_same_v<agnes::allocator_traits<agnes::allocator<int>>::size_type      , std::size_t   >);

static_assert(std::is_same_v<agnes::allocator_traits<agnes::allocator<int>>::propagate_on_container_copy_assignment, std::false_type>);
static_assert(std::is_same_v<agnes::allocator_traits<agnes::allocator<int>>::propagate_on_container_move_assignment, std::true_type >);
static_assert(std::is_same_v<agnes::allocator_traits<agnes::allocator<int>>::propagate_on_container_swap           , std::false_type>);
static_assert(std::is_same_v<agnes::allocator_traits<agnes::allocator<int>>::is_always_equal                       , std::true_type >);

static_assert(std::is_same_v<agnes::allocator_traits<agnes::allocator<int>>::rebind_alloc <float>,                         agnes::allocator<float> >);
static_assert(std::is_same_v<agnes::allocator_traits<agnes::allocator<int>>::rebind_traits<float>, agnes::allocator_traits<agnes::allocator<float>>>);

TEST_CASE("agnes::allocator_traits allocate/deallocate")
{
	static constexpr auto test = []
	{
		agnes::allocator<int> a;

		agnes::pointer<int*> p{agnes::allocator_traits<agnes::allocator<int>>::allocate(a, 1)};

		AGNES_REQUIRE(p != nullptr);

		agnes::allocator_traits<agnes::allocator<int>>::deallocate(a, p, 1);

		return true;
	};

	AGNES_RUN(test);
}

TEST_CASE("agnes::allocator_traits default construct and destroy")
{
	using value_type     = recorded<int>;
	using allocator_type = agnes::allocator<value_type>;
	using pointer_type   = agnes::pointer<value_type*>;

	const auto xxx = sizeof(recorded<int>);

	int i = 0;

	static constexpr auto test = []
	{
		allocator_type a;
		pointer_type   p{agnes::allocator_traits<allocator_type>::allocate(a, 1)};

		agnes::allocator_traits<allocator_type>::construct(a, p);

		AGNES_REQUIRE(get<0>(p)->actions.last() == class_action::defaultConstructed);

		class_actions actions;

		get<0>(p)->actions.copy_on_destruction(actions);

		agnes::allocator_traits<allocator_type>::destroy(a, p);

		AGNES_REQUIRE(actions.last() == class_action::destructed);

		agnes::allocator_traits<allocator_type>::deallocate(a, p, 1);

		return true;
	};

	AGNES_RUN(test);
}

TEST_CASE("agnes::allocator_traits direct construct and destroy")
{
	using value_type     = recorded<int>;
	using allocator_type = agnes::allocator<value_type>;
	using pointer_type   = agnes::pointer<value_type*>;

	static constexpr auto test = []
	{
		allocator_type a;
		pointer_type   p    {agnes::allocator_traits<allocator_type>::allocate(a, 1)};
		int            value{0};

		agnes::allocator_traits<allocator_type>::construct(a, p, value);

		AGNES_REQUIRE(get<0>(p)->actions.last() == class_action::directConstructed);

		AGNES_REQUIRE(get<0>(p)->value == value);

		class_actions actions;

		get<0>(p)->actions.copy_on_destruction(actions);

		agnes::allocator_traits<allocator_type>::destroy(a, p);

		AGNES_REQUIRE(actions.last() == class_action::destructed);

		agnes::allocator_traits<allocator_type>::deallocate(a, p, 1);

		return true;
	};

	AGNES_RUN(test);
}

TEST_CASE("agnes::allocator_traits copy construct and destroy")
{
	using value_type     = recorded<int>;
	using allocator_type = agnes::allocator<value_type>;
	using pointer_type   = agnes::pointer<value_type*>;

	static constexpr auto test = []
	{
		allocator_type a;
		pointer_type   p    {agnes::allocator_traits<allocator_type>::allocate(a, 1)};
		value_type     value{0};

		agnes::allocator_traits<allocator_type>::construct(a, p, value);

		AGNES_REQUIRE(get<0>(p)->actions.last() == class_action::copyConstructed);

		AGNES_REQUIRE(get<0>(p)->value == value);

		class_actions actions;

		get<0>(p)->actions.copy_on_destruction(actions);

		agnes::allocator_traits<allocator_type>::destroy(a, p);

		AGNES_REQUIRE(actions.last() == class_action::destructed);

		agnes::allocator_traits<allocator_type>::deallocate(a, p, 1);

		return true;
	};

	AGNES_RUN(test);
}

TEST_CASE("agnes::allocator_traits move construct and destroy")
{
	using value_type     = recorded<int>;
	using allocator_type = agnes::allocator<value_type>;
	using pointer_type   = agnes::pointer<value_type*>;

	static constexpr auto test = []
	{
		allocator_type a;
		pointer_type   p    {agnes::allocator_traits<allocator_type>::allocate(a, 1)};
		value_type     value{0};

		agnes::allocator_traits<allocator_type>::construct(a, p, std::move(value));

		AGNES_REQUIRE(get<0>(p)->actions.last() == class_action::moveConstructed);

		AGNES_REQUIRE(get<0>(p)->value == value);

		class_actions actions;

		get<0>(p)->actions.copy_on_destruction(actions);

		agnes::allocator_traits<allocator_type>::destroy(a, p);

		AGNES_REQUIRE(actions.last() == class_action::destructed);

		agnes::allocator_traits<allocator_type>::deallocate(a, p, 1);

		return true;
	};

	AGNES_RUN(test);
}

TEST_CASE("agnes::allocator_traits direct construct from reference and destroy")
{
	using value_type     = recorded<int>;
	using allocator_type = agnes::allocator<value_type>;
	using pointer_type   = agnes::pointer<value_type*>;
	using param_type     = int;
	using reference_type = agnes::reference<param_type&>;

	static constexpr auto test = []
	{
		allocator_type a;
		pointer_type   p    {agnes::allocator_traits<allocator_type>::allocate(a, 1)};
		param_type     value{0};
		reference_type r    {value};

		agnes::allocator_traits<allocator_type>::construct(a, p, r);

		AGNES_REQUIRE(get<0>(p)->actions.last() == class_action::directConstructed);

		AGNES_REQUIRE(get<0>(p)->value == value);

		class_actions actions;

		get<0>(p)->actions.copy_on_destruction(actions);

		agnes::allocator_traits<allocator_type>::destroy(a, p);

		AGNES_REQUIRE(actions.last() == class_action::destructed);

		agnes::allocator_traits<allocator_type>::deallocate(a, p, 1);

		return true;
	};

	AGNES_RUN(test);
}

TEST_CASE("agnes::allocator_traits copy construct from reference and destroy")
{
	using value_type     = recorded<int>;
	using allocator_type = agnes::allocator<value_type>;
	using pointer_type   = agnes::pointer<value_type*>;
	using reference_type = agnes::reference<value_type&>;

	static constexpr auto test = []
	{
		allocator_type a;
		pointer_type   p    {agnes::allocator_traits<allocator_type>::allocate(a, 1)};
		value_type     value{0};
		reference_type r    {value};

		agnes::allocator_traits<allocator_type>::construct(a, p, r);

		AGNES_REQUIRE(get<0>(p)->actions.last() == class_action::copyConstructed);

		AGNES_REQUIRE(get<0>(p)->value == value);

		class_actions actions;

		get<0>(p)->actions.copy_on_destruction(actions);

		agnes::allocator_traits<allocator_type>::destroy(a, p);

		AGNES_REQUIRE(actions.last() == class_action::destructed);

		agnes::allocator_traits<allocator_type>::deallocate(a, p, 1);

		return true;
	};

	AGNES_RUN(test);
}

TEST_CASE("agnes::allocator_traits move construct from reference when type not move-constructible and destroy")
{
	using value_type     = recorded<int>;
	using allocator_type = agnes::allocator<value_type>;
	using pointer_type   = agnes::pointer<value_type*>;
	using reference_type = agnes::reference<value_type&&>;

	static constexpr auto test = []
	{
		allocator_type a;
		pointer_type   p    {agnes::allocator_traits<allocator_type>::allocate(a, 1)};
		value_type     value{0};
		reference_type r    {std::move(value)};

		agnes::allocator_traits<allocator_type>::construct(a, p, std::move(r));

		AGNES_REQUIRE(get<0>(p)->actions.last() == class_action::moveConstructed);

		AGNES_REQUIRE(get<0>(p)->value == value);

		class_actions actions;

		get<0>(p)->actions.copy_on_destruction(actions);

		agnes::allocator_traits<allocator_type>::destroy(a, p);

		AGNES_REQUIRE(actions.last() == class_action::destructed);

		agnes::allocator_traits<allocator_type>::deallocate(a, p, 1);

		return true;
	};

	AGNES_RUN(test);
}

TEST_CASE("agnes::allocator_traits piecewise direct construct and destroy")
{
	using value_type     = recorded<int>;
	using allocator_type = agnes::allocator<value_type>;
	using pointer_type   = agnes::pointer<value_type*>;

	static constexpr auto test = []
	{
		allocator_type a;
		pointer_type   p    {agnes::allocator_traits<allocator_type>::allocate(a, 1)};
		int            value{0};

		agnes::allocator_traits<allocator_type>::construct(
			a,
			p,
			std::piecewise_construct,
			std::forward_as_tuple(value));

		AGNES_REQUIRE(get<0>(p)->actions.last() == class_action::directConstructed);

		AGNES_REQUIRE(get<0>(p)->value == value);

		class_actions actions;

		get<0>(p)->actions.copy_on_destruction(actions);

		agnes::allocator_traits<allocator_type>::destroy(a, p);

		AGNES_REQUIRE(actions.last() == class_action::destructed);

		agnes::allocator_traits<allocator_type>::deallocate(a, p, 1);

		return true;
	};

	AGNES_RUN(test);
}

TEST_CASE("agnes::allocator_traits piecewise copy construct and destroy")
{
	using value_type     = recorded<int>;
	using allocator_type = agnes::allocator<value_type>;
	using pointer_type   = agnes::pointer<value_type*>;

	static constexpr auto test = []
	{
		allocator_type a;
		pointer_type   p    {agnes::allocator_traits<allocator_type>::allocate(a, 1)};
		value_type     value{0};

		agnes::allocator_traits<allocator_type>::construct(
			a,
			p,
			std::piecewise_construct,
			std::forward_as_tuple(value));

		AGNES_REQUIRE(get<0>(p)->actions.last() == class_action::copyConstructed);

		AGNES_REQUIRE(get<0>(p)->value == value);

		class_actions actions;

		get<0>(p)->actions.copy_on_destruction(actions);

		agnes::allocator_traits<allocator_type>::destroy(a, p);

		AGNES_REQUIRE(actions.last() == class_action::destructed);

		agnes::allocator_traits<allocator_type>::deallocate(a, p, 1);

		return true;
	};

	AGNES_RUN(test);
}

TEST_CASE("agnes::allocator_traits piecewise move construct and destroy")
{
	using value_type     = recorded<int>;
	using allocator_type = agnes::allocator<value_type>;
	using pointer_type   = agnes::pointer<value_type*>;

	static constexpr auto test = []
	{
		allocator_type a;
		pointer_type   p    {agnes::allocator_traits<allocator_type>::allocate(a, 1)};
		value_type     value{0};

		agnes::allocator_traits<allocator_type>::construct(
			a,
			p,
			std::piecewise_construct,
			std::forward_as_tuple(std::move(value)));

		AGNES_REQUIRE(get<0>(p)->actions.last() == class_action::moveConstructed);

		AGNES_REQUIRE(get<0>(p)->value == value);

		class_actions actions;

		get<0>(p)->actions.copy_on_destruction(actions);

		agnes::allocator_traits<allocator_type>::destroy(a, p);

		AGNES_REQUIRE(actions.last() == class_action::destructed);

		agnes::allocator_traits<allocator_type>::deallocate(a, p, 1);

		return true;
	};

	AGNES_RUN(test);
}

TEST_CASE("agnes::allocator_traits piecewise mixed construct and destroy")
{
	using value_type     = recorded<int>;
	using allocator_type = agnes::allocator<value_type, value_type>;
	using pointer_type   = agnes::pointer<value_type*, value_type*>;

	static constexpr auto test = []
	{
		allocator_type a;
		pointer_type   p       {agnes::allocator_traits<allocator_type>::allocate(a, 1)};
		value_type     values[]{0, 1};

		agnes::allocator_traits<allocator_type>::construct(
			a,
			p,
			std::piecewise_construct,
			std::forward_as_tuple(values[0]),
			std::forward_as_tuple(std::move(values[1])));

		AGNES_REQUIRE(get<0>(p)->actions.last() == class_action::copyConstructed);

		AGNES_REQUIRE(get<0>(p)->value == values[0]);

		AGNES_REQUIRE(get<1>(p)->actions.last() == class_action::moveConstructed);

		AGNES_REQUIRE(get<1>(p)->value == values[1]);

		bool wasDestructed[]{false, false};

		get<0>(p)->wasDestructed = &wasDestructed[0];
		get<1>(p)->wasDestructed = &wasDestructed[1];

		agnes::allocator_traits<allocator_type>::destroy(a, p);

		AGNES_REQUIRE(wasDestructed[0]);
		AGNES_REQUIRE(wasDestructed[1]);

		agnes::allocator_traits<allocator_type>::deallocate(a, p, 1);

		return true;
	};

	AGNES_RUN(test);
}
