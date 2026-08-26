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
#include <type_traits>

#include <agnes/internal_/algorithm.h>
#include <agnes/internal_/vector_base.h>

namespace agnes
{
	template <typename Allocator> struct allocate_with;

	template <typename... T> class allocator;
	template <typename... T> class pointer;

	template <typename... T>
	class vector : public internal_::make_vector_base_t<T...>
	{
		using base = internal_::make_vector_base_t<T...>;

	public:
		using value_type      = base::value_type;
		using allocator_type  = base::allocator_type;
		using pointer         = base::pointer;
		using const_pointer   = base::const_pointer;
		using reference       = base::reference;
		using const_reference = base::const_reference;
		using size_type       = base::size_type;
		using difference_type = base::difference_type;
		using iterator        = base::iterator;
		using const_iterator  = base::const_iterator;

		using base::base;

		constexpr vector(const vector&) = default;

		constexpr vector(vector&&) = default;

		constexpr vector& operator=(const vector&) = default;

		constexpr vector& operator=(vector&&) = default;

		using base::operator=;
	};

	/// Deduction guide for #vector.
	template <typename... T, typename Allocator = allocator<std::remove_const_t<T>...>>
	vector(const pointer<T*...>&, const pointer<T*...>&, const Allocator& = Allocator{}) ->
		vector<std::remove_const_t<T>..., allocate_with<Allocator>>;

	template <std::size_t I, typename Vector> struct vector_element;

	/// Provides a member typedef `type` that names the
	/// `I`th element type of `vector<T...>`.
	template <std::size_t I, typename... T> 
	struct vector_element<I, vector<T...>> : internal_::vector_base_element<I, internal_::make_vector_base_t<T...>> {};

	/// Helper type that is an alias for `typename vector_element<I, vector<T...>>::type`.
	template <std::size_t I, typename Vector>
	using vector_element_t = typename vector_element<I, Vector>::type;

	/// Determines if `lhs` and `rhs` contain the same number of
	/// object cross-sections and each cross-section in lhs compares equal with
	/// the cross-section in rhs at the same position. 
	template <typename... T>
	constexpr bool operator==(const vector<T...>& lhs, const vector<T...>& rhs)
	{
		return (&lhs == &rhs) ||
			((lhs.size() == rhs.size()) && internal_::equal(lhs.begin(), lhs.end(), rhs.begin(), rhs.end()));
	}

	/// Compare the contents of `lhs` and `rhs` lexicographically.
	template <typename... T>
	constexpr auto operator<=>(const vector<T...>& lhs, const vector<T...>& rhs)
	{
		using result_type = decltype(std::compare_three_way{}(*lhs.begin(), *rhs.begin()));

		return (&lhs == &rhs) ?
			result_type::equivalent :
			internal_::lexicographical_compare_three_way(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
	}

	/// Swaps the contents of `x` and `y`.
	template <typename... T>
	constexpr void swap(vector<T...>& x, vector<T...>& y) noexcept(noexcept(x.swap(y)))
	{ return x.swap(y); }

	/// Removes all object cross-sections from `v` that compare equal to
	/// the cross-section contained in `value`.
	template <typename... T, typename U>
	constexpr typename vector<T...>::size_type erase(vector<T...>& v, const U& value)
	{
		auto       newEnd = internal_::remove(v.begin(), v.end(), value);
		const auto result = v.cend() - newEnd;

		v.erase(newEnd, v.cend());

		return result;
	}

	/// Removes all object cross-sections from `v` that satisfy the predicate `pred`.
	template <typename... T, typename Predicate>
	constexpr typename vector<T...>::size_type erase_if(vector<T...>& v, Predicate pred)
	{
		auto       newEnd = internal_::remove_if(v.begin(), v.end(), pred);
		const auto result = v.cend() - newEnd;

		v.erase(newEnd, v.cend());

		return result;
	}

	template <typename Vector, std::size_t... I> class vector_subview;

	template <typename Vector> class vector_subview<Vector>;

	/// Subview of a vector that exposes cross-sections of
	/// only the objects at each index in `I...`.
	template <typename Vector, std::size_t... I>
	class vector_subview
	{
	public:
		using size_type = typename Vector::size_type;

		constexpr vector_subview(Vector& v) noexcept : v_{v} {}

		constexpr auto begin() const noexcept { return subset<I...>(v_.begin()); }

		constexpr auto end() const noexcept { return subset<I...>(v_.end()); }

		constexpr auto cbegin() const noexcept { return subset<I...>(v_.cbegin()); }

		constexpr auto cend() const noexcept { return subset<I...>(v_.cend()); }

		constexpr size_type size() const noexcept { return v_.size(); }

		constexpr auto operator[](const size_type n) const { return *(subset<I...>(v_.begin()) + n); }

		constexpr auto at(const size_type n) const { v_.base::check_range(n); return *(subset<I...>(v_.begin()) + n); }

		constexpr auto front() const { return *subset<I...>(v_.begin()); }

		constexpr auto back() const { return *(subset<I...>(v_.end()) - 1); }

		constexpr auto data() const noexcept { return subset<I...>(v_.data()); }

	private:
		Vector& v_;
	};

	/// Creates a mutable subview of #vector `v`.
	template <std::size_t...I, typename... T>
	constexpr auto subview(vector<T...>& v)
	{ return vector_subview<vector<T...>, I...>{v}; }

	/// Creates a const subview of #vector `v`.
	template <std::size_t...I, typename... T>
	constexpr auto subview(const vector<T...>& v)
	{ return vector_subview<const vector<T...>, I...>{v}; }

#ifdef AGNES_FOR_DOCUMENTATION_ONLY
	/// Stores sequences of objects of multiple types arranged linearly by type.
	/// Provides fast random access to any cross-section of objects.
	template <typename... T>
	class vector
	{
	public:
		/// The value types stored in cross-sections of the vector.
		using value_type      = fields<T...>;
		/// The allocator type used by the vector.
		using allocator_type  = implementation-defined;
		/// A type that provides pointers to mutable objects in a
		/// cross-section of the vector.
		using pointer         = allocator_traits::pointer;
		/// A type that provides pointers to const objects in a
		/// cross-section of the vector.
		using const_pointer   = allocator_traits::const_pointer;
		/// A type that provides references to mutable objects in a
		/// cross-section of the vector.
		using reference       = lvalue_reference_to_t<value_type>;
		/// A type that provides references to const objects in a
		/// cross-section of the vector.
		using const_reference = lvalue_reference_to_t<add_const_t<value_type>>;
		/// A type that represents sizes and indexes in the vector.
		using size_type       = allocator_traits::size_type;
		/// A type that represents distances in the vector.
		using difference_type = allocator_traits::difference_type;
		/// A random-access iterator to mutable cross-sections of
		/// objects in the vector.
		using iterator        = implementation-defined;
		/// A random-access iterator to const cross-sections of
		/// objects in the vector.
		using const_iterator  = implementation-defined;

		/// Constructs an empty vector with a default allocator.
		constexpr vector() noexcept(noexcept(Allocator{}));

		/// Constructs an empty vector with a given allocator.
		constexpr explicit vector(const Allocator& alloc) noexcept;

		/// Constructs a vector with `n` default-constructed
		/// cross-sections of objects.
		constexpr explicit vector(size_type n, const Allocator& alloc = Allocator{});

		/// Constructs a vector with `n` cross-sections of
		/// objects copied from `values`.
		constexpr vector(
			size_type        n,
			const T&...      values,
			const Allocator& alloc = Allocator{});

		/// Constructs a vector with `n` cross-sections of
		/// objects copied from `values`.
		constexpr vector(
			size_type                      n,
			agnes::reference<const T&...>& values,
			const allocator_type&          alloc = Allocator{});

		/// Constructs a vector by copying cross-sections of
		/// objects in the range [`first`, `last`).
		template <std::input_iterator InputIterator>
		constexpr vector(
			InputIterator    first,
			InputIterator    last,
			const Allocator& alloc = Allocator{});

		/// Constructs a vector by copying cross-sections of
		/// objects in the range [`first`, `last`).
		constexpr vector(
			const agnes::pointer<const T*...>& first,
			const agnes::pointer<const T*...>& last,
			const Allocator&                   alloc = Allocator{});

		/// Constructs a vector by copying cross-sections of
		/// objects in the range [`first`, `last`).
		constexpr vector(
			const_iterator&  first,
			const_iterator&  last,
			const Allocator& alloc = Allocator{});

		/// Constructs a vector by copying the contents of `other`.
		constexpr vector(const vector& other);

		/// Constructs a vector by moving the contents of `other`.
		constexpr vector(vector&& other) noexcept;

		/// Constructs a vector by copying the contents of `other` and
		/// using `alloc` as its allocator.
		constexpr vector(const vector& other, const Allocator& alloc);

		/// Constructs a vector by moving the contents of `other` and
		/// using `alloc` as its allocator.
		constexpr vector(vector&& other, const Allocator& alloc);

		/// Constructs a vector by copying cross-sections of objects in `il`.
		constexpr vector(
			std::initializer_list<std::tuple<T...>> il,
			const Allocator&                        alloc = Allocator{});

		/// Destroys the vector.
		constexpr ~vector();

		/// Replaces the contents of the vector by copying the contents of /p other.
		constexpr vector& operator=(const vector& other);

		/// Replaces the contents of the vector by moving the contents of /p other.
		constexpr vector& operator=(vector&& other);

		/// Replaces the contents of the vector by copying cross-sections of
		/// objects in `il`.
		constexpr vector& operator=(std::initializer_list<std::tuple<T...>> il);

		/// Replaces the contents of the vector by copying cross-sections of
		/// objects from the range [`first`, `last`).
		template <std::input_iterator InputIterator>
		constexpr void assign(InputIterator first, InputIterator last);

		/// Replaces the contents of the vector by copying cross-sections of
		/// objects from the range [`first`, `last`).
		template <typename... U>
			requires (sizeof...(T) == sizeof...(U))
		constexpr void assign(const agnes::pointer<U*...>& first, const agnes::pointer<U*...>& last);

		/// Replaces the contents of the vector by copying cross-sections of
		/// objects from the range [`first`, `last`).
		constexpr void assign(const const_iterator& first, const const_iterator& last);

		/// Replaces the contents of the vector by making `n` copies of `values`.
		constexpr void assign(size_type n, const T&... values);

		/// Replaces the contents of the vector by making `n` copies of `values`.
		constexpr void assign(size_type n, const agnes::reference<const T&...>& values);

		/// Replaces the contents of the vector by copying cross-sections of
		/// objects from `il`.
		constexpr void assign(std::initializer_list<std::tuple<T...>> il);

		/// Returns a copy of the allocator object.
		constexpr allocator_type get_allocator() const noexcept;

		/// Returns a random access iterator to
		/// the first cross-section of objects in the vector.
		constexpr const iterator& begin() noexcept;

		/// Returns a random access const iterator to
		/// the first cross-section of objects in the vector.
		constexpr const_iterator begin() const noexcept;

		/// Returns a random access iterator to
		/// the last cross-section of objects in the vector.
		constexpr const iterator& end() noexcept;

		/// Returns a random access const iterator to
		/// the last cross-section of objects in the vector.
		constexpr const_iterator end() const noexcept;

		/// Returns a random access const iterator to
		/// the first cross-section of objects in the vector.
		constexpr const_iterator cbegin() const noexcept;

		/// Returns a random access const iterator to
		/// the last cross-section of objects in the vector.
		constexpr const_iterator cend() const noexcept;

		/// Determines if the vector is empty.
		[[nodiscard]] constexpr bool empty() const noexcept;

		/// Returns the number of cross-sections of objects in the vector.
		constexpr size_type size() const noexcept;

		/// Returns the maximum number of cross-sections of
		/// objects the vector can hold.
		constexpr size_type max_size() const noexcept;

		/// Returns the number of cross-sections of
		/// objects the vector has allocated memory for.
		constexpr size_type capacity() const noexcept;

		/// Resizes the vector to contain `sz` cross-sections of objects and
		/// default-constructs any new cross-sections of objects.
		constexpr void resize(const size_type sz);

		/// Resizes the vector to contain `sz` cross-sections of objects and
		/// assigns any new cross-sections of objects to copies of `values`.
		constexpr void resize(size_type sz, const T&... values);

		/// Allocates memory for at least `n` cross-sections of objects.
		constexpr void reserve(size_type n);

		/// Reduces allocated memory to the exact amount needed by the vector.
		constexpr void shrink_to_fit();

		/// Returns mutable references to the cross-section of
		/// objects at index `n` in the vector.
		constexpr reference operator[](const size_type n);

		/// Returns const references to the cross-section of
		/// objects at index `n` in the vector.
		constexpr const_reference operator[](size_type n) const;

		/// Returns mutable references to the cross-section of
		/// objects at index `n` in the vector.
		constexpr reference at(size_type n);

		/// Returns const references to the cross-section of
		/// objects at index `n` in the vector.
		constexpr const_reference at(size_type n) const;

		/// Returns mutable references to the first cross-section of
		/// objects in the vector.
		constexpr reference front();

		/// Returns const references to the first cross-section of
		/// objects in the vector.
		constexpr const_reference front() const;

		/// Returns mutable references to the last cross-section of
		/// objects in the vector.
		constexpr reference back();

		/// Returns const references to the last cross-section of
		/// objects in the vector.
		constexpr const_reference back() const;

		/// Returns pointers to mutable objects in
		/// the first cross-section of the vector.
		constexpr agnes::pointer<T*...> data() noexcept;

		/// Returns pointers to const objects in
		/// the first cross-section of the vector.
		constexpr agnes::pointer<const T*...> data() const noexcept;

		/// Adds a cross-section of objects to the end of
		/// the vector by copying `values`.
		constexpr void push_back(const T&... values);

		/// Adds a cross-section of objects to the end of
		/// the vector by moving `values`.
		constexpr void push_back(T&&... values);

		/// Discards the last cross-section of objects in the vector.
		constexpr void pop_back();

		/// Constructs a cross-section of objects in place at the end of the vector.
		template <typename... Args>
		constexpr reference emplace_back(Args&&... args);

		/// Constructs a cross-section of objects in place in the vector at `position`.
		template <typename... Args>
		constexpr iterator emplace(const const_iterator& position, Args&&... args);

		/// Inserts a cross-section of objects in the vector at `position`
		/// and copies `values` into them.
		constexpr iterator insert(const const_iterator& position, const T&... values);

		/// Inserts a cross-section of objects in the vector at `position`
		/// and moves `values` into them.
		constexpr iterator insert(const const_iterator& position, T&&... values);

		/// Inserts `n` cross-sections of objects in the vector at `position`
		/// and copies `values` into them.
		constexpr iterator insert(const const_iterator& position, size_type n, const T&... values);

		/// Inserts each cross-section of objects in
		/// the range [`first`, `last`) into the vector at `position`.
		template <std::input_iterator InputIterator>
		constexpr iterator insert(
			const const_iterator& position,
			InputIterator         first,
			InputIterator         last);

		/// Inserts each cross-section of objects in
		/// the range [`first`, `last`) into the vector at `position`.
		template <typename... U>
			requires (sizeof...(T) == sizeof...(U))
		constexpr iterator insert(
			const const_iterator&        position,
			const agnes::pointer<U*...>& first,
			const agnes::pointer<U*...>& last);

		/// Inserts each cross-section of objects in
		/// the range [`first`, `last`) into the vector at `position`.
		constexpr iterator insert(
			const const_iterator& position,
			const const_iterator& first,
			const const_iterator& last);

		/// Inserts each cross-section of objects in
		/// `il` into the vector at `position`.
		constexpr iterator insert(const const_iterator& position, std::initializer_list<std::tuple<T...>> il);

		/// Removes the cross-section of objects from the vector at `position`. 
		constexpr iterator erase(const const_iterator& position);

		/// Removes the cross-section of objects from the vector in
		/// the range of postions [`first`, `last`). 
		constexpr iterator erase(const const_iterator& first, const const_iterator& last);

		/// Swaps the contents of the vector with `other`..
		constexpr void swap(vector& other);

		/// Clears the contents of the vector.
		constexpr void clear() noexcept;

		/// Returns a random access iterator to a subset of
		/// the first cross-section of objects in the vector.
		template <std::size_t... I>
		friend constexpr auto begin(vector_base& v) noexcept;

		/// Returns a random access const iterator to a subset of
		/// the first cross-section of objects in the vector.
		template <std::size_t... I>
		friend constexpr auto begin(const vector_base& v) noexcept;

		/// Returns a random access iterator to a subset of
		/// the last cross-section of objects in the vector.
		template <std::size_t... I>
		friend constexpr auto end(vector_base& v) noexcept;

		/// Returns a random access const iterator to a subset of
		/// the last cross-section of objects in the vector.
		template <std::size_t... I>
		friend constexpr auto end(const vector_base& v) noexcept;

		/// Returns a random access const iterator to a subset of
		/// the first cross-section of objects in the vector.
		template <std::size_t... I>
		friend constexpr auto cbegin(const vector_base& v) noexcept;

		/// Returns a random access const iterator to a subset of
		/// the last cross-section of objects in the vector.
		template <std::size_t... I>
		friend constexpr auto cend(const vector_base& v) noexcept;

		/// Returns mutable references to a subset of
		/// the cross-section of objects at index `n` in the vector.
		template <std::size_t... I>
		friend constexpr auto at(vector_base& v, size_type n);

		/// Returns const references to a subset of
		/// the cross-section of objects at index `n` in the vector.
		template <std::size_t... I>
		friend constexpr auto at(const vector_base& v, size_type n);

		/// Returns mutable references to a subset of
		/// the first cross-section of objects in the vector.
		template <std::size_t... I>
		friend constexpr auto front(vector_base& v);

		/// Returns const references to a subset of
		/// the first cross-section of objects in the vector.
		template <std::size_t... I>
		friend constexpr auto front(const vector_base& v);

		/// Returns mutable references to a subset of
		/// the last cross-section of objects in the vector.
		template <std::size_t... I>
		friend constexpr auto back(vector_base& v);

		/// Returns const references to a subset of
		/// the last cross-section of objects in the vector.
		template <std::size_t... I>
		friend constexpr auto back(const vector_base& v);
	};
#endif
}
