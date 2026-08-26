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

#include <cstddef>
#include <iterator>
#include <utility>

#include <agnes/memory.h>
#include <agnes/pointer.h>
#include <agnes/utility.h>

namespace agnes::internal_
{
	template <typename Allocator>
	class construct_transaction
	{
		using allocator_traits = agnes::allocator_traits<Allocator>;

	public:
		using pointer = typename allocator_traits::pointer;

		constexpr explicit construct_transaction(Allocator& alloc, const pointer& dest) :
			alloc_{alloc},
			first_{dest},
			last_ {dest} {}

		constexpr ~construct_transaction()
		{
			for (; first_ != last_; ++first_)
			{
				allocator_traits::destroy(alloc_, first_);
			}
		}

		constexpr pointer& current() { return last_; }

		constexpr const pointer& complete()
		{
			first_ = last_;

			return last_;
		}

	private:
		Allocator& alloc_;
		pointer    first_;
		pointer    last_;
	};

	template <typename Allocator, typename... Args>
	constexpr typename allocator_traits<Allocator>::pointer transactional_construct(
		Allocator&                                           alloc,
		const typename allocator_traits<Allocator>::pointer& dest,
		typename allocator_traits<Allocator>::size_type      count,
		Args&&...                                            args)
	{
		auto transaction = construct_transaction{alloc, dest};

		for (; count > 0; --count)
		{
			allocator_traits<Allocator>::construct(alloc, transaction.current()++, std::forward<Args>(args)...);
		}

		return transaction.complete();
	}

	template <typename Allocator, typename InputIterator>
	constexpr typename allocator_traits<Allocator>::pointer transactional_construct_impl(
		Allocator&                                           alloc,
		const typename allocator_traits<Allocator>::pointer& dest,
		const InputIterator&                                 first,
		const InputIterator&                                 last)
	{
		auto transaction = construct_transaction{alloc, dest};

		for (auto iter = first; iter != last; ++iter)
		{
			allocator_traits<Allocator>::construct(alloc, transaction.current()++, *iter);
		}

		return transaction.complete();
	}

	template <typename Allocator, std::input_iterator InputIterator>
	constexpr typename allocator_traits<Allocator>::pointer transactional_construct(
		Allocator&                                           alloc,
		const typename allocator_traits<Allocator>::pointer& dest,
		const InputIterator&                                 first,
		const InputIterator&                                 last)
	{
		return transactional_construct_impl(alloc, dest, first, last);
	}

	template <typename Allocator, typename... T>
	constexpr typename allocator_traits<Allocator>::pointer transactional_construct(
		Allocator&                                           alloc,
		const typename allocator_traits<Allocator>::pointer& dest,
		const pointer<T*...>&                                first,
		const pointer<T*...>&                                last)
	{
		return transactional_construct_impl(alloc, dest, first, last);
	}

	template <typename Allocator>
	constexpr typename allocator_traits<Allocator>::pointer transactional_uninitialized_move(
		Allocator&                                           alloc,
		typename allocator_traits<Allocator>::pointer        first,
		const typename allocator_traits<Allocator>::pointer& last,
		const typename allocator_traits<Allocator>::pointer& dest)
	{
		auto transaction = construct_transaction{alloc, dest};

		for (; first != last; ++first)
		{
			allocator_traits<Allocator>::construct(alloc, transaction.current()++, as_rvalue(*first));
		}

		return transaction.complete();
	}

	template <typename Allocator>
	constexpr typename allocator_traits<Allocator>::pointer transactional_uninitialized_fill_n(
		Allocator&                                                   alloc,
		const typename allocator_traits<Allocator>::pointer&         first,
		std::size_t                                                  n,
		const typename allocator_traits<Allocator>::const_reference& values)
	{
		auto transaction = construct_transaction{alloc, first};

		for (; n > 0; --n)
		{
			allocator_traits<Allocator>::construct(alloc, transaction.current()++, values);
		}

		return transaction.complete();
	}
}
