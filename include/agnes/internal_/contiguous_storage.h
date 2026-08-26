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
#include <cstddef>
#include <iterator>
#include <utility>

#include <agnes/memory.h>
#include <agnes/reference.h>
#include <agnes/utility.h>

namespace agnes::internal_
{
	template <typename, typename...> class contiguous_base;

	template <typename Allocator, typename... T>
	class contiguous_storage
	{
		using allocator_traits = agnes::allocator_traits<Allocator>;
		using contiguous_base  = agnes::internal_::contiguous_base<Allocator, T...>;

	public:
		using pointer       = typename allocator_traits::pointer;
		using const_pointer = typename contiguous_base::const_pointer;

		constexpr explicit contiguous_storage(contiguous_base& v, const std::size_t n) :
			contiguous_{v},
			frontFirst_{(assert(n > 0), allocator_traits::allocate(contiguous_.alloc_, n))},
			frontLast_ {frontFirst_},
			backFirst_ {frontFirst_},
			backLast_  {frontFirst_},
			cap_       {frontFirst_ + n} {}

		constexpr ~contiguous_storage()
		{
			if (frontFirst_)
			{
				destroy();
			}
		}

		template <typename... Args>
		constexpr void construct_all(Args&... args)
		{
			assert(frontFirst_ != nullptr);
			assert(frontLast_  == frontFirst_);
			assert(backFirst_  == frontFirst_);
			assert(backLast_   == frontFirst_);
			assert(cap_        != nullptr);

			for(; backLast_ != cap_; ++backLast_)
			{
				allocator_traits::construct(contiguous_.alloc_, backLast_, args...);
			}
		}

		template <typename InputIterator>
		constexpr void copy_construct_all(const InputIterator& first, const InputIterator& last)
		{
			assert(frontFirst_ != nullptr);
			assert(frontLast_  == frontFirst_);
			assert(backFirst_  == frontFirst_);
			assert(backLast_   == frontFirst_);
			assert(cap_        != nullptr);

			for (auto dest = first; dest != last; ++dest)
			{
				allocator_traits::construct(contiguous_.alloc_, backLast_++, *dest);
			}

			assert(backLast_ == cap_);
		}

		template <typename InputIterator>
		constexpr void move_construct_all(const InputIterator& first, const InputIterator& last)
		{
			assert(frontFirst_ != nullptr);
			assert(frontLast_  == frontFirst_);
			assert(backFirst_  == frontFirst_);
			assert(backLast_   == frontFirst_);
			assert(cap_        != nullptr);

			for (auto dest = first; dest != last; ++dest)
			{
				allocator_traits::construct(contiguous_.alloc_, backLast_++, as_rvalue(*dest));
			}

			assert(backLast_ == cap_);
		}

		template <typename... Args>
		constexpr void construct_at(const std::size_t offset, const std::size_t count, Args&&... args)
		{
			assert(frontFirst_ != nullptr);
			assert(frontLast_  == frontFirst_);
			assert(backFirst_  == frontFirst_);
			assert(backLast_   == frontFirst_);
			assert(cap_        != nullptr);

			assert(count > 0);
			assert((cap_ - (backFirst_ + offset)) >= count);

			backFirst_ += offset;

			const auto newBackLast = backFirst_ + count;

			for (backLast_ = backFirst_; backLast_ != newBackLast; ++backLast_)
			{
				allocator_traits::construct(contiguous_.alloc_, backLast_, std::forward<Args>(args)...);
			}
		}

		template <std::input_iterator InputIterator>
		constexpr void construct_at(const std::size_t offset, const InputIterator& first, const InputIterator& last)
		{
			return construct_at_impl(offset, first, last);
		}

		constexpr void construct_at(const std::size_t offset, const const_pointer& first, const const_pointer& last)
		{
			return construct_at_impl(offset, first, last);
		}

		template <typename InputIterator>
		constexpr void move_if_nothrow_construct(const InputIterator& first, const InputIterator& last)
		{
			assert(frontFirst_ != nullptr);
			assert(frontLast_  != nullptr);
			assert(backFirst_  != nullptr);
			assert(backLast_   != nullptr);
			assert(cap_        != nullptr);

			[this, &first, &last]<std::size_t... I>(std::index_sequence<I...>)
			{
				auto iter = first;
				for (; (iter != last) && (frontLast_ != backFirst_); ++iter)
				{
					allocator_traits::construct(
						contiguous_.alloc_,
						frontLast_++,
						std::move_if_noexcept(get<I>(*iter))...);
				}
				for (; iter != last; ++iter)
				{
					allocator_traits::construct(
						contiguous_.alloc_,
						backLast_++,
						std::move_if_noexcept(get<I>(*iter))...);
				}
			}(std::index_sequence_for<T...>{});
		}

		constexpr void exchange()
		{
			assert(frontLast_ == backFirst_);

			using std::swap;

			swap(frontFirst_, contiguous_.first_);
			swap(backLast_  , contiguous_.last_ );
			swap(cap_       , contiguous_.cap_  );

			frontLast_ = frontFirst_;
			backFirst_ = frontFirst_;
		}

	private:
		constexpr std::size_t capacity() { return cap_ - frontFirst_; }

		template <typename InputIterator>
		constexpr void construct_at_impl(
			const std::size_t    offset,
			const InputIterator& first,
			const InputIterator& last)
		{
			assert(frontFirst_ != nullptr);
			assert(frontLast_  == frontFirst_);
			assert(backFirst_  == frontFirst_);
			assert(backLast_   == frontFirst_);
			assert(cap_        != nullptr);

			const auto count = static_cast<std::size_t>(last - first);

			assert(count > 0);
			assert((cap_ - backFirst_) >= count);

			backFirst_ += offset;

			const auto newBackLast = backFirst_ + count;

			auto dest = first;

			for (backLast_ = backFirst_; backLast_ != newBackLast; ++backLast_)
			{
				allocator_traits::construct(contiguous_.alloc_, backLast_, *dest);

				++dest;
			}
		}

		constexpr void destroy()
		{
			assert(frontFirst_ != nullptr);
			assert(frontLast_  != nullptr);
			assert(cap_        != nullptr);
			assert(backFirst_  != nullptr);
			assert(backLast_   != nullptr);

			for (auto atPtr = frontFirst_; atPtr != frontLast_; ++atPtr)
			{
				allocator_traits::destroy(contiguous_.alloc_, atPtr);
			}

			for (auto atPtr = backFirst_; atPtr != backLast_; ++atPtr)
			{
				allocator_traits::destroy(contiguous_.alloc_, atPtr);
			}

			allocator_traits::deallocate(contiguous_.alloc_, frontFirst_, capacity());
		}

		contiguous_base& contiguous_;
		pointer          frontFirst_ = nullptr;
		pointer          frontLast_  = nullptr;
		pointer          backFirst_  = nullptr;
		pointer          backLast_   = nullptr;
		pointer          cap_        = nullptr;
	};
}
