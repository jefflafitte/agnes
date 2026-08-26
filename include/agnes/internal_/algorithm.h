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

#include <compare>
#include <functional>

#include <agnes/utility.h>

namespace agnes { template <typename...> class reference; }

namespace agnes::internal_
{
	template <typename InputIterator1, typename InputIterator2>
	constexpr bool equal(
		const InputIterator1& first1,
		const InputIterator1& last1,
		const InputIterator2& first2,
		const InputIterator2& last2)
	{
		auto iter1 = first1;
		auto iter2 = first2;

		auto lhsExhausted = iter1 == last1;
		auto rhsExhausted = iter2 == last2;

		while (!lhsExhausted && !rhsExhausted)
		{
			if (!std::equal_to{}(*iter1, *iter2))
			{
				return false;
			}

			++iter1;
			++iter2;

			lhsExhausted = iter1 == last1;
			rhsExhausted = iter2 == last2;
		}

		return lhsExhausted && rhsExhausted;
	}

	template <typename InputIterator, typename OutputIterator>
	constexpr OutputIterator move(const InputIterator& first, const InputIterator& last, const OutputIterator& result)
	{
		auto dest = result;

		for (auto iter = first; iter != last; ++iter)
		{
			*dest++ = as_rvalue(*iter);
		}

		return dest;
	}

	template <typename BidirectionalIterator1, typename BidirectionalIterator2>
	constexpr BidirectionalIterator2 move_backward(
		const BidirectionalIterator1& first,
		const BidirectionalIterator1& last,
		const BidirectionalIterator2& result)
	{
		auto iter = last;
		auto dest = result;

		while (first != iter)
		{
			*--dest = as_rvalue(*--iter);
		}

		return dest;
	}

	template <typename ForwardIterator, typename... T>
	constexpr void fill(const ForwardIterator& first, const ForwardIterator& last, const reference<const T&...>& values)
	{
		for (auto iter = first; iter != last; ++iter)
		{
			*iter = values;
		}
	}

	template <typename OutputIterator, typename Size, typename... T>
	constexpr OutputIterator fill_n(const OutputIterator& first, Size n, const reference<const T&...>& values)
	{
		auto iter = first;

		for (; n > 0; --n)
		{
			*iter++ = values;
		}

		return iter;
	}

	template <typename ForwardIterator, typename Predicate>
	constexpr ForwardIterator remove_if(const ForwardIterator& first, const ForwardIterator& last, Predicate pred);

	template <typename ForwardIterator, typename T>
	constexpr ForwardIterator remove(ForwardIterator first, const ForwardIterator last, const T& value)
	{ return internal_::remove_if(first, last, [&value](const auto& elem) { return elem == value; }); }

	template <typename ForwardIterator, typename Predicate>
	constexpr ForwardIterator remove_if(const ForwardIterator& first, const ForwardIterator& last, Predicate pred)
	{
		auto iter = first;

		for (; iter != last; ++iter)
		{
			if (pred(*iter))
			{
				break;
			}
		}

		auto newLast = iter;

		if (iter != last)
		{
			while (++iter != last)
			{
				if (!pred(*iter))
				{
					*newLast++ = as_rvalue(*iter);
				}
			}
		}

		return newLast;
	}

	template <typename InputIterator1, typename InputIterator2>
	constexpr auto lexicographical_compare_three_way(
		const InputIterator1& b1,
		const InputIterator1& e1,
		const InputIterator2& b2,
		const InputIterator2& e2)
	{
		using result_type = decltype(std::compare_three_way{}(*b1, *b2));

		auto iter1 = b1;
		auto iter2 = b2;

		auto lhsExhausted = iter1 == e1;
		auto rhsExhausted = iter2 == e2;

		while (!lhsExhausted && !rhsExhausted)
		{
			if (auto result = std::compare_three_way{}(*iter1, *iter2); result != 0)
			{
				return result;
			}

			++iter1;
			++iter2;

			lhsExhausted = iter1 == e1;
			rhsExhausted = iter2 == e2;
		}

		return !lhsExhausted ? result_type::greater : !rhsExhausted ? result_type::less : result_type::equivalent;
	}
}
