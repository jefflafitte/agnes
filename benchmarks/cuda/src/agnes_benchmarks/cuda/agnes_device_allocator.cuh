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

#include <algorithm>
#include <array>
#include <cstddef>
#include <new>
#include <tuple>
#include <utility>

#include <agnes/fields.h>
#include <agnes/pointer.h>

#include <cuda_runtime.h>

namespace agnes_benchmarks::cuda
{
	template <typename... T>
	class agnes_device_allocator
	{
	public:
		using value_type = agnes::fields<T...>;

		agnes_device_allocator() noexcept = default;

		template <typename... U>
		agnes_device_allocator(const agnes_device_allocator<U...>&) noexcept {}

		[[nodiscard]] agnes::pointer<T*...> allocate(const std::size_t n)
		{
			const auto [sizes, totalSize] = calc_sizes(n, indexes);

			auto* bytes = static_cast<aligned_byte*>(nullptr);

			if (cudaMalloc(&bytes, totalSize*sizeof(aligned_byte)) != cudaSuccess)
			{
				throw std::bad_alloc();
			}

			return pointer_to(bytes, sizes, indexes);
		}

		void deallocate(const agnes::pointer<T*...>& p, const std::size_t n) noexcept
		{ cudaFree(reinterpret_cast<aligned_byte*>(get<0>(p))); }

		template <typename... Args>
		void construct(const agnes::pointer<T*...>&, Args&&...) noexcept {}

		void destroy(const agnes::pointer<T*...>&) noexcept {}

		template <typename U>
		bool operator==(const agnes_device_allocator<U>&) const noexcept
		{ return true; }

		template <typename U>
		bool operator!=(const agnes_device_allocator<U>&) const noexcept
		{ return false; }

	private:
		static constexpr auto element_count = sizeof...(T);
		static constexpr auto indexes       = std::index_sequence_for<T...>{};

		using size_array = std::array<std::size_t, element_count>;

		static constexpr auto byte_alignment = std::max({alignof(T)...});

		struct alignas(byte_alignment) aligned_byte { std::byte x; };

		template <std::size_t... I>
		static constexpr std::tuple<size_array, std::size_t> calc_sizes(
			const std::size_t n,
			std::index_sequence<I...>)
		{
			auto result = std::tuple<size_array, std::size_t>{};

			get<1>(result) = ((get<0>(result)[I] = calc_element_size<I, T>(n)) + ...);

			return result;
		}

		template <std::size_t... I>
		static constexpr std::size_t calc_total_size(const std::size_t n, std::index_sequence<I...>)
		{ return (calc_element_size<I, T>(n) + ...); }

		template <std::size_t I, typename T_>
		static constexpr std::size_t calc_element_size(const std::size_t n)
		{ return (n*sizeof(T_) + sizeof(aligned_byte) - std::size_t{1})/sizeof(aligned_byte); }

		template <std::size_t... I>
		static constexpr agnes::pointer<T*...> pointer_to(
			aligned_byte*     bytes,
			const size_array& sizes,
			std::index_sequence<I...>)
		{ return {pointer_to<T, I>(bytes, sizes)...}; }

		template <typename U, std::size_t I>
		static constexpr auto* pointer_to(aligned_byte*& bytes, const size_array& sizes)
		{
			if constexpr (I > 0) { bytes += sizes[I - 1]; }

			return reinterpret_cast<U*>(bytes);
		}
	};
}
