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

#include <compare>
#include <cstddef>
#include <iterator>
#include <tuple>
#include <type_traits>

#include <agnes/memory.h>
#include <agnes/pointer.h>
#include <agnes/reference.h>
#include <agnes/type_traits.h>

namespace agnes::internal_
{
	template <typename, typename...> class contiguous_base;
	template <typename, typename...> class contiguous_storage;

	template <typename Pointer> class contiguous_iterator;

	template <typename, std::size_t...> struct contiguous_iterator_subset;

	template <typename ContiguousIterator> struct contiguous_iterator_subset<ContiguousIterator>;

	template <typename Pointer, std::size_t... I>
	struct contiguous_iterator_subset<contiguous_iterator<Pointer>, I...> :
		std::type_identity<contiguous_iterator<pointer_subset_t<Pointer, I...>>> {};

	template <typename ContiguousIterator, std::size_t... I>
	using contiguous_iterator_subset_t = typename contiguous_iterator_subset<ContiguousIterator, I...>::type;

	template <typename Pointer> 
	class contiguous_iterator
	{
		// TODO: Use std::zip_view techniques to model iterator concepts.

		using pointer_traits = agnes::pointer_traits<Pointer>;

	public:
		using difference_type = typename pointer_traits::difference_type;
		using value_type      = remove_cv_t<typename pointer_traits::element_type>;
		using pointer         = Pointer;
		using reference       = lvalue_reference_to_t<typename pointer_traits::element_type>;

		constexpr contiguous_iterator() noexcept = default;

		template <typename OtherPointer>
			requires (!std::is_same_v<OtherPointer, Pointer>) && std::is_convertible_v<OtherPointer, Pointer>
		constexpr contiguous_iterator(const contiguous_iterator<OtherPointer>& other) noexcept : p_{other.p_} {}

		template <typename OtherPointer>
			requires (!std::is_same_v<OtherPointer, Pointer>) && std::is_convertible_v<OtherPointer, Pointer>
		constexpr contiguous_iterator& operator=(const contiguous_iterator<OtherPointer>& other) noexcept
		{ p_ = other.p_; return *this; }

		constexpr contiguous_iterator operator++(int) noexcept { auto previous = *this; ++p_; return previous; }

		constexpr contiguous_iterator operator--(int) noexcept { auto previous = *this; --p_; return previous; }

		constexpr reference operator[](const difference_type n) const noexcept { return *(*this + n); }

		constexpr reference operator*() const noexcept { return *p_; }

		constexpr contiguous_iterator& operator++() noexcept { ++p_; return *this; }

		constexpr contiguous_iterator& operator--() noexcept { --p_; return *this; }

		constexpr contiguous_iterator& operator+=(const difference_type n) noexcept { p_ += n; return *this; }

		constexpr contiguous_iterator& operator-=(const difference_type n) noexcept { p_ -= n; return *this; }

		friend constexpr contiguous_iterator operator+(
			const contiguous_iterator& lhs,
			const difference_type      rhs) noexcept { return contiguous_iterator{lhs.p_ + rhs}; }

		friend constexpr contiguous_iterator operator-(
			const contiguous_iterator& lhs,
			const difference_type      rhs) noexcept { return contiguous_iterator{lhs.p_ - rhs}; }

		friend constexpr difference_type operator-(
			const contiguous_iterator& lhs,
			const contiguous_iterator& rhs) noexcept { return lhs.p_ - rhs.p_; }

		template <typename OtherPointer>
		friend constexpr bool operator==(
			const contiguous_iterator&               lhs,
			const contiguous_iterator<OtherPointer>& rhs) noexcept
		{ return lhs.equal_to(rhs); }

		template <typename OtherPointer>
		friend constexpr auto operator<=>(
			const contiguous_iterator&               lhs,
			const contiguous_iterator<OtherPointer>& rhs) noexcept
		{ return lhs.compare_three_way(rhs); }

		template <std::size_t I>
		friend constexpr auto* get(const contiguous_iterator& iter) noexcept
		{ return get<I>(iter.p_); }

		friend constexpr pointer to_address(const contiguous_iterator& iter) noexcept
		{ return agnes::to_address(iter.p_); }

		template <std::size_t... I>
		friend constexpr contiguous_iterator_subset_t<contiguous_iterator, I...> subset(
			const contiguous_iterator& iter) noexcept
		{ return iter.get_subset<I...>(); }

	private:
		template <typename, typename...> friend class contiguous_base;
		template <typename, typename...> friend class contiguous_storage;
		template <typename> friend class contiguous_iterator;

		constexpr contiguous_iterator(const pointer& p) noexcept : p_{p} {}

		template <typename OtherPointer>
			requires (!std::is_same_v<OtherPointer, Pointer>) && std::is_convertible_v<OtherPointer, Pointer>
		constexpr contiguous_iterator(const OtherPointer& p) noexcept : p_{p} {}

		template <typename OtherPointer>
		constexpr bool equal_to(const contiguous_iterator<OtherPointer>& rhs) const
		{ return p_ == rhs.p_; }

		template <typename OtherPointer>
		constexpr auto compare_three_way(const contiguous_iterator<OtherPointer>& rhs) const
		{ return p_ <=> rhs.p_; }

		template <std::size_t... I>
		constexpr contiguous_iterator_subset_t<contiguous_iterator, I...> get_subset() const
		{ return contiguous_iterator_subset_t<contiguous_iterator, I...>{subset<I...>(p_)}; }

		pointer p_;
	};
}

namespace agnes
{
	template <typename Pointer>
	constexpr const typename internal_::contiguous_iterator<Pointer>::pointer to_address(
		const internal_::contiguous_iterator<Pointer>& iter) noexcept
	{ return to_address(iter); }
}
