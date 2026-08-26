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

#include <utility>

#include "agnes_tests/recorded.h"

namespace agnes_tests
{
	template <typename T, T Default = static_cast<T>(-1)>
	struct not_noexcept_moveable final : recorded<T, Default>
	{
		using base = recorded<T, Default>;

		constexpr not_noexcept_moveable() = default;
		constexpr not_noexcept_moveable(const T& value) noexcept(noexcept(base{std::declval<const T&>()})) :
			base{value} {}
		constexpr not_noexcept_moveable(const not_noexcept_moveable&) = default;
		constexpr not_noexcept_moveable(not_noexcept_moveable&&) noexcept(false) = default;
		constexpr not_noexcept_moveable& operator=(const not_noexcept_moveable&) = default;
		constexpr not_noexcept_moveable& operator=(not_noexcept_moveable&&) noexcept(false) = default;
	};
}
