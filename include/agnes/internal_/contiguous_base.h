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
#include <cassert>
#include <cstddef>
#include <initializer_list>
#include <iterator>
#include <limits>
#include <stdexcept>
#include <tuple>
#include <utility>

#include <agnes/internal_/algorithm.h>
#include <agnes/internal_/allocated_values.h>
#include <agnes/internal_/transactional_construct.h>
#include <agnes/internal_/contiguous_iterator.h>
#include <agnes/internal_/contiguous_storage.h>

#include <agnes/fields.h>
#include <agnes/memory.h>
#include <agnes/pointer.h>
#include <agnes/reference.h>
#include <agnes/utility.h>

namespace agnes::internal_
{
	template <typename, typename...> class contiguous_base;

	template <typename Allocator> class contiguous_base<Allocator>;

	template <typename Allocator, typename... T>
	class contiguous_base
	{
		// TODO: Handle std::input_iterator correctly.

		using allocator_traits = agnes::allocator_traits<Allocator>;

	public:
		using value_type      = fields<T...>;
		using allocator_type  = Allocator;
		using pointer         = allocator_traits::pointer;
		using const_pointer   = allocator_traits::const_pointer;
		using reference       = lvalue_reference_to_t<value_type>;
		using const_reference = lvalue_reference_to_t<add_const_t<value_type>>;
		using size_type       = allocator_traits::size_type;
		using difference_type = allocator_traits::difference_type;
		using iterator        = contiguous_iterator<pointer>;
		using const_iterator  = contiguous_iterator<const_pointer>;

		constexpr contiguous_base() noexcept(noexcept(Allocator{})) : contiguous_base{Allocator{}} {};

		constexpr explicit contiguous_base(const Allocator& alloc) noexcept : alloc_{alloc} {}

		constexpr explicit contiguous_base(const size_type n, const Allocator& alloc = Allocator{}) : alloc_{alloc}
		{
			if (n > 0)
			{
				auto s = contiguous_storage{*this, n};

				s.construct_all();

				s.exchange();
			}
		}

		constexpr contiguous_base(
			const size_type  n,
			const T&...      values,
			const Allocator& alloc = Allocator{}) : alloc_{alloc}
		{
			if (n > 0)
			{
				auto s = contiguous_storage{*this, n};

				s.construct_all(values...);

				s.exchange();
			}
		}

		constexpr contiguous_base(
			const size_type                      n,
			const agnes::reference<const T&...>& values,
			const allocator_type&                alloc = Allocator{}) : alloc_{alloc}
		{
			if (n > 0)
			{
				auto s = contiguous_storage{*this, n};

				s.construct_all(values);

				s.exchange();
			}
		}

		template <std::input_iterator InputIterator>
		constexpr contiguous_base(
			const InputIterator& first,
			const InputIterator& last,
			const Allocator&     alloc = Allocator{}) : alloc_{alloc}
		{ initialize(first, last); }

		constexpr contiguous_base(
			const agnes::pointer<T*...>& first,
			const agnes::pointer<T*...>& last,
			const Allocator&             alloc = Allocator{}) : alloc_{alloc}
		{ initialize(first, last); }

		constexpr contiguous_base(
			const agnes::pointer<const T*...>& first,
			const agnes::pointer<const T*...>& last,
			const Allocator&                   alloc = Allocator{}) : alloc_{alloc}
		{ initialize(first, last); }

		constexpr contiguous_base(
			const const_iterator& first,
			const const_iterator& last,
			const Allocator&      alloc = Allocator{}) : alloc_{alloc}
		{ initialize(first, last); }

		constexpr contiguous_base(const contiguous_base& other) :
			contiguous_base{other, allocator_traits::select_on_container_copy_construction(other.alloc_)} {}

		constexpr contiguous_base(contiguous_base&& other) noexcept :
			alloc_{std::move(other.alloc_)},
			first_{other.first_},
			last_ {other.last_},
			cap_  {other.cap_}
		{
			other.first_ = nullptr;
			other.last_  = nullptr;
			other.cap_   = nullptr;
		}

		constexpr contiguous_base(const contiguous_base& other, const Allocator& alloc) :
			contiguous_base{other.first_, other.last_, alloc} {}

		constexpr contiguous_base(contiguous_base&& other, const Allocator& alloc) : alloc_{alloc}
		{
			if (alloc_ == other.alloc_)
			{
				move_storage_from(std::move(other));
			}
			else
			{
				initialize(other.first_, other.last_);
			}
		}

		constexpr contiguous_base(
			const std::initializer_list<std::tuple<T...>> il,
			const Allocator&                              alloc = Allocator{}) :
			contiguous_base{il.begin(), il.end(), alloc} {}

		constexpr ~contiguous_base() { clear(); }

		constexpr contiguous_base& operator=(const contiguous_base& other)
		{
			if (this != &other)
			{
				if constexpr (allocator_traits::propagate_on_container_copy_assignment::value)
				{
					if (alloc_ != other.alloc_)
					{
						clear();
					}

					alloc_ = other.alloc_;
				}

				assign(other.first_, other.last_);
			}

			return *this;
		}

		constexpr contiguous_base& operator=(contiguous_base&& other)
			noexcept(allocator_traits::propagate_on_container_move_assignment::value ||
				allocator_traits::is_always_equal::value)
		{
			if (this != &other)
			{
				if constexpr (allocator_traits::propagate_on_container_move_assignment::value)
				{
					clear();

					alloc_ = std::move(other.alloc_);

					move_storage_from(std::move(other));
				}
				else if constexpr (allocator_traits::is_always_equal::value)
				{
					clear();

					move_storage_from(std::move(other));
				}
				else if (alloc_ == other.alloc_)
				{
					clear();

					move_storage_from(std::move(other));
				}
				else
				{
					move_assign_from(std::move(other));
				}
			}

			return *this;
		}

		constexpr contiguous_base& operator=(const std::initializer_list<std::tuple<T...>> il)
		{ assign(il); return *this; }

		template <std::input_iterator InputIterator>
		constexpr void assign(const InputIterator& first, const InputIterator& last)
		{ assign_impl(first, last); }

		template <typename... U>
			requires (sizeof...(T) == sizeof...(U))
		constexpr void assign(const agnes::pointer<U*...>& first, const agnes::pointer<U*...>& last)
		{ assign_impl(first, last); }

		constexpr void assign(const const_iterator& first, const const_iterator& last)
		{ assign_impl(first, last); }

		constexpr void assign(const size_type n, const T&... values)
		{
			const auto oldSize = size();

			if (n <= oldSize)
			{
				const auto oldLast = last_;
				const auto newLast = first_ + n;

				for (auto p = first_; p != newLast; ++p)
				{
					[&p]<std::size_t... I>(std::index_sequence<I...>, const auto&... values)
					{
						((get<I>(*p) = values), ...);
					}(indexes, values...);
				}

				last_ = newLast;

				destroy_each(last_, oldLast);
			}
			else if (n <= capacity())
			{
				for (auto p = first_; p != last_; ++p)
				{
					[&p]<std::size_t... I>(std::index_sequence<I...>, const auto&... values)
					{
						((get<I>(*p) = values), ...);
					}(indexes, values...);
				}

				auto constructionCount = n - oldSize;

				for (; constructionCount > 0; --constructionCount)
				{
					allocator_traits::construct(alloc_, last_, values...);

					++last_;
				}
			}
			else
			{
				reallocated_emplace_n(n, 0, n, values...);
			}
		}

		constexpr void assign(const size_type n, const agnes::reference<const T&...>& values)
		{
			[this, n, &values]<std::size_t... I>(std::index_sequence<I...>)
			{
				assign(n, get<I>(values)...);
			}(indexes);
		}

		constexpr void assign(const std::initializer_list<std::tuple<T...>> il)
		{ assign_impl(il.begin(), il.end()); }

		constexpr allocator_type get_allocator() const noexcept { return alloc_; }

		[[nodiscard]] constexpr bool empty() const noexcept { return first_ == last_; }

		constexpr size_type size() const noexcept { return static_cast<size_type>(last_ - first_); }

		constexpr size_type max_size() const noexcept
		{
			return std::min(
				allocator_traits::max_size(alloc_),
				static_cast<size_type>(std::numeric_limits<difference_type>::max()));
		}

		constexpr size_type capacity() const noexcept { return static_cast<size_type>(cap_ - first_); }

		constexpr void resize(const size_type sz) { resize_impl(sz); }

		constexpr void resize(const size_type sz, const T&... values) { resize_impl(sz, values...); }

		constexpr void reserve(const size_type n)
		{
			check_length(n);

			if (n > capacity())
			{
				set_capacity(n);
			}
		}

		constexpr void shrink_to_fit()
		{
			const auto newCapacity = size();

			if (capacity() > newCapacity)
			{
				auto s = contiguous_storage{*this, newCapacity};

				s.move_if_nothrow_construct(first_, last_);

				s.exchange();
			}
		}

		constexpr agnes::pointer<T*...> data() noexcept { return first_; }

		constexpr agnes::pointer<const T*...> data() const noexcept { return first_; }

		constexpr void push_back(const T&... values) { emplace_back(values...); }

		constexpr void push_back(T&&... values) { emplace_back(std::move(values)...); }

		constexpr void pop_back()
		{
			destroy_each(last_ - 1, last_);

			--last_;
		}

		constexpr void swap(contiguous_base& other)
			noexcept(allocator_traits::propagate_on_container_swap::value || allocator_traits::is_always_equal::value)
		{
			using std::swap;

			if constexpr (allocator_traits::propagate_on_container_swap::value)
			{
				swap(alloc_, other.alloc_);
			}
			else
			{
				assert(alloc_ == other.alloc_);
			}

			swap(first_, other.first_);
			swap(last_ , other.last_ );
			swap(cap_  , other.cap_  );
		}

		constexpr void clear() noexcept
		{
			if (first_)
			{
				destroy_all();

				allocator_traits::deallocate(alloc_, first_, capacity());

				first_ = nullptr;
				last_  = nullptr;
				cap_   = nullptr;
			}
		}

	protected:
		static constexpr iterator       make_iterator      (const pointer&       p) { return p; }
		static constexpr const_iterator make_const_iterator(const const_pointer& p) { return p; }

		constexpr const pointer& first() const noexcept { return first_; }

		constexpr const pointer& last() const noexcept { return last_; }

		template <typename... Args>
		constexpr void emplace_back(Args&&... args)
		{
			if (last_ != cap_)
			{
				allocator_traits::construct(alloc_, last_, std::forward<Args>(args)...);

				++last_;
			}
			else
			{
				const auto oldSize = size();

				reallocated_emplace_n(calculate_growth(oldSize + 1), oldSize, 1, std::forward<Args>(args)...);
			}
		}

		template <typename... Args>
		constexpr iterator emplace(const const_iterator& position, Args&&... args)
		{
			const auto offset = position - first_;
			auto       where  = first_ + offset;

			if (last_ != cap_)
			{
				if (position == last_)
				{
					allocator_traits::construct(alloc_, last_, std::forward<Args>(args)...);

					++last_;
				}
				else
				{
					const auto values = allocated_values{alloc_, std::forward<Args>(args)...};

					move_tail_backward(where, last_ - where);

					*where = as_rvalue(values.get());
				}
			}
			else
			{
				where = reallocated_emplace_n(calculate_growth(size() + 1), offset, 1, std::forward<Args>(args)...);
			}

			return where;
		}

		constexpr iterator insert(const const_iterator& position, const T&... values)
		{ return emplace(position, values...); }

		constexpr iterator insert(const const_iterator& position, T&&... values)
		{ return emplace(position, std::move(values)...); }

		constexpr iterator insert(const const_iterator& position, const size_type n, const T&... values)
		{
			const auto offset = position - first_;
			auto       where  = first_ + offset;

			if (n > 0)
			{
				if (n == 1)
				{
					emplace(position, values...);
				}
				else if (n <= (cap_ - last_))
				{
					const auto allocated = allocated_values{alloc_, values...};
					const auto valueRef  = allocated.get();

					const auto oldLast   = last_;
					const auto moveCount = oldLast - position;

					if (n <= moveCount)
					{
						move_tail_backward(where, n);

						internal_::fill_n(where, n, valueRef);
					}
					else
					{
						last_ = internal_::transactional_uninitialized_fill_n(
							alloc_,
							oldLast,
							n - moveCount,
							valueRef);

						last_ = internal_::transactional_uninitialized_move(alloc_, where, oldLast, last_);

						internal_::fill(where, oldLast, valueRef);
					}
				}
				else
				{
					where = reallocated_emplace_n(calculate_growth(size() + n), offset, n, values...);
				}
			}

			return where;
		}

		template <std::input_iterator InputIterator>
		constexpr iterator insert(
			const const_iterator& position,
			const InputIterator&  first,
			const InputIterator&  last)
		{ return insert_impl(position, first, last); }

		template <typename... U>
		constexpr iterator insert(
			const const_iterator&        position,
			const agnes::pointer<U*...>& first,
			const agnes::pointer<U*...>& last)
		{ return insert_impl(position, first, last); }

		constexpr iterator insert(
			const const_iterator& position,
			const const_iterator& first,
			const const_iterator& last)
		{ return insert_impl(position, first, last); }

		constexpr iterator insert(
			const const_iterator&                         position,
			const std::initializer_list<std::tuple<T...>> il)
		{ return insert_impl(position, il.begin(), il.end()); }

		constexpr iterator erase(const const_iterator& position)
		{
			const auto where = first_ + (position - first_);

			internal_::move(where + 1, last_, where);

			allocator_traits::destroy(alloc_, last_ - 1);

			--last_;

			return where;
		}

		constexpr iterator erase(const const_iterator& first, const const_iterator& last)
		{
			const auto where = first_ + (first - first_);

			if (first != last)
			{
				const auto newLast = internal_::move(where + (last - first), last_, where);

				for (auto p = newLast; p != last_; ++p)
				{
					allocator_traits::destroy(alloc_, p);
				}

				last_ = newLast;
			}

			return where;
		}

		constexpr void check_range(const size_type n) const
		{
			if (n >= size())
			{
				throw std::out_of_range{"invalid subscript"};
			}
		}

	private:
		using allocated_values   = internal_::allocated_values<Allocator, T...>;
		using contiguous_storage = internal_::contiguous_storage<Allocator, T...>;

		friend contiguous_storage;

		static constexpr auto indexes = std::index_sequence_for<T...>{};

		constexpr void check_length(const size_type n) const
		{
			if (n > max_size())
			{
				throw std::length_error{"contiguous storage too long"};
			}
		}

		constexpr size_type calculate_growth(const size_type newSize) const
		{
			check_length(newSize);

			const auto oldCapacity = capacity();

			assert(newSize > oldCapacity);

			const auto maxSize = max_size();

			assert(newSize <= maxSize);

			const auto growth = std::max(oldCapacity/size_type{2}, newSize - oldCapacity);

			return (oldCapacity <= (maxSize - growth)) ? (oldCapacity + growth) : maxSize;
		}

		template <typename InputIterator>
		constexpr void initialize(const InputIterator& first, const InputIterator& last)
		{
			const auto n = static_cast<std::size_t>(last - first);

			if (n > 0)
			{
				auto s = contiguous_storage{*this, n};

				s.copy_construct_all(first, last);

				s.exchange();
			}
		}

		constexpr void move_storage_from(contiguous_base&& other)
		{
			first_ = other.first_;
			last_  = other.last_;
			cap_   = other.cap_;

			other.first_ = nullptr;
			other.last_  = nullptr;
			other.cap_   = nullptr;
		}

		constexpr void move_assign_from(contiguous_base&& other)
		{
			const auto count   = other.size();
			const auto oldSize = size();

			if (count <= oldSize)
			{
				const auto oldLast = last_;

				last_ = internal_::move(other.first_, other.last_, first_);

				destroy_each(last_, oldLast);
			}
			else if (count <= capacity())
			{
				auto middle = other.first_;

				middle += oldSize;

				internal_::move(other.first_, middle, first_);

				for (; middle != other.last_; ++middle)
				{
					allocator_traits::construct(alloc_, last_, as_rvalue(*middle));

					++last_;
				}
			}
			else
			{
				auto s = contiguous_storage{*this, count};

				s.move_construct_all(other.first_, other.last_);

				s.exchange();
			}
		}

		template <typename InputIterator>
		constexpr void assign_impl(const InputIterator& first, const InputIterator& last)
		{
			const auto count   = static_cast<size_type>(last - first);
			const auto oldSize = size();

			if (count <= oldSize)
			{
				const auto oldLast = last_;

				last_ = std::copy(first, last, first_);

				destroy_each(last_, oldLast);
			}
			else if (count <= capacity())
			{
				auto middle = first;

				middle += oldSize;

				std::copy(first, middle, first_);

				for (; middle != last; ++middle)
				{
					allocator_traits::construct(alloc_, last_, *middle);

					++last_;
				}
			}
			else
			{
				auto s = contiguous_storage{*this, count};

				s.copy_construct_all(first, last);

				s.exchange();
			}
		}

		template <typename... U>
		constexpr void resize_impl(const size_type sz, const U&... values)
		{
			check_length(sz);

			const auto oldSize = size();

			if (sz <= oldSize)
			{
				const auto oldLast = last_;

				last_ = first_ + sz;

				destroy_each(last_, oldLast);
			}
			else if (sz <= capacity())
			{
				auto constructionCount = sz - oldSize;

				for (; constructionCount > 0; --constructionCount)
				{
					allocator_traits::construct(alloc_, last_, values...);

					++last_;
				}
			}
			else
			{
				reallocated_emplace_n(sz, oldSize, sz - oldSize, values...);
			}
		}

		template <typename InputIterator>
		constexpr iterator insert_impl(
			const const_iterator& position,
			const InputIterator&  first,
			const InputIterator&  last)
		{
			const auto offset = position - first_;
			auto       where  = first_ + offset;
			const auto count  = static_cast<size_type>(last - first);

			if (count > 0)
			{
				if (count == 1)
				{
					emplace(position, *first);
				}
				else if (count <= (cap_ - last_))
				{
					const auto oldLast   = last_;
					const auto moveCount = oldLast - position;

					if (count <= moveCount)
					{
						move_tail_backward(where, count);

						std::copy(first, last, where);
					}
					else
					{
						auto split = first;

						split += moveCount;

						last_ = internal_::transactional_construct(alloc_, oldLast, split, last);

						last_ = internal_::transactional_uninitialized_move(alloc_, where, oldLast, last_);

						std::copy(first, split, where);
					}
				}
				else
				{
					auto s = contiguous_storage{*this, calculate_growth(size() + count)};

					s.construct_at(offset, first, last);

					s.move_if_nothrow_construct(first_, last_);

					s.exchange();

					where = first_ + offset;
				}
			}

			return where;
		}

		constexpr void set_capacity(const size_type sz)
		{
			assert((sz == size()) || (sz > capacity()));

			auto s = contiguous_storage{*this, sz};

			s.move_if_nothrow_construct(first_, last_);

			s.exchange();
		}

		constexpr void move_tail_backward(const pointer& first, const size_type count)
		{
			assert(count <= size());
			assert((last_ - first) >= count);
			assert((size() + count) <= capacity());

			const auto split   = last_ - count;
			const auto oldLast = last_;

			last_ = internal_::transactional_uninitialized_move(alloc_, split, last_, last_);

			internal_::move_backward(first, split, oldLast);
		}

		template <typename... Args>
		constexpr pointer reallocated_emplace_n(
			const size_type       newSize,
			const difference_type offset,
			const size_type       n,
			Args&&...             args)
		{
			assert(newSize > size());
			assert(newSize >= size() + n);
			assert(offset < newSize);
			assert(offset + n <= newSize);

			auto s = contiguous_storage{*this, newSize};

			s.construct_at(offset, n, std::forward<Args>(args)...);

			s.move_if_nothrow_construct(first_, last_);

			s.exchange();

			return first_ + offset;
		}

		constexpr void destroy_each(pointer first, const pointer& last)
		{
			for (; first != last; ++first)
			{
				allocator_traits::destroy(alloc_, first);
			}
		}

		constexpr void destroy_all() { destroy_each(first_, last_); }

		Allocator alloc_;
		pointer   first_;
		pointer   last_;
		pointer   cap_;
	};

	template <std::size_t... I, typename Allocator, typename... T>
	constexpr pointer_subset_t<typename contiguous_base<Allocator, T...>::pointer, I...> data(
		contiguous_base<Allocator, T...>& v) noexcept
	{ return subset<I...>(v.data()); }

	template <std::size_t... I, typename Allocator, typename... T>
	constexpr pointer_subset_t<typename contiguous_base<Allocator, T...>::const_pointer, I...> data(
		const contiguous_base<Allocator, T...>& v) noexcept
	{ return subset<I...>(v.data()); }

	template <typename...> struct make_contiguous_base_impl;

	template <typename... Head, typename Next, typename... Tail>
	struct make_contiguous_base_impl<std::tuple<Head...>, Next, Tail...> :
		make_contiguous_base_impl<std::tuple<Head..., Next>, Tail...> {};

	template <typename... T>
	struct make_contiguous_base_impl<std::tuple<T...>> :
		std::type_identity<contiguous_base<allocator<T...>, T...>> {};

	template <typename... T, typename Allocator>
	struct make_contiguous_base_impl<std::tuple<T...>, allocate_with<Allocator>> :
		std::type_identity<contiguous_base<Allocator, T...>> {};

	template <typename Head, typename... Tail>
	struct make_contiguous_base : make_contiguous_base_impl<std::tuple<Head>, Tail...> {};

	template <typename... T>
	using make_contiguous_base_t = make_contiguous_base<T...>::type;
}
