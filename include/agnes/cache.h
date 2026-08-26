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

/// @file cache.h

#pragma once

#include <bit>
#include <cstddef>
#include <stdexcept>
#include <type_traits>
#include <mutex>

#ifndef AGNES_AVOID_CACHE_CONFLICTS
#define AGNES_AVOID_CACHE_CONFLICTS 0
#endif

#ifndef AGNES_CACHE_ALIGN
#define AGNES_CACHE_ALIGN AGNES_AVOID_CACHE_CONFLICTS
#endif

#ifndef AGNES_CACHE_LINE_SIZE
#define AGNES_CACHE_LINE_SIZE 64
#endif

static_assert(
	std::is_same_v<decltype(AGNES_AVOID_CACHE_CONFLICTS), int> &&
		((AGNES_AVOID_CACHE_CONFLICTS == 0) || (AGNES_AVOID_CACHE_CONFLICTS == 1)),
	"AGNES_AVOID_CACHE_CONFLICTS must be 0 or 1.");

static_assert(
	std::is_same_v<decltype(AGNES_CACHE_ALIGN), int> &&
		((AGNES_CACHE_ALIGN == 0) || (AGNES_CACHE_ALIGN == 1)),
	"AGNES_CACHE_ALIGN must be 0 or 1.");

static_assert(
	(AGNES_AVOID_CACHE_CONFLICTS == 0) || (AGNES_CACHE_ALIGN == 1),
	"AGNES_AVOID_CACHE_CONFLICTS requires AGNES_CACHE_ALIGN.");

static_assert(
	std::is_integral_v<decltype(AGNES_CACHE_LINE_SIZE)>	&&
		(AGNES_CACHE_LINE_SIZE > 0) &&
		std::has_single_bit(std::size_t{AGNES_CACHE_LINE_SIZE}),
	"AGNES_CACHE_LINE_SIZE must be a power of 2.");

namespace agnes
{
	inline constexpr bool        cache_align_v           = bool{AGNES_CACHE_ALIGN};
	inline constexpr bool        avoid_cache_conflicts_v = bool{AGNES_AVOID_CACHE_CONFLICTS};
	inline constexpr std::size_t cache_line_size_v       = std::size_t{AGNES_CACHE_LINE_SIZE};

	struct cache_configuration
	{
		std::size_t size          = 0;
		std::size_t associativity = 0;
	};

	namespace internal_
	{
		struct default_allocator_cache_configuration
		{
			inline static default_allocator_cache_configuration& instance() noexcept
			{
				static default_allocator_cache_configuration instance;

				return instance;
			}

			std::mutex          mutex;
			cache_configuration value;
		};

		inline void check_cache_configuration(const cache_configuration& config)
		{
			if (config.size == 0)
			{
				throw std::invalid_argument{"Cache size must be > 0."};
			}

			if (config.associativity == 0)
			{
				throw std::invalid_argument{"Cache associativity must be > 0."};
			}

			if (config.size%(config.associativity*cache_line_size_v) != 0)
			{
				throw std::invalid_argument{"Cache size must be a multiple of "
					"the product of cache associativity and AGNES_CACHE_LINE_SIZE."};
			}
		}
	}

	inline const cache_configuration& get_default_allocator_cache_configuration()
	{
		const auto& instance = internal_::default_allocator_cache_configuration::instance();

		if constexpr (avoid_cache_conflicts_v)
		{
			if (instance.value.size == 0)
			{
				throw std::logic_error{"get_default_allocator_cache_configuration() called "
					"before set_default_allocator_cache_configuration()."};
			}
		}

		return instance.value;
	}

	inline void set_default_allocator_cache_configuration(const cache_configuration& config)
	{
		internal_::check_cache_configuration(config);

		auto& instance = internal_::default_allocator_cache_configuration::instance();

		std::lock_guard<std::mutex> lock(instance.mutex);

		if (instance.value.size != 0)
		{
			throw std::logic_error{"set_default_allocator_cache_configuration() called more than once."};
		}

		instance.value = config;
	}
}
