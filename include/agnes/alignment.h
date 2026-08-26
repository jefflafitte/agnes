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

/// @file alignment.h

#pragma once

#include <algorithm>
#include <cstddef>
#include <utility>

#include <agnes/internal_/calc_aligned_unit_count.h>

#include <agnes/cache.h>
#include <agnes/pointer.h>

namespace agnes
{
	/// Returns `size` rounded up to the next multiple of `alignment`.
	constexpr std::size_t align_up(const std::size_t size, const std::size_t alignment) noexcept
	{ return (size + alignment - std::size_t{1}) & ~(alignment - std::size_t{1}); }

	/// Provides a member typedef `type` with the following alignment:
	/// - If `get_cache_align()` is `true`,
	///   then `type` is aligned to the maximum of `alignof(T)...` and `get_cache_line_size()`.
	/// - If `get_cache_align()` is `false`,
	///   then `type` is aligned to the maximum of `alignof(T)...`.
	template <typename... T>
	struct aligned_unit
	{
		static constexpr std::size_t alignment = cache_align_v ?
			std::max({alignof(T)..., cache_line_size_v}) :
			std::max({alignof(T)..., std::size_t{1}});

		struct alignas(alignment) type {};
	};

	/// Helper type that is an alias for `typename aligned_unit<T...>::type`.
	template <typename... T>
	using aligned_unit_t = typename aligned_unit<T...>::type;

	template <typename... T> class aligned_unit_layout;

	template <> class aligned_unit_layout<>;

	template <typename... T>
	class aligned_unit_layout
	{
	public:
		constexpr aligned_unit_layout(const std::size_t n) noexcept
			requires (!avoid_cache_conflicts_v)
		{ initialize(n, {}); }

		constexpr aligned_unit_layout(const std::size_t n, const cache_configuration& config)
		{ initialize(n, config); }

		constexpr std::size_t size() const noexcept { return offset(sizeof...(T)); }

		constexpr std::size_t offset(const std::size_t index) const noexcept
		{ return (index == 0) ? std::size_t{0} : ends_[index - 1]; }

		constexpr pointer<T*...> pointer_to(aligned_unit_t<T...>* const bytes) const noexcept
		{
			return [this, bytes]<std::size_t... I>(std::index_sequence<I...>)
			{
				return pointer<T*...>{reinterpret_cast<T*>(bytes + offset(I))...};
			}(indexes);
		}

	private:
		static constexpr auto indexes = std::index_sequence_for<T...>{};

		constexpr void initialize(std::size_t n, const cache_configuration& config)
		{
			[this, n, &config]<std::size_t... I>(std::index_sequence<I...>)
			{
				const auto calcAlignedUnitCount = internal_::calc_aligned_unit_count<T...>{n, config};

				((ends_[I] = offset(I) + calcAlignedUnitCount(I)), ...);
			}(indexes);
		}

		std::array<std::size_t, sizeof...(T)> ends_;
	};

	template <typename... T>
		requires (sizeof...(T) > 0)
	constexpr std::size_t aligned_unit_layout_size(std::size_t n, const cache_configuration& config)
	{
		return [n, &config]<std::size_t... I>(std::index_sequence<I...>)
		{
			const auto calcAlignedUnitCount = internal_::calc_aligned_unit_count<T...>{n, config};

			return (calcAlignedUnitCount(I) + ... + std::size_t{0});
		}(std::index_sequence_for<T...>{});
	}

	template <typename... T>
		requires (sizeof...(T) > 0) && (!avoid_cache_conflicts_v)
	constexpr std::size_t aligned_unit_layout_size(std::size_t n) noexcept
	{ return aligned_unit_layout_size<T...>(n, {}); }
}
