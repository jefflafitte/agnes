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

#include <agnes_tests/recorded.h>

namespace agnes_tests
{
	struct base : recorded<int>
	{
		constexpr base() = default;
		constexpr base(const int& value) noexcept : recorded<int>{value} {}
		constexpr base(const base&) = default;
		constexpr base(base&&) = default;
		constexpr base& operator=(const base&) = default;
		constexpr base& operator=(base&&) = default;
		constexpr virtual ~base() = default;
	};
}
