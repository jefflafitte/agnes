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

/// @file

#pragma once

#include <array>
#include <cstddef>
#include <tuple>
#include <type_traits>

#include <agnes/cache.h>

namespace agnes
{
	constexpr std::size_t align_up(std::size_t, std::size_t) noexcept;

	template <typename... T> struct aligned_unit;
}

namespace agnes::internal_
{
	template <typename...> struct all_same_size : std::false_type {};

	template <typename T> struct all_same_size<T> : std::true_type {};

	template <typename Head, typename... Tail> struct all_same_size<Head, Tail...> :
		std::bool_constant<((sizeof(Head) == sizeof(Tail)) && ...)> {};

	template <typename... T>
	inline constexpr bool all_same_size_v = all_same_size<T...>::value;

	template <typename... T> class calc_aligned_unit_count;

	template <> class calc_aligned_unit_count<>;

	template <typename... T>
	class calc_aligned_unit_count
	{
	public:
		constexpr calc_aligned_unit_count(const std::size_t element_count, const cache_configuration&) noexcept :
			element_count_{element_count} {}

		constexpr std::size_t operator()(const std::size_t element_index) const noexcept
		{ return align_up(element_count_*element_sizes[element_index], aligned_unit_size)/aligned_unit_size; }

	private:
		static constexpr auto element_sizes     = std::array<std::size_t, sizeof...(T)>{sizeof(T)...};
		static constexpr auto aligned_unit_size = sizeof(typename aligned_unit<T...>::type);

		std::size_t element_count_ = 0;
	};

	template <typename... T>
		requires (!avoid_cache_conflicts_v) && all_same_size_v<T...>
	class calc_aligned_unit_count<T...>
	{
	public:
		constexpr calc_aligned_unit_count(const std::size_t element_count, const cache_configuration&) noexcept :
			unit_count_{align_up(element_count*element_size, aligned_unit_size)/aligned_unit_size} {}

		constexpr std::size_t operator()(const std::size_t) const noexcept
		{ return unit_count_; }

	private:
		static constexpr auto element_size      = sizeof(std::tuple_element_t<0, std::tuple<T...>>);
		static constexpr auto aligned_unit_size = sizeof(typename aligned_unit<T...>::type);

		std::size_t unit_count_ = 0;
	};

	template <typename... T>
		requires avoid_cache_conflicts_v && all_same_size_v<T...>
	class calc_aligned_unit_count<T...>
	{
	public:
		constexpr calc_aligned_unit_count(const std::size_t element_count, const cache_configuration& config)
		{ initialize(element_count, config); }

		constexpr std::size_t operator()(const std::size_t element_index) const noexcept
		{ return is_conflicting(element_index + 1) ? unit_count_ + std::size_t{1} : unit_count_; }

	private:
		static constexpr auto element_size      = sizeof(std::tuple_element_t<0, std::tuple<T...>>);
		static constexpr auto aligned_unit_size = sizeof(typename aligned_unit<T...>::type);

		constexpr void initialize(const std::size_t element_count, const cache_configuration& config)
		{
			check_cache_configuration(config);

			const auto byteCount = align_up(element_count*element_size, aligned_unit_size);

			unit_count_         = byteCount/aligned_unit_size;
			is_critical_stride_ = byteCount%(config.size/config.associativity) == 0;
		}

		constexpr bool is_conflicting(const std::size_t element_index) const
		{ return is_critical_stride_ && (element_index < sizeof...(T)); }

		std::size_t unit_count_         = 0;
		bool        is_critical_stride_ = false;
	};
}
