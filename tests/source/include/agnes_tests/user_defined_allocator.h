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

#pragma once

#include <cstddef>
#include <type_traits>

#include <agnes/fields.h>
#include <agnes/memory.h>
#include <agnes/pointer.h>

namespace agnes_tests
{
	template <bool POCMA, bool AlwaysEqual, typename... T>
	struct user_defined_allocator_base
	{
	public:
		using value_type                             = agnes::fields<T...>;
		using size_type                              = std::size_t;
		using difference_type                        = std::ptrdiff_t;
		using propagate_on_container_move_assignment = std::integral_constant<bool, POCMA>;
		using is_always_equal                        = std::integral_constant<bool, AlwaysEqual>;

		constexpr user_defined_allocator_base() noexcept = default;

		constexpr user_defined_allocator_base(const user_defined_allocator_base&) = default;

		template <typename... U>
		constexpr user_defined_allocator_base(const user_defined_allocator_base<POCMA, AlwaysEqual, U...>&) noexcept {}

		constexpr ~user_defined_allocator_base() = default;

		[[nodiscard]] constexpr agnes::pointer<T*...> allocate(const std::size_t n)
		{ return agnes::allocator<T...>{}.allocate(n); }

		constexpr void deallocate(const agnes::pointer<T*...>& p, const std::size_t n)
		{ return agnes::allocator<T...>{}.deallocate(p, n); }
	};

	template <bool POCMA, bool AlwaysEqual, typename... T, typename... U>
	constexpr bool operator==(
		const user_defined_allocator_base<POCMA, AlwaysEqual, T...>&,
		const user_defined_allocator_base<POCMA, AlwaysEqual, U...>&) noexcept
	{ return AlwaysEqual; }

	template <typename... T>
	struct user_defined_allocator_pocma : user_defined_allocator_base<true, true, T...> {};

	template <typename... T>
	struct user_defined_allocator_no_pocma_always_equal : user_defined_allocator_base<false, true, T...> {};

	template <typename... T>
	struct user_defined_allocator_no_pocma_not_always_equal : user_defined_allocator_base<false, false, T...> {};
}
