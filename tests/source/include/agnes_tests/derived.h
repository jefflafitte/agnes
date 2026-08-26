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

#include "agnes_tests/base.h"

namespace agnes_tests
{
	struct derived final : base
	{
	public:
		constexpr derived() = default;
		constexpr derived(const int& value) noexcept : base{value} {}
		constexpr derived(const derived&) = default;
		constexpr derived(derived&&) = default;
		constexpr derived& operator=(const derived&) = default;
		constexpr derived& operator=(derived&&) = default;
		constexpr ~derived() final = default;
	};
}
