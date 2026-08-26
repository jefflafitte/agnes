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

#include <stdexcept>
#include <utility>

#include "agnes_tests/recorded.h"

namespace agnes_tests
{
	template <typename T, T Default = static_cast<T>(-1)>
	struct throw_on_copy final : recorded<T, Default>
	{
		using base = recorded<T, Default>;

		constexpr throw_on_copy() = default;
		constexpr throw_on_copy(const T& value) noexcept(noexcept(base{std::declval<const T&>()})) : base{value} {}
		constexpr throw_on_copy(const throw_on_copy& other) : base{other} { try_throw(other); }
		constexpr throw_on_copy(throw_on_copy&&) = delete;
		constexpr throw_on_copy& operator=(const throw_on_copy& other)
		{
			*static_cast<base*>(this) = other;

			try_throw(other);

			return *this;
		}
		constexpr throw_on_copy& operator=(throw_on_copy&&) = delete;

		constexpr void try_throw(const throw_on_copy& other) const
		{
			if (other.throwWhenCopied)
			{
				throw std::runtime_error{""};
			}
		}

		bool throwWhenCopied = false;
	};
}
