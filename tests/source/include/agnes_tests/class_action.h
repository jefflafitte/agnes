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
#include <cstddef>
#include <initializer_list>

namespace agnes_tests
{
	enum class class_action
	{
		none,
		defaultConstructed,
		directConstructed,
		copyConstructed,
		moveConstructed,
		destructed,
		copyAssigned,
		moveAssigned
	};

	class class_actions
	{
	public:
		constexpr class_actions() noexcept : nextAction{actions.begin()} {}

		constexpr class_actions(const class_actions&) noexcept : nextAction{actions.begin()} {}

		constexpr class_actions(class_actions&&) noexcept : nextAction{actions.begin()} {}

		constexpr class_actions& operator=(const class_actions&) noexcept { return *this; }

		constexpr class_actions& operator=(class_actions&&) noexcept { return *this; }

		constexpr ~class_actions()
		{
			if (destructedCopy)
			{
				*destructedCopy = *this;

				destructedCopy->add(class_action::destructed);
			}
		}

		constexpr void add(const class_action action) noexcept
		{
			if (nextAction != actions.end())
			{
				*nextAction++ = action;
			}
		}

		constexpr class_action last() const noexcept
		{
			return (nextAction != actions.begin()) ? *(nextAction - 1) : class_action::none;
		}

		constexpr bool equals(const std::initializer_list<class_action>& otherActions) const
		{
			if (actions.size() < otherActions.size())
			{
				return false;
			}

			auto i = std::size_t{0};

			for (const auto otherAction : otherActions)
			{
				if (actions[i] != otherAction)
				{
					return false;
				}

				++i;
			}

			return (i == actions.size()) || (actions[i] == class_action::none);
		}

		constexpr void copy_on_destruction(class_actions& actions) noexcept { destructedCopy = &actions; }

	private:
		std::array<class_action, 16> actions{};
		decltype(actions)::iterator  nextAction;
		class_actions*               destructedCopy = nullptr;
	};
}
