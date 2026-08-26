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

#include <cassert>
#include <compare>
#include <cstddef>
#include <type_traits>
#include <utility>

#include <agnes/internal_/pointer_tuple.h>

namespace agnes { template <typename...> class pointer; }

namespace agnes::internal_
{
	template <typename...> class pointer_impl;

	template <> class pointer_impl<>;

	template <typename... T>
	class pointer_impl
	{
	private:
		friend class agnes::pointer<T*...>;

		constexpr explicit pointer_impl(T* const... ptrs) noexcept : ptrs_{ptrs...} { assert(all_or_none_null()); }

		constexpr explicit pointer_impl(const pointer_impl& other, const std::ptrdiff_t i) noexcept :
			ptrs_{other.ptrs_}, i_{other.i_} { i_ += i; }

		template <typename... U>
			requires (sizeof...(T) == sizeof...(U)) && (std::is_constructible_v<T*, U* const&> && ...)
		constexpr pointer_impl(const pointer_impl<U...>& other) noexcept : ptrs_{other.ptrs_}, i_{other.i_} {}

		constexpr bool is_null() const { return get<0>(ptrs_) == nullptr; }

		constexpr void increment() noexcept { ++i_; }
		constexpr void decrement() noexcept { --i_; }

		constexpr void advance(const std::ptrdiff_t n) noexcept { i_ += n; }

		constexpr std::ptrdiff_t subtract(const pointer_impl& other) const noexcept
		{ assert(three_way_consistent(other)); return get<0>(*this) - get<0>(other); }

		template <typename... U>
			requires (sizeof...(T) == sizeof...(U))
		constexpr bool equal_to(const pointer_impl<U...>& other) const noexcept
		{ assert(equal_consistent(other)); return get<0>(*this) == get<0>(other); }

		template <typename... U>
			requires (sizeof...(T) == sizeof...(U))
		constexpr auto compare_three_way(const pointer_impl<U...>& other) const noexcept
		{ assert(three_way_consistent(other)); return get<0>(*this) <=> get<0>(other); }

		template <std::size_t I>
		friend constexpr auto* get(const pointer_impl& p) noexcept
		{ return get<I>(p.ptrs_) + p.i_; }

		constexpr void swap(pointer_impl& other) noexcept
		{ ptrs_.swap(other.ptrs_); std::swap(i_, other.i_); }

		template <typename... U> friend class pointer_impl;

		static constexpr auto indexes = std::index_sequence_for<T...>{};

		constexpr bool all_or_none_null() const
		{
			return [this]<std::size_t Zero, std::size_t... I>(std::index_sequence<Zero, I...>)
			{
				return (get<0>(ptrs_) == nullptr) ?
					((get<I>(ptrs_) == nullptr) && ...) :
					((get<I>(ptrs_) != nullptr) && ...);
			}(indexes);
		}

		template <typename... U>
		constexpr bool equal_consistent(const pointer_impl<U...>& other) const
		{
			return [this, &other]<std::size_t Zero, std::size_t... I>(std::index_sequence<Zero, I...>)
			{
				return (get<0>(ptrs_) == get<0>(other.ptrs_)) ?
					((get<I>(ptrs_) == get<I>(other.ptrs_)) && ...) :
					((get<I>(ptrs_) != get<I>(other.ptrs_)) && ...);
			}(indexes);
		}

		template <typename... U>
		constexpr bool three_way_consistent(const pointer_impl<U...>& other) const
		{
			return [this, &other]<std::size_t Zero, std::size_t... I>(std::index_sequence<Zero, I...>)
			{
				return (get<0>(ptrs_) < get<0>(other.ptrs_)) ? ((get<I>(ptrs_) < get<I>(other.ptrs_)) && ...) :
					((get<0>(ptrs_) == get<0>(other.ptrs_)) ? ((get<I>(ptrs_) == get<I>(other.ptrs_)) && ...) :
					((get<I>(ptrs_) < get<I>(other.ptrs_)) && ...));
			}(indexes);
		}

		pointer_tuple<T...> ptrs_;
		std::ptrdiff_t      i_ = 0;
	};
}
