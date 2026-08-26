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

#include <array>
#include <utility>

#include "agnes_tests/class_action.h"
#include "agnes_tests/defaulted.h"

namespace agnes_tests
{
	template <typename T = int, T Default = static_cast<T>(-1)>
	struct recorded : defaulted<T, Default>
	{
		using base = defaulted<T, Default>;

		constexpr recorded() noexcept
		{ actions.add(class_action::defaultConstructed); }

		constexpr recorded(const T& value) noexcept : base{value}
		{ actions.add(class_action::directConstructed); }

		constexpr recorded(const recorded& other) noexcept : base{other}
		{ actions.add(class_action::copyConstructed); }

		constexpr recorded(recorded&& other) noexcept : base{std::move(other)}
		{ actions.add(class_action::moveConstructed); }

		constexpr ~recorded()
		{
			if (wasDestructed)
			{
				*wasDestructed = true;
			}
		}

		constexpr recorded& operator=(const recorded& other) noexcept
		{
			static_cast<base&>(*this) = other;

			actions.add(class_action::copyAssigned);

			return *this;
		}

		constexpr recorded& operator=(recorded&& other) noexcept
		{
			static_cast<base&>(*this) = std::move(other);

			actions.add(class_action::moveAssigned);

			return *this;
		}

		class_actions actions;
		bool*         wasDestructed = nullptr;
	};
}
