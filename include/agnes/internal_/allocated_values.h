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
#include <memory>
#include <tuple>
#include <utility>

#include <agnes/memory.h>
#include <agnes/pointer.h>
#include <agnes/reference.h>

namespace agnes::internal_
{
	template <typename Allocator, typename... T>
	class allocated_values
	{
	public:
		template <typename... Args>
		constexpr explicit allocated_values(Allocator& alloc, Args&&... args) :
			allocated_values{indexes, alloc, std::forward<Args>(args)...} {}

		constexpr ~allocated_values() { allocator_traits::destroy(alloc_, address_); }

		constexpr reference<T&...> get() const { return *address_; }

	private:
		using allocator_traits = agnes::allocator_traits<Allocator>;

		static constexpr auto indexes = std::index_sequence_for<T...>{};

		template <std::size_t... I, typename... Args>
		constexpr allocated_values(std::index_sequence<I...>, Allocator& alloc, Args&&... args) :
			alloc_  {alloc},
			address_{std::addressof(std::get<I>(value_).value_)...}
		{
			allocator_traits::construct(alloc_, address_, std::forward<Args>(args)...);
		}

		template <typename U>
		struct inner
		{
			constexpr inner() {}
			constexpr ~inner() {}
			union { U value_; };
		};

		Allocator&              alloc_;
		pointer<T*...>          address_;
		std::tuple<inner<T>...> value_;
	};
}
