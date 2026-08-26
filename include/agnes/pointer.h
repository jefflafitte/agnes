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

#include <cassert>
#include <compare>
#include <cstddef>
#include <functional>
#include <tuple>
#include <type_traits>
#include <utility>

#include <agnes/internal_/pointer_impl.h>

#include <agnes/reference.h>
#include <agnes/type_traits.h>

namespace agnes
{
	template <typename...> struct fields;

	template <typename...> class reference;

	template <typename... T> class pointer
	{
		static_assert(sizeof...(T) > 0, "agnes::pointer must be instantiated with at least one template parameter.");
		static_assert(false, "all agnes::pointer parameters must be pointers.");
	};

	/// Variadic class template specialization that contains a collection of
	/// pointers to one or more object types.
	template <typename... T>
	class pointer<T*...>
	{
		static_assert((std::is_object_v<T> && ...), "All pointers must point to objects.");

	public:
		/// Constructs a collection of null pointers.
		constexpr pointer() noexcept : pointer{nullptr} {}

		/// Constructs a collection of null pointers.
		constexpr pointer(std::nullptr_t) noexcept : pointer{static_cast<T*>(nullptr)...} {}

		/// Constructs a collection of pointers containing `ptrs...`.
		constexpr pointer(T* const... ptrs) noexcept : impl_{ptrs...} {}

		/// Constructs a collection of pointers by copying them from `other`.
		constexpr pointer(const pointer& other) noexcept = default;

		/// Constructs a collection of pointers by moving them from `other`.
		constexpr pointer(pointer&& other) noexcept = default;

		/// Constructs a collection of pointers by copying them from `other`.
		template <typename... U>
			requires (sizeof...(T) == sizeof...(U)) && (std::is_constructible_v<T*, U* const&> && ...)
		constexpr pointer(const pointer<U*...>& other) noexcept : impl_{other.impl_} {}

		/// Assigns this collection of pointers to a corresponding collection of pointers.
		constexpr pointer& operator=(const pointer&) noexcept = default;

		/// Assigns this collection of pointers to a corresponding collection of pointers.
		constexpr pointer& operator=(pointer&& other) noexcept = default;

		/// Assigns this collection of pointers to a corresponding collection of pointers.
		template <typename... U>
			requires (sizeof...(T) == sizeof...(U)) && (std::is_assignable_v<T*&, U* const&> && ...)
		constexpr pointer& operator=(const pointer<U*...>& other) noexcept
		{ impl_ = other.impl_; return *this; }

		/// Post-increments the collection of pointers.
		constexpr pointer operator++(int) noexcept
		{ auto previous = *this; ++(*this); return previous; }

		/// Post-decrements the collection of pointers.
		constexpr pointer operator--(int) noexcept
		{ auto previous = *this; --(*this); return previous; }

		/// Converts to `true` if the collection of pointers are not null.
		constexpr explicit operator bool() const noexcept
		{ return !impl_.is_null(); }

		/// Returns a collection of references obtained by dereferencing the
		/// collection of pointers after offsetting them by `n`.
		constexpr reference<T&...> operator[](const std::ptrdiff_t n) const noexcept
		{ return *(*this + n); }

		/// Returns a collection of references obtained by dereferencing the
		/// collection of pointers.
		constexpr reference<T&...> operator*() const noexcept
		{
			return [this]<std::size_t... I>(std::index_sequence<I...>)
			{
				return reference<T&...>{*get<I>(impl_)...};
			}(indexes);
		}

		/// Pre-increments the collection of pointers.
		constexpr pointer& operator++() noexcept
		{ impl_.increment(); return *this; }

		/// Pre-decrements the collection of pointers.
		constexpr pointer& operator--() noexcept
		{ impl_.decrement(); return *this; }

		/// Adds `n` to the collection of pointers and assigns them their new values.
		constexpr pointer& operator+=(const std::ptrdiff_t n) noexcept
		{ impl_.advance(n); return *this; }

		/// Subtracts `n` to the collection of pointers and assigns them their new values.
		constexpr pointer& operator-=(const std::ptrdiff_t n) noexcept
		{ impl_.advance(-n); return *this; }

		/// Swaps this collection of pointers with the corresponding pointers in `other`.
		constexpr void swap(pointer& other) noexcept
		{ impl_.swap(other.impl_); }

		/// Returns the result of adding `rhs` to a collection of pointers.
		friend constexpr pointer operator+(const pointer& lhs, const std::ptrdiff_t rhs) noexcept
		{ return pointer{lhs, rhs}; }

		/// Returns the result of subtracting `rhs` from a collection of pointers.
		friend constexpr pointer operator-(const pointer& lhs, const std::ptrdiff_t rhs) noexcept
		{ return pointer{lhs, -rhs}; }

		/// Returns the distance between two collections of pointers.
		friend constexpr std::ptrdiff_t operator-(const pointer& lhs, const pointer& rhs) noexcept
		{ return lhs.subtract(rhs); }

		/// Compares two collections of pointers.
		template <typename... U>
			requires (sizeof...(T) == sizeof...(U))
		friend constexpr bool operator==(const pointer& lhs, const pointer<U*...>& rhs) noexcept
		{ return lhs.equal_to(rhs); }

		/// Lexicographically compares two collections of pointers.
		template <typename... U>
			requires (sizeof...(T) == sizeof...(U))
		friend constexpr auto operator<=>(const pointer& lhs, const pointer<U*...>& rhs) noexcept
		{ return lhs.compare_three_way(rhs); }

		/// Returns the `I`th pointer in a collection of pointers.
		template <std::size_t I>
		friend constexpr auto* get(const pointer& p) noexcept
		{ return get<I>(p.impl_); }

	private:
		template <typename... U> friend class pointer;

		static constexpr auto indexes = std::index_sequence_for<T...>{};

		constexpr pointer(const pointer& other, const std::ptrdiff_t i) noexcept : impl_{other.impl_, i} {}

		constexpr std::ptrdiff_t subtract(const pointer& rhs) const
		{ return impl_.subtract(rhs.impl_); }

		template <typename... U>
		constexpr bool equal_to(const pointer<U*...>& rhs) const
		{ return impl_.equal_to(rhs.impl_); }

		template <typename... U>
		constexpr auto compare_three_way(const pointer<U*...>& rhs) const
		{ return impl_.compare_three_way(rhs.impl_); }

		internal_::pointer_impl<T...> impl_;
	};

	/// Deduction guide for pointer<T*...>.
	template <typename... T>
	pointer(T*...) -> pointer<T*...>;

	/// Constant #pointer<Ptrs...> that contains a collection of null pointers.
	template <typename... Ptrs>
	inline constexpr pointer<Ptrs...> null_ptr = nullptr;

	template <typename> struct pointer_to;

	/// Provides a member typedef `type` that names the type `pointer<T*...>`.
	template <typename... T>
	struct pointer_to<fields<T...>> : std::type_identity<pointer<T*...>> {};

	/// Helper type that is an alias for `typename pointer_to<T>::type`.
	template <typename T>
	using pointer_to_t = typename pointer_to<T>::type;

	/// Returns the result of adding `lhs` to a collection of pointers.
	template <typename... T>
	constexpr pointer<T*...> operator+(const std::ptrdiff_t n, const pointer<T*...>& rhs) noexcept
	{ return rhs + n; }

	/// Invokes `f` with the collection of pointers in `p` as arguments.
	template <typename F, typename... T>
		requires std::is_invocable_v<F, T*...>
	constexpr decltype(auto) apply(F&& f, const pointer<T*...>& p)
		noexcept(noexcept(std::invoke(std::forward<F>(f), std::declval<T*>()...)))
	{
		return [ff = std::forward<F>(f), &p]<std::size_t... I>(std::index_sequence<I...>) mutable -> decltype(auto)
		{
			return std::invoke(std::forward<F>(ff), get<I>(p)...);
		}(std::index_sequence_for<T...>{});
	}

	template <typename> struct pointer_size;

	template <typename T>
	struct pointer_size<const T> : std::integral_constant<std::size_t, pointer_size<T>::value> {};

	/// Provides a member constant `value` equal to `sizeof...(T)`.
	template <typename... T>
	struct pointer_size<pointer<T*...>> : std::integral_constant<size_t, sizeof...(T)>
	{
		static_assert((std::is_object_v<T> && ...));
	};

	template <std::size_t, typename> struct pointer_element;

	template <std::size_t I, typename T>
	struct pointer_element<I, const T> : pointer_element<I, T> {};

	/// Provides a member typedef `type` that names the
	/// `I`th pointer type of `pointer<T*...>`.
	template <std::size_t I, typename... T>
	struct pointer_element<I, pointer<T*...>> : std::tuple_element<I, std::tuple<T*...>>
	{
		static_assert((std::is_object_v<T> && ...));
	};

	/// Helper type that is an alias for `typename pointer_element<I, T>::type`.
	template <std::size_t I, typename T>
	using pointer_element_t = typename pointer_element<I, T>::type;

	template <typename, std::size_t...> struct pointer_subset;

	template <typename Pointer> struct pointer_subset<Pointer>;

	/// Provides a member typedef `type` that names the
	/// type `pointer<pointer_element_t<I, pointer<T*...>>...>`
	template <typename... T, std::size_t... I>
	struct pointer_subset<pointer<T*...>, I...> : std::type_identity<pointer<pointer_element_t<I, pointer<T*...>>...>>
	{
		static_assert((std::is_object_v<T> && ...));
	};

	/// Helper type that is an alias for `typename pointer_subset<Pointer, I...>::type`.
	template <typename Pointer, std::size_t... I>
	using pointer_subset_t = typename pointer_subset<Pointer, I...>::type;

	/// Swaps the collections of pointers in `x` and `y`.
	template <typename... T>
	constexpr void swap(pointer<T*...>& x, pointer<T*...>& y)
	{ x.swap(y); }

	/// Returns a subset of pointer `p`.
	template <std::size_t...I, typename... T>
	constexpr pointer_subset_t<pointer<T*...>, I...> subset(const pointer<T*...>& p)
	{ return {get<I>(p)...}; }

	/// Constant equal to `pointer_size<T>::value`.
	template <typename T>
	inline constexpr std::size_t pointer_size_v = pointer_size<T>::value;
}

template <typename... T>
struct std::tuple_size<agnes::pointer<T*...>> : std::tuple_size<std::tuple<T*...>>
{
	static_assert((std::is_object_v<T> && ...));
};

template <std::size_t I, typename... T>
struct std::tuple_element<I, agnes::pointer<T*...>> : std::tuple_element<I, std::tuple<T*...>>
{
	static_assert((std::is_object_v<T> && ...));
};
