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

namespace agnes_tests
{
	template <typename T = int, T Default = static_cast<T>(-1)>
	struct defaulted
	{
		using value_type = T;

		constexpr defaulted() = default;
		constexpr defaulted(const T& value) noexcept(noexcept(T{std::declval<const T&>()})) : value{value} {}
		constexpr defaulted(T&& value) noexcept(noexcept(T{std::declval<T&&>()})) : value{std::move(value)} {}
		constexpr defaulted(const defaulted&) = default;
		constexpr defaulted(defaulted&&) = default;
		constexpr defaulted& operator=(const defaulted&) = default;
		constexpr defaulted& operator=(defaulted&&) = default;

		T value = Default;
	};

	template <typename T, T LhsDefault, T RhsDefault>
	constexpr bool operator==(const defaulted<T, LhsDefault>& lhs, const defaulted<T, RhsDefault>& rhs) noexcept
	{
		return lhs.value == rhs.value;
	}

	template <typename T, T LhsDefault, T RhsDefault>
	constexpr auto operator<=>(const defaulted<T, LhsDefault>& lhs, const defaulted<T, RhsDefault>& rhs) noexcept
	{
		return lhs.value <=> rhs.value;
	}

	template <typename T, T Default>
	constexpr bool operator==(const defaulted<T, Default>& lhs, const T& rhs) noexcept
	{
		return lhs.value == rhs;
	}
}
