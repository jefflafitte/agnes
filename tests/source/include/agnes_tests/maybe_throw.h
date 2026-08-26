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

namespace agnes_tests
{
	struct maybe_throw_copy_assignable
	{
		constexpr maybe_throw_copy_assignable& operator=(const maybe_throw_copy_assignable&) noexcept(false) = default;
		constexpr maybe_throw_copy_assignable& operator=(const int&) noexcept(false) { return *this; }
		constexpr maybe_throw_copy_assignable& operator=(maybe_throw_copy_assignable&&) = delete;
	};

	struct maybe_throw_move_assignable
	{
		constexpr maybe_throw_move_assignable& operator=(const maybe_throw_move_assignable&) = delete;
		constexpr maybe_throw_move_assignable& operator=(maybe_throw_move_assignable&&) noexcept(false) = default;
		constexpr maybe_throw_move_assignable& operator=(const int&&) noexcept(false) { return *this; }
	};
}
