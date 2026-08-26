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
#include <tuple>

#include <agnes/reference.h>

namespace agnes_tests
{
	template <typename Types, typename Args = Types> class filled_value;

	template <typename... Types, typename... Args>
	class filled_value<std::tuple<Types...>, std::tuple<Args...>>
	{
	public:
		constexpr filled_value(const std::size_t offset = 0) :
			filled_value{std::index_sequence_for<Types...>{}, offset} {}

		constexpr operator std::tuple<Types...>&() { return value_; }

		constexpr operator const std::tuple<Types...>&() const { return value_; }

		template <std::size_t I>
		constexpr friend auto& get(filled_value& value) { return std::get<I>(value.value_); }

		template <std::size_t I>
		constexpr friend const auto& get(const filled_value& value) { return std::get<I>(value.value_); }

	private:
		template <std::size_t... I>
		constexpr filled_value(std::index_sequence<I...>, const std::size_t offset) :
			value_{static_cast<std::tuple_element_t<I, std::tuple<Args...>>>(offset*sizeof...(I) + I)...} {}

		std::tuple<Types...> value_;
	};
}
