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
#include <cassert>
#include <compare>
#include <concepts>
#include <cstddef>
#include <memory>
#include <tuple>
#include <type_traits>
#include <utility>

#ifndef AGNES_POINTER_NO_CONST_EVAL
#define AGNES_POINTER_NO_CONST_EVAL 0
#endif

#ifndef AGNES_POINTER_NON_CONST_EVAL_OPTIMIZATION
#if defined(__GNUC__) && !defined(__clang__)
#define AGNES_POINTER_NON_CONST_EVAL_OPTIMIZATION 0
#else
#define AGNES_POINTER_NON_CONST_EVAL_OPTIMIZATION 1
#endif
#endif

namespace agnes::internal_
{
	template <typename... T> concept all_same = (std::same_as<std::tuple_element_t<0, std::tuple<T...>>, T> && ...);

	template <typename...> class pointer_tuple;

	template <> class pointer_tuple<>;

#if AGNES_POINTER_NO_CONST_EVAL

	template <typename... T>
		requires (!all_same<T...>)
	class pointer_tuple<T...>
	{
	public:
		constexpr explicit pointer_tuple(T* const... ptrs) noexcept :
			ptrs_{const_cast<std::remove_const_t<T>*>(ptrs)...} {}

		template <typename... U>
			requires (sizeof...(T) == sizeof...(U)) && (std::is_constructible_v<T*, U* const&> && ...)
		constexpr explicit pointer_tuple(const pointer_tuple<U...>& other) noexcept : pointer_tuple{indexes, other} {}

		template <std::size_t I> friend constexpr auto* get(const pointer_tuple& p) noexcept
		{ return static_cast<std::tuple_element_t<I, std::tuple<T*...>>>(p.ptrs_[I]); }

		constexpr void swap(pointer_tuple& other) noexcept { ptrs_.swap(other.ptrs_); }

	private:
		static constexpr auto indexes = std::index_sequence_for<T...>{};

		template <std::size_t... I, typename... U>
		constexpr pointer_tuple(std::index_sequence<I...>, const pointer_tuple<U...>& other) noexcept :
			ptrs_{const_cast<std::remove_const_t<T>*>(get<I>(other))...} {}

		std::array<void*, sizeof...(T)> ptrs_;
	};

#elif AGNES_POINTER_NON_CONST_EVAL_OPTIMIZATION

	template <typename T0, typename... T>
		requires (!all_same<T0, T...>)
	class pointer_tuple<T0, T...>
	{
	public:
		constexpr pointer_tuple(T0* p0, T* const... ptrs) noexcept : ptr0_{p0}
		{
			if (aptrs_active_)
			{
				std::construct_at(&aptrs_);

				[this, ptrs...]<std::size_t... I>(std::index_sequence<I...>)
				{
					((aptrs_[I] = const_cast<std::remove_const_t<T>*>(ptrs)), ...);
				}(indexes);
			}
			else
			{
				std::construct_at(&tptrs_, ptrs...);
			}
		}

		constexpr pointer_tuple(const pointer_tuple& other) noexcept { assign(other); }

		template <typename U0, typename... U>
			requires (sizeof...(T) == sizeof...(U)) &&
				std::is_constructible_v<T0*, U0* const&> &&
				(std::is_constructible_v<T*, U* const&> && ...)
		constexpr pointer_tuple(const pointer_tuple<U0, U...>& other) noexcept { assign(other); }

		constexpr pointer_tuple& operator=(const pointer_tuple& other) noexcept
		{ assign(other); return *this; }

		template <std::size_t I> friend constexpr auto* get(const pointer_tuple& p) noexcept
		{
			if constexpr (I == 0)
			{
				return p.ptr0_;
			}
			else if (p.aptrs_active_)
			{
				return static_cast<std::tuple_element_t<I - 1, std::tuple<T*...>>>(p.aptrs_[I - 1]);
			}
			else
			{
				return std::get<I - 1>(p.tptrs_);
			}
		}

		constexpr void swap(pointer_tuple& other) noexcept
		{
			std::swap(ptr0_, other.ptr0_);

			if (aptrs_active_)
			{
				assert(other.aptrs_active_);

				aptrs_.swap(other.aptrs_);
			}
			else
			{
				assert(!other.aptrs_active_);

				tptrs_.swap(other.tptrs_);
			}
		}

	private:
		static constexpr auto indexes = std::index_sequence_for<T...>{};

		template <typename... U>
		constexpr void assign(const pointer_tuple<U...>& other)
		{
			ptr0_ = get<0>(other);

			if (aptrs_active_)
			{
				std::construct_at(&aptrs_);

				[this, &other]<std::size_t... I>(std::index_sequence<I...>)
				{
					((aptrs_[I] = const_cast<std::remove_const_t<T>*>(get<I + 1>(other))), ...);
				}(indexes);
			}
			else
			{
				[this, &other]<std::size_t... I>(std::index_sequence<I...>)
				{
					std::construct_at(&tptrs_, get<I + 1>(other)...);
				}(indexes);
			}
		}

		T0*                                 ptr0_ = nullptr;
		union
		{
			std::array<void*, sizeof...(T)> aptrs_;
			std::tuple<T*...>               tptrs_;
		};
		const bool                          aptrs_active_ = !std::is_constant_evaluated();
	};

#else

	template <typename T0, typename... T>
		requires (!all_same<T0, T...>)
	class pointer_tuple<T0, T...>
	{
	public:
		constexpr pointer_tuple(T0* p0, T* const... ptrs) noexcept : ptr0_{p0}, ptrs_{ptrs...} {}

		template <typename U0, typename... U>
			requires (sizeof...(T) == sizeof...(U)) &&
				std::is_constructible_v<T0*, U0* const&> &&
				(std::is_constructible_v<T*, U* const&> && ...)
		constexpr pointer_tuple(const pointer_tuple<U0, U...>& other) noexcept : pointer_tuple{indexes, other} {}

		template <std::size_t I> friend constexpr auto* get(const pointer_tuple& p) noexcept
		{
			if constexpr (I == 0)
			{
				return p.ptr0_;
			}
			else
			{
				return std::get<I - 1>(p.ptrs_);
			}
		}

		constexpr void swap(pointer_tuple& other) noexcept
		{ std::swap(ptr0_, other.ptr0_); ptrs_.swap(other.ptrs_); }

	private:
		static constexpr auto indexes = std::index_sequence_for<T...>{};

		template <std::size_t... I, typename... U>
		constexpr pointer_tuple(std::index_sequence<I...>, const pointer_tuple<U...>& other) noexcept :
			ptr0_{get<0>(other)}, ptrs_{get<I + 1>(other)...} {}

		T0*               ptr0_ = nullptr;
		std::tuple<T*...> ptrs_;
	};

#endif

	template <typename... T>
		requires all_same<T...>
	class pointer_tuple<T...>
	{
	public:
		constexpr pointer_tuple(T* const... ptrs) noexcept : ptrs_{ptrs...} {}

		template <typename... U>
			requires (sizeof...(T) == sizeof...(U)) && (std::is_constructible_v<T*, U* const&> && ...)
		constexpr pointer_tuple(const pointer_tuple<U...>& other) noexcept : pointer_tuple{indexes, other} {}

		template <std::size_t I>
		friend constexpr auto* get(const pointer_tuple& p) noexcept
		{ return p.ptrs_[I]; }

		constexpr void swap(pointer_tuple& other) noexcept { ptrs_.swap(other.ptrs_); }

	private:
		static constexpr auto indexes = std::index_sequence_for<T...>{};

		template <std::size_t... I, typename... U>
		constexpr pointer_tuple(std::index_sequence<I...>, const pointer_tuple<U...>& other) noexcept :
			ptrs_{get<I>(other)...} {}

		std::array<std::tuple_element_t<0, std::tuple<T...>>*, sizeof...(T)> ptrs_;
	};
}
