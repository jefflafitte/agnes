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
#include <cstddef>
#include <functional>
#include <tuple>
#include <type_traits>
#include <utility>

#include <agnes/internal_/elements_are_assignable.h>
#include <agnes/internal_/pointer_tuple.h>
#include <agnes/internal_/tuple_util.h>

#include <agnes/pointer.h>

namespace agnes
{
	template <typename...> struct fields;

	template <typename...> class pointer;

	template <typename...> class reference;

	template <typename... T> class reference
	{
		static_assert(sizeof...(T) > 0, "agnes::reference must be instantiated with at least one template parameter.");
		static_assert(
			false,
			"All agnes::reference parameters must be either lvalue references or rvalue references, exclusively.");
	};

	template <typename... T> class reference<T&&...>;
	template <typename... T> class reference<T&... >;

	/// Variadic class template specialization that contains a collection of
	/// lvalue references to one or more object types.
	///
	/// reference<T&...> follows the reference collapsing rules of lvalue references.
	///
	/// In other words, each of the following is equivalent to `T&` for each `T...`.
	///
	/// @code
	///       reference<T&...>
	///       reference<T&...>&
	///       reference<T&...>&&
	/// const reference<T&...>
	/// const reference<T&...>&
	/// const reference<T&...>&&
	/// @endcode
	template <typename... T>
	class reference<T&...> : private reference<T&&...>
	{
		static_assert((std::is_object_v<T> && ...), "All references must reference to objects.");

	public:
		reference() = delete;

		/// Constructs a collection of lvalue references bound to `refs...`.
		///
		/// Given
		/// @code
		/// T t;
		/// @endcode
		///
		/// Equivalent to
		/// @code
		/// T& r{t};
		/// @endcode
		constexpr reference(T&... refs) noexcept : base{from_lvalue_ref, refs...} {}

		/// Constructs a collection of lvalue references by
		/// copying their bindings from `other`.
		///
		/// Given
		/// @code
		/// T  t;
		/// T& other{t};
		/// @endcode
		///
		/// Equivalent to
		/// @code
		/// T& r{other};
		/// @endcode
		constexpr reference(const reference& other) noexcept : base{from_lvalue_ref, other} {}

		/// Constructs a collection of lvalue references by
		/// copying their bindings from `other`.
		///
		/// Given
		/// @code
		/// T  t;
		/// T& other{t};
		/// @endcode
		///
		/// Equivalent to
		/// @code
		/// T& r{std::move(other)};
		/// @endcode
		constexpr reference(const reference&& other) noexcept : base{from_lvalue_ref, other} {}

		/// Constructs a collection of lvalue references by
		/// copying their bindings from `other`.
		///
		/// Given
		/// @code
		/// U  u;
		/// U& other{u};
		/// @endcode
		///
		/// Equivalent to
		/// @code
		/// T& r{other};
		/// @endcode
		template <typename... U>
			requires (sizeof...(T) == sizeof...(U)) && (std::is_constructible_v<T&, U&> && ...)
		constexpr reference(const reference<U&...>& other) noexcept : base{from_lvalue_ref, other} {}

		/// Constructs a collection of lvalue references by
		/// copying their bindings from `other`.
		///
		/// Given
		/// @code
		/// U  u;
		/// U& other{u};
		/// @endcode
		///
		/// Equivalent to
		/// @code
		/// T& r{other};
		/// @endcode
		template <typename... U>
			requires (sizeof...(T) == sizeof...(U)) && (std::is_constructible_v<T&, U&> && ...)
		constexpr reference(const reference<U&&...>& other) noexcept : base{from_lvalue_ref, other} {}

		/// Constructs a collection of lvalue references by
		/// copying their bindings from `other`.
		///
		/// Given
		/// @code
		/// U   u;
		/// U&& other{std::move(u)};
		/// @endcode
		///
		/// Equivalent to
		/// @code
		/// T& r{std::move(other)};
		/// @endcode
		template <typename... U>
			requires (sizeof...(T) == sizeof...(U)) && (std::is_constructible_v<T&, U&&> && ...)
		constexpr reference(const reference<U&&...>&& other) noexcept : base{from_lvalue_ref, other} {}

		/// Deleted to prevent fallback to copy constructor.
		template <typename... U>
			requires (sizeof...(T) != sizeof...(U)) || (!std::is_constructible_v<T&, U&&> || ...)
		constexpr reference(const reference<U&&...>&&) = delete;

		/// Copies the values of the objects referenced by other to the
		/// corresponding objects referenced by this collection of lvalue references.
		///
		/// Given
		/// @code
		/// T  t1;
		/// T  t2;
		/// T& other{t1};
		/// T& r    {t2};
		/// @endcode
		///
		/// Equivalent to
		/// @code
		/// r = other;
		/// @endcode
		constexpr reference& operator=(const reference& other)
			noexcept((std::is_nothrow_assignable_v<T&, T&> && ...))
			requires (std::is_assignable_v<T&, T&> && ...)
		{ base::copy_assign(other.ptrs_); return *this; }

		/// Copies the values of the objects referenced by other to the
		/// corresponding objects referenced by this collection of lvalue references.
		///
		/// Given
		/// @code
		/// T  t1;
		/// T  t2;
		/// T& other{t1};
		/// T& r    {t2};
		/// @endcode
		///
		/// Equivalent to
		/// @code
		/// r = other;
		/// @endcode
		constexpr reference& operator=(const reference&& other)
			noexcept((std::is_nothrow_assignable_v<T&, T&&> && ...))
			requires (std::is_assignable_v<T&, T&&> && ...)
		{ base::copy_assign(other.ptrs_); return *this; }

		/// Copies the values of the objects referenced by other into the
		/// corresponding objects referenced by this collection of lvalue references.
		///
		/// Given
		/// @code
		/// U  u;
		/// T  t;
		/// U& other{u};
		/// T& r    {t};
		/// @endcode
		///
		/// Equivalent to
		/// @code
		/// r = other;
		/// @endcode
		template <typename... U>
			requires (sizeof...(T) == sizeof...(U)) && (std::is_assignable_v<T&, U&> && ...)
		constexpr reference& operator=(const reference<U&...>& other)
			noexcept((std::is_nothrow_assignable_v<T&, U&> && ...))
		{ base::copy_assign(other.ptrs_); return *this; }

		/// Copies the values of the objects referenced by other into the
		/// corresponding objects referenced by this collection of lvalue references.
		///
		/// Given
		/// @code
		/// U  u;
		/// T  t;
		/// U& other{u};
		/// T& r    {t};
		/// @endcode
		///
		/// Equivalent to
		/// @code
		/// r = other;
		/// @endcode
		template <typename... U>
			requires (sizeof...(T) == sizeof...(U)) && (std::is_assignable_v<T&, U&> && ...)
		constexpr reference& operator=(const reference<U&&...>& other)
			noexcept((std::is_nothrow_assignable_v<T&, U&> && ...))
		{ base::copy_assign(other.ptrs_); return *this; }

		/// Moves the values of the objects referenced by other into the
		/// corresponding objects referenced by this collection of lvalue references.
		///
		/// Given
		/// @code
		/// U   u;
		/// T   t;
		/// U&& other{std::move(u)};
		/// T&  r    {t};
		/// @endcode
		///
		/// Equivalent to
		/// @code
		/// r = std::move(other);
		/// @endcode
		template <typename... U>
			requires (sizeof...(T) == sizeof...(U)) && (std::is_assignable_v<T&, U&&> && ...)
		constexpr reference& operator=(const reference<U&&...>&& other)
			noexcept((std::is_nothrow_assignable_v<T&, U&&> && ...))
		{ base::move_assign(other.ptrs_); return *this; }

		/// Forwards the values of the objects contained in other into the
		/// corresponding objects referenced by this collection of lvalue references.
		template <internal_::std_tuple_like TupleLike>
			requires internal_::elements_are_assignable_v<std::tuple<T&...>&, TupleLike>
		constexpr reference& operator=(TupleLike&& other)
			noexcept(internal_::elements_are_nothrow_assignable_v<std::tuple<T&...>&, TupleLike>)
		{ base::forward_assign(std::forward<TupleLike>(other)); return *this; }

		/// Returns a collection of pointers obtained from the addresses of the
		/// collection of lvalue references.
		constexpr pointer<T*...> operator&() const noexcept
		{ return base::operator&(); }

		/// Swaps the collection of references with the
		/// corresponding references in `other`.
		constexpr void swap(reference& other)
			noexcept((std::is_nothrow_swappable_v<T&> && ...))
		{ base::swap(other); }

		/// Swaps the collection of references with the
		/// corresponding references in `other`.
		constexpr void swap(reference<T&&...>& other)
			noexcept((std::is_nothrow_swappable_v<T&> && ...))
		{ base::swap(other); }

		/// Converts the collection of lvalue references to a collection of
		/// rvalue references and indicates the referenced objects can be moved.
		constexpr reference<T&&...>&& as_rvalue() noexcept
		{ return std::move(static_cast<base&>(*this)); }

		/// Converts the collection of lvalue references to a collection of
		/// rvalue references and indicates the referenced objects can be moved.
		constexpr const reference<T&&...>&& as_rvalue() const noexcept
		{ return std::move(static_cast<const base&>(*this)); }

		/// Compares the objects referenced by two collections of lvalue references.
		template <typename... U>
			requires (sizeof...(T) == sizeof...(U))
		friend constexpr bool operator==(const reference& lhs, const reference<U&...>& rhs)
			noexcept((noexcept(std::declval<T&>() == std::declval<U&>()) && ...))
		{ return lhs.equal_to(rhs); }

		/// Compares the objects referenced by two collections of references.
		template <typename... U>
			requires (sizeof...(T) == sizeof...(U))
		friend constexpr bool operator==(const reference& lhs, const reference<U&&...>& rhs)
			noexcept((noexcept(std::declval<T&>() == std::declval<U&>()) && ...))
		{ return lhs.equal_to(rhs); }

		/// Compares the objects referenced by a collection of lvalue references with a tuple.
		template <typename... U>
			requires (sizeof...(T) == sizeof...(U))
		friend constexpr bool operator==(const reference& lhs, const std::tuple<U...>& rhs)
			noexcept((noexcept(std::declval<T&>() == std::declval<U>()) && ...))
		{ return lhs.equal_to(rhs); }

		/// Lexicographically compares the objects referenced by
		// two collections of lvalue references.
		template <typename... U>
			requires (sizeof...(T) == sizeof...(U))
		friend constexpr auto operator<=>(const reference& lhs, const reference<U&...>& rhs)
			noexcept((noexcept(std::declval<T&>() <=> std::declval<U&>()) && ...))
		{ return lhs.compare_three_way(rhs); }

		/// Lexicographically compares the objects referenced by
		/// two collections of references.
		template <typename... U>
			requires (sizeof...(T) == sizeof...(U))
		friend constexpr auto operator<=>(const reference& lhs, const reference<U&&...>& rhs)
			noexcept((noexcept(std::declval<T&>() <=> std::declval<U&>()) && ...))
		{ return lhs.compare_three_way(rhs); }

		/// Lexicographically compares the objects referenced by
		/// a collection of lvalue references with a tuple.
		template <typename... U>
			requires (sizeof...(T) == sizeof...(U))
		friend constexpr auto operator<=>(const reference& lhs, const std::tuple<U...>& rhs)
			noexcept((noexcept(std::declval<T&>() <=> std::declval<U>()) && ...))
		{ return lhs.compare_three_way(rhs); }

		/// Returns the `I`th lvalue reference in a collection of lvalue references.
		template <std::size_t I>
		friend constexpr auto& get(const reference& r) noexcept
		{ return get<I>(static_cast<const base&>(r)); }

	private:
		template <typename... U> friend class reference;

		using base = reference<T&&...>;

		static constexpr auto from_lvalue_ref = typename base::from_lvalue_ref_t{};

		template <typename... U>
		constexpr bool equal_to(const reference<U&...>& other) const
		{ return static_cast<const base&>(*this).equal_to(other); }

		template <typename... U>
		constexpr bool equal_to(const reference<U&&...>& other) const
		{ return static_cast<const base&>(*this).equal_to(other); }

		template <typename... U>
		constexpr bool equal_to(const std::tuple<U...>& other) const
		{ return static_cast<const base&>(*this).equal_to(other); }

		template <typename... U>
		constexpr auto compare_three_way(const reference<U&...>& other) const
		{ return static_cast<const base&>(*this).compare_three_way(other); }

		template <typename... U>
		constexpr auto compare_three_way(const reference<U&&...>& other) const
		{ return static_cast<const base&>(*this).compare_three_way(other); }

		template <typename... U>
		constexpr auto compare_three_way(const std::tuple<U...>& other) const
		{ return static_cast<const base&>(*this).compare_three_way(other); }
	};

	/// Deduction guide for reference<T&...>.
	template <typename... T>
	reference(T&...) -> reference<T&...>;

	/// Deduction guide for reference<T&...>.
	template <typename... T>
	reference(reference<T&&...>&) -> reference<T&...>;

	/// Deduction guide for reference<T&...>.
	template <typename... T>
	reference(const reference<T&&...>&) -> reference<T&...>;

	template <typename> struct lvalue_reference_to;

	/// Provides a member typedef `type` that names the type `reference<T&...>`.
	template <typename... T>
	struct lvalue_reference_to<fields<T...>> : std::type_identity<reference<T&...>> {};

	/// Helper type that is an alias for `typename lvalue_reference_to<T>::type`.
	template <typename T>
	using lvalue_reference_to_t = typename lvalue_reference_to<T>::type;

	/// Swaps the collections of lvalue references in `x` and `y`.
	template <typename... T>
	constexpr void swap(reference<T&...>& x, reference<T&...>& y)
	{ x.swap(y); }

	/// Swaps the collections of references in `x` and `y`.
	template <typename... T>
	constexpr void swap(reference<T&...>& x, reference<T&&...>& y)
	{ x.swap(y); }

	/// Variadic class template specialization that contains a collection of
	/// rvalue references to one or more object types.
	///
	/// reference<T&&...> follows the reference collapsing rules of rvalue references.
	///
	/// In other words, each of the following is equivalent to `T&` for each `T...`.
	///
	/// @code
	///       reference<T&&>
	///       reference<T&&>&
	/// const reference<T&&>
	/// const reference<T&&>&
	/// @endcode
	///
	/// Whereas each of the following is equivalent to `T&&` for each `T...`.
	///
	/// @code
	///       reference<T&&>&&
	/// const reference<T&&>&&
	/// @endcode
	template <typename... T>
	class reference<T&&...>
	{
		static_assert((std::is_object_v<T> && ...));

	public:
		reference() = delete;

		/// Constructs a collection of rvalue references bound to `refs...`.
		///
		/// Given
		/// @code
		/// T t;
		/// @endcode
		///
		/// Equivalent to
		/// @code
		/// T&& r{std::move(t)};
		/// @endcode
		constexpr reference(T&&... refs) noexcept : ptrs_{&refs...} {}

		constexpr reference(const reference& other) noexcept = delete;

		/// Constructs a collection of rvalue references by
		/// copying their bindings from `other`.
		///
		/// Given
		/// @code
		/// T   t;
		/// T&& other{std::move(t)};
		/// @endcode
		///
		/// Equivalent to
		/// @code
		/// T&& r{std::move(other)};
		/// @endcode
		constexpr reference(const reference&& other) noexcept : ptrs_{other.ptrs_} {}

		/// Constructs a collection of rvalue references by
		/// copying their bindings from `other`.
		///
		/// Given
		/// @code
		/// T  t;
		/// T& other{t};
		/// @endcode
		///
		/// Equivalent to
		/// @code
		/// T& r{std::move(other)};
		/// @endcode
		template <typename... U>
			requires (sizeof...(T) == sizeof...(U)) && (std::is_constructible_v<T&&, U&&> && ...)
		constexpr reference(const reference<U&&...>&& other) noexcept : ptrs_{other.ptrs_} {}

		/// Copies the values of the objects referenced by other to the
		/// corresponding objects referenced by this collection of rvalue references.
		///
		/// Given
		/// @code
		/// T   t1;
		/// T   t2;
		/// T&  other{t1};
		/// T&& r    {std::move(t2)};
		/// @endcode
		///
		/// Equivalent to
		/// @code
		/// r = other;
		/// @endcode
		constexpr reference& operator=(const reference& other)
			noexcept((std::is_nothrow_assignable_v<T&, T&> && ...))
			requires (std::is_assignable_v<T&, T&> && ...)
		{ copy_assign(other.ptrs_); return *this; }

		/// Moves the values of the objects referenced by other to the
		/// corresponding objects referenced by this collection of rvalue references.
		///
		/// Given
		/// @code
		/// T   t1;
		/// T   t2;
		/// T&& other{std::move(t1)};
		/// T&& r    {std::move(t2)};
		/// @endcode
		///
		/// Equivalent to
		/// @code
		/// r = std::move(other);
		/// @endcode
		constexpr reference& operator=(const reference&& other)
			noexcept((std::is_nothrow_assignable_v<T&, T&&> && ...))
			requires (std::is_assignable_v<T&, T&&> && ...)
		{ move_assign(other.ptrs_); return *this; }

		/// Copies the values of the objects referenced by other to the
		/// corresponding objects referenced by this collection of rvalue references.
		///
		/// Given
		/// @code
		/// U   u;
		/// T   t;
		/// U&  other{u};
		/// T&& r    {std::move(t)};
		/// @endcode
		///
		/// Equivalent to
		/// @code
		/// r = other;
		/// @endcode
		template <typename... U>
			requires (sizeof...(T) == sizeof...(U)) && (std::is_assignable_v<T&, U&> && ...)
		constexpr reference& operator=(const reference<U&&...>& other)
			noexcept((std::is_nothrow_assignable_v<T&, U&> && ...))
		{ copy_assign(other.ptrs_); return *this; }

		/// Copies the values of the objects referenced by other to the
		/// corresponding objects referenced by this collection of rvalue references.
		///
		/// Given
		/// @code
		/// U   u;
		/// T   t;
		/// U&  other{u};
		/// T&& r    {std::move(t)};
		/// @endcode
		///
		/// Equivalent to
		/// @code
		/// r = other;
		/// @endcode
		template <typename... U>
			requires (sizeof...(T) == sizeof...(U)) && (std::is_assignable_v<T&, U&> && ...)
		constexpr reference& operator=(const reference<U&...>& other)
			noexcept((std::is_nothrow_assignable_v<T&, U&> && ...))
		{ copy_assign(other.ptrs_); return *this; }

		/// Moves the values of the objects referenced by other to the
		/// corresponding objects referenced by this collection of rvalue references.
		///
		/// Given
		/// @code
		/// U   u;
		/// T   t;
		/// U&& other{std::move(u)};
		/// T&& r    {std::move(t)};
		/// @endcode
		///
		/// Equivalent to
		/// @code
		/// r = std::move(other);
		/// @endcode
		template <typename... U>
			requires (sizeof...(T) == sizeof...(U)) && (std::is_assignable_v<T&, U&&> && ...)
		constexpr reference& operator=(const reference<U&&...>&& other)
			noexcept((std::is_nothrow_assignable_v<T&, U&&> && ...))
		{ move_assign(other.ptrs_); return *this; }

		/// Forwards the values of the objects contained in other into the
		/// corresponding objects referenced by this collection of rvalue references.
		template <internal_::std_tuple_like TupleLike>
			requires internal_::elements_are_assignable_v<std::tuple<T&...>&, TupleLike>
		constexpr reference& operator=(TupleLike&& other)
			noexcept(internal_::elements_are_nothrow_assignable_v<std::tuple<T&...>&, TupleLike>)
		{ forward_assign(std::forward<TupleLike>(other)); return *this; }

		/// Returns a collection of pointers obtained from the addresses of the
		/// collection of rvalue references.
		constexpr pointer<T*...> operator&() const noexcept
		{
			return [this]<std::size_t... I>(std::index_sequence<I...>)
			{
				return pointer<T*...>{get<I>(ptrs_)...};
			}(indexes);
		}

		/// Swaps the collection of references with the
		/// corresponding references in `other`.
		constexpr void swap(reference& other)
			noexcept((std::is_nothrow_swappable_v<T&> && ...))
		{
			[this, &other]<std::size_t... I>(std::index_sequence<I...>)
			{
				using std::swap;
				(swap(*get<I>(ptrs_), *get<I>(other.ptrs_)), ...);
			}(indexes);
		}

		/// Swaps the collection of references with the
		/// corresponding references in `other`.
		constexpr void swap(reference<T&...>& other)
			noexcept((std::is_nothrow_swappable_v<T&> && ...))
		{
			[this, &other]<std::size_t... I>(std::index_sequence<I...>)
			{
				using std::swap;
				(swap(*get<I>(ptrs_), *get<I>(other.ptrs_)), ...);
			}(indexes);
		}

		/// Compares the objects referenced by two collections of references.
		template <typename... U>
			requires (sizeof...(T) == sizeof...(U))
		friend constexpr bool operator==(const reference& lhs, const reference<U&&...>& rhs)
			noexcept((noexcept(std::declval<T&>() == std::declval<U&>()) && ...))
		{ return lhs.equal_to(rhs); }

		/// Compares the objects referenced by a collection of rvalue references with a tuple.
		template <typename... U>
			requires (sizeof...(T) == sizeof...(U))
		friend constexpr bool operator==(const reference& lhs, const std::tuple<U...>& rhs)
			noexcept((noexcept(std::declval<T&>() == std::declval<U>()) && ...))
		{ return lhs.equal_to(rhs); }

		/// Lexicographically compares the objects referenced by
		// two collections of rvalue references.
		template <typename... U>
			requires (sizeof...(T) == sizeof...(U))
		friend constexpr auto operator<=>(const reference& lhs, const reference<U&&...>& rhs)
			noexcept((noexcept(std::declval<T&>() <=> std::declval<U&>()) && ...))
		{ return lhs.compare_three_way(rhs); }

		/// Lexicographically compares the objects referenced by
		/// a collection of rvalue references with a tuple.
		template <typename... U>
			requires (sizeof...(T) == sizeof...(U))
		friend constexpr auto operator<=>(const reference& lhs, const std::tuple<U...>& rhs)
			noexcept((noexcept(std::declval<T&>() <=> std::declval<U>()) && ...))
		{ return lhs.compare_three_way(reference{rhs}); }

		/// Returns the `I`th collapsed lvalue reference in a collection of rvalue references.
		template <std::size_t I>
		friend constexpr auto& get(const reference& r) noexcept
		{ return *get<I>(r.ptrs_); }

		/// Returns the `I`th rvalue reference in a collection of rvalue references.
		template <std::size_t I>
		friend constexpr auto&& get(const reference&& r) noexcept
		{ return std::move(*get<I>(r.ptrs_)); }

	protected:
		struct from_lvalue_ref_t {};

		constexpr reference(from_lvalue_ref_t, T&... refs) noexcept : ptrs_{&refs...} {}

		template <typename... U>
		constexpr reference(from_lvalue_ref_t, const reference<U&...>& other) : ptrs_{other.ptrs_} {}

		template <typename... U>
		constexpr reference(from_lvalue_ref_t, const reference<U&&...>& other) : ptrs_{other.ptrs_} {}

		template <typename... U>
		constexpr void copy_assign(const internal_::pointer_tuple<U...>& ptrs)
		{
			[this, &ptrs]<std::size_t... I>(std::index_sequence<I...>)
			{
				((*get<I>(ptrs_) = *get<I>(ptrs)), ...);
			}(indexes);
		}

		template <typename... U>
		constexpr void move_assign(const internal_::pointer_tuple<U...>& ptrs)
		{
			[this, &ptrs]<std::size_t... I>(std::index_sequence<I...>)
			{
				((*get<I>(ptrs_) =  std::move(*get<I>(ptrs))), ...);
			}(indexes);
		}

		template <typename Tuple>
		constexpr void forward_assign(Tuple&& other)
		{
			[this]<std::size_t... I>(std::index_sequence<I...>, auto&& values)
			{
				((*get<I>(ptrs_) = std::get<I>(std::forward<decltype(values)>(values))), ...);
			}(indexes, std::forward<Tuple>(other));
		}

		template <typename... U>
		constexpr bool equal_to(const reference<U&&...>& other) const
		{
			return [this, &other]<std::size_t... I>(std::index_sequence<I...>)
			{
				return ((*get<I>(ptrs_) == *get<I>(other.ptrs_)) && ...);
			}(indexes);
		}

		template <typename... U>
		constexpr bool equal_to(const std::tuple<U...>& other) const
		{
			return [this, &other]<std::size_t... I>(std::index_sequence<I...>)
			{
				return ((*get<I>(ptrs_) == std::get<I>(other)) && ...);
			}(indexes);
		}

		template <typename... U>
		constexpr auto compare_three_way(const reference<U&&...>& other) const
		{
			return [this, &other]<std::size_t... I>(std::index_sequence<I...>)
			{
				return std::tuple{*get<I>(ptrs_)...} <=> std::tuple{*get<I>(other.ptrs_)...};
			}(indexes);
		}

		template <typename... U>
		constexpr auto compare_three_way(const std::tuple<U...>& other) const
		{
			return [this, &other]<std::size_t... I>(std::index_sequence<I...>)
			{
				return std::tuple{*get<I>(ptrs_)...} <=> other;
			}(indexes);
		}

	private:
		template <typename... U> friend class reference;

		static constexpr auto indexes = std::index_sequence_for<T...>{};

		internal_::pointer_tuple<T...> ptrs_;
	};

	/// Deduction guide for reference<T&&...>.
	template <typename... T>
	reference(T&&...) -> reference<T&&...>;

	/// Deduction guide for reference<T&&...>.
	template <typename... T>
	reference(reference<T&&...>&&) -> reference<T&&...>;

	/// Deduction guide for reference<T&&...>.
	template <typename... T>
	reference(const reference<T&&...>&&) -> reference<T&&...>;

	/// Invokes `f` with the collection of lvalue references in `r` as arguments.
	template <typename F, typename... T>
	constexpr decltype(auto) apply(F&& f, const reference<T&...>& r)
		noexcept(noexcept(std::invoke(std::forward<F>(f), std::declval<T&>()...)))
		requires std::is_invocable_v<F, T&...>
	{
		return [ff = std::forward<F>(f), &r]<std::size_t... I>(std::index_sequence<I...>) mutable -> decltype(auto)
		{
			return std::invoke(std::forward<F>(ff), get<I>(r)...);
		}(std::index_sequence_for<T...>{});
	}

	/// Invokes `f` with the collection of collapsed lvalue references in `r` as arguments.
	template <typename F, typename... T>
		requires std::is_invocable_v<F, T&...>
	constexpr decltype(auto) apply(F&& f, const reference<T&&...>& r)
		noexcept(noexcept(std::invoke(std::forward<F>(f), std::declval<T&>()...)))
	{
		return [ff = std::forward<F>(f), &r]<std::size_t... I>(std::index_sequence<I...>) mutable -> decltype(auto)
		{
			return std::invoke(std::forward<F>(ff), get<I>(r)...);
		}(std::index_sequence_for<T...>{});
	}

	/// Invokes `f` with the collection of rvalue references in `r` as arguments.
	template <typename F, typename... T>
		requires std::is_invocable_v<F, T&&...>
	constexpr decltype(auto) apply(F&& f, const reference<T&&...>&& r)
		noexcept(noexcept(std::invoke(std::forward<F>(f), std::declval<T&&>()...)))
	{
		return [ff = std::forward<F>(f), &r]<std::size_t... I>(std::index_sequence<I...>) mutable -> decltype(auto)
		{
			return std::invoke(std::forward<F>(ff), std::move(get<I>(std::move(r)))...);
		}(std::index_sequence_for<T...>{});
	}

	template <typename> struct reference_size;

	template <typename T>
	struct reference_size<const T> : std::integral_constant<std::size_t, reference_size<T>::value> {};

	/// Provides a member constant `value` equal to `sizeof...(T)`.
	template <typename... T>
	struct reference_size<reference<T&...>> : std::integral_constant<size_t, sizeof...(T)>
	{
		static_assert((std::is_object_v<T> && ...));
	};

	/// Provides a member constant `value` equal to `sizeof...(T)`.
	template <typename... T>
	struct reference_size<reference<T&&...>> : std::integral_constant<size_t, sizeof...(T)>
	{
		static_assert((std::is_object_v<T> && ...));
	};

	template <std::size_t, typename> struct reference_element;

	template <std::size_t I, typename T>
	struct reference_element<I, const T> : reference_element<I, T> {};

	/// Provides a member typedef `type` that names the
	/// `I`th lvalue reference type of `reference<T&...>`.
	template <std::size_t I, typename... T>
	struct reference_element<I, reference<T&...>> : std::tuple_element<I, std::tuple<T&...>>
	{
		static_assert((std::is_object_v<T> && ...));
	};

	/// Provides a member typedef `type` that names the
	/// `I`th rvalue reference type of `reference<T&&...>`.
	template <std::size_t I, typename... T>
	struct reference_element<I, reference<T&&...>> : std::tuple_element<I, std::tuple<T&&...>>
	{
		static_assert((std::is_object_v<T> && ...));
	};

	/// Helper type that is an alias for `typename reference_element<I, T>::type`.
	template <std::size_t I, typename T>
	using reference_element_t = typename reference_element<I, T>::type;

	template <typename, std::size_t...> struct reference_subset;

	template <typename Reference> struct reference_subset<Reference>;

	/// Provides a member typedef `type` that names the
	/// type `reference<reference_element_t<I, reference<T&...>>...>`
	template <typename... T, std::size_t... I>
	struct reference_subset<reference<T&...>, I...> :
		std::type_identity<reference<reference_element_t<I, reference<T&...>>...>>
	{
		static_assert((std::is_object_v<T> && ...));
	};

	/// Provides a member typedef `type` that names the
	/// type `reference<reference_element_t<I, reference<T&&...>>...>`
	template <typename... T, std::size_t... I>
	struct reference_subset<reference<T&&...>, I...> :
		std::type_identity<reference<reference_element_t<I, reference<T&&...>>...>>
	{
		static_assert((std::is_object_v<T> && ...));
	};

	/// Helper type that is an alias for `typename reference_subset<Reference, I...>::type`.
	template <typename Reference, std::size_t... I>
	using reference_subset_t = typename reference_subset<Reference, I...>::type;

	/// Swaps the collections of rvalue references in `x` and `y`.
	template <typename... T>
	constexpr void swap(reference<T&&...>& x, reference<T&&...>& y)
	{ x.swap(y); }

	/// Swaps the collections of references in `x` and `y`.
	template <typename... T>
	constexpr void swap(reference<T&&...>& x, reference<T&...>& y)
	{ x.swap(y); }

	/// Returns a subset of reference `r`.
	template <std::size_t...I, typename... T>
	constexpr reference_subset_t<reference<T&...>, I...> subset(const reference<T&...>& r)
	{ return {get<I>(r)...}; }

	/// Returns a subset of reference `r`.
	template <std::size_t...I, typename... T>
	constexpr reference_subset_t<reference<T&&...>, I...> subset(const reference<T&&...>& r)
	{ return {std::move(get<I>(r))...}; }

	/// Constant equal to `reference_size<T>::value`.
	template <typename T>
	inline constexpr std::size_t reference_size_v = reference_size<T>::value;
}

template <typename... T>
struct std::tuple_size<agnes::reference<T&...>> : std::tuple_size<std::tuple<T&...>>
{
	static_assert((std::is_object_v<T> && ...));
};

template <typename... T>
struct std::tuple_size<agnes::reference<T&&...>> : std::tuple_size<std::tuple<T&&...>>
{
	static_assert((std::is_object_v<T> && ...));
};

template <std::size_t I, typename... T>
struct std::tuple_element<I, agnes::reference<T&...>> : std::tuple_element<I, std::tuple<T&...>>
{
	static_assert((std::is_object_v<T> && ...));
};

template <std::size_t I, typename... T>
struct std::tuple_element<I, agnes::reference<T&&...>> : std::tuple_element<I, std::tuple<T&&...>>
{
	static_assert((std::is_object_v<T> && ...));
};
