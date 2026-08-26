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

/// @file memory.h

#pragma once

#include <cstddef>
#include <memory>
#include <limits>
#include <type_traits>
#include <tuple>
#include <utility>

#include <agnes/internal_/allocator_traits_types.h>
#include <agnes/internal_/can_construct_at.h>
#include <agnes/internal_/pointer_traits_types.h>
#include <agnes/internal_/tuple_util.h>

#include <agnes/alignment.h>
#include <agnes/cache.h>
#include <agnes/fields.h>
#include <agnes/pointer.h>
#include <agnes/reference.h>

namespace agnes
{
	template <typename Ptr> struct pointer_traits;

	template <typename... T> struct pointer_traits<pointer<T*...>>;

	template <typename Ptr> constexpr auto to_address(const Ptr& p) noexcept;

	template <typename... T> constexpr pointer<T*...> to_address(const pointer<T*...>& p) noexcept;

	template <typename Alloc> struct allocator_traits;

	template <typename... T> class allocator;

	template <> class allocator<>;

	template <typename... T> constexpr pointer<T*...> addressof(const reference<T&...>& arg) noexcept;

	template <typename... T> pointer<const T*...> addressof(const reference<const T&&...>&&) = delete;

	template <typename... T>
		requires internal_::can_construct_at_v<pointer<T*...>>
	constexpr pointer<T*...> construct_at(const pointer<T*...>& locations);

	template <typename... T, typename... Args>
		requires internal_::can_construct_at_from_args_v<pointer<T*...>, Args...>
	constexpr pointer<T*...> construct_at(const pointer<T*...>& locations, Args&&... args);

	template <typename... T, typename Reference>
		requires internal_::can_construct_at_from_reference_v<pointer<T*...>, Reference>
	constexpr pointer<T*...> construct_at(const pointer<T*...>& locations, Reference&& args);

	template <typename... T, internal_::std_tuple_like TupleLike>
		requires internal_::can_construct_at_from_std_tuple_like_v<pointer<T*...>, TupleLike>
	constexpr pointer<T*...> construct_at(const pointer<T*...>& locations, TupleLike&& args);

	template <typename... T, typename... Tuples>
		requires internal_::can_construct_at_piecewise_v<pointer<T*...>, Tuples...>
	constexpr pointer<T*...> construct_at(
		const pointer<T*...>& locations,
		std::piecewise_construct_t,
		Tuples&&...           tuples);

	template <typename... T>
	constexpr void destroy_at(const pointer<T...>& locations);

	/// Provides a uniform interface to attributes of a
	/// variadic class of pointer-like types.
	template <typename Ptr>
	struct pointer_traits
	{
		/// The variadic class of pointer-like types this pointer_traits refers to.
		using pointer         = Ptr;

		/// A tag template that carries the types pointed to.
		///
		/// `Ptr::element_type` if the qualified-id `Ptr::element_type` is valid and
		/// denotes a type; otherwise, `fields<T...>` if `Ptr` is a
		/// class template instantiation of the form `SomePointer<T*...>`.
		using element_type    = internal_::pointer_traits_element_type_t<Ptr>;

		/// The difference type used by `Ptr`.
		///
		/// `Ptr::difference_type` if the qualified-id `Ptr::difference_type` is
		/// valid and denotes a type; otherwise, `std::ptrdiff_t`.
		using difference_type = internal_::pointer_traits_difference_type_t<Ptr>;

		/// Rebinds a variadic class of pointer-like types to other pointer-like types.
		///
		/// `Ptr::rebind<U...>` if the qualified-id `Ptr::rebind<U...>` is valid and
		/// denotes a type; otherwise, `SomePointer<U*...>` if `Ptr` is a
		/// class template instantiation of the form `SomePointer<T*...>`.
		template <typename... U> using rebind = internal_::pointer_traits_rebind_type_t<Ptr, U...>;

		/// Returns a dereferenceable variadic class of pointer-like types to `r`.
		static pointer pointer_to(const agnes::lvalue_reference_to_t<element_type>& r) { return Ptr::pointer_to(r); }
	};

	/// pointer_traits specialization for pointer<T*...>.
	template <typename... T>
	struct pointer_traits<pointer<T*...>>
	{
		using pointer         = agnes::pointer<T*...>; ///< See: pointer_traits::pointer
		using element_type    = fields<T...>;          ///< See: pointer_traits::element_type
		using difference_type = std::ptrdiff_t;        ///< See: pointer_traits::difference_type

		/// See: pointer_traits::rebind()
		template <typename... U> using rebind = agnes::pointer<U*...>;

		/// See: pointer_traits::pointer_to()
		static constexpr pointer pointer_to(const reference<T&...>& r) { return agnes::addressof(r); }
	};

	/// Returns the addresses pointed to by `p`.
	// TODO: implement
	template <typename Ptr> constexpr auto to_address(const Ptr& p) noexcept;

	/// Overload of to_address() for pointer<T*...>.
	template <typename... T> constexpr pointer<T*...> to_address(const pointer<T*...>& p) noexcept { return p; }

	/// Provides a uniform interface to variadic allocator types.
	template <typename Alloc>
	struct allocator_traits
	{
		/// The variadic allocator type this allocator_traits refers to.
		using allocator_type                         = Alloc;

		/// A tag template that carries the types Alloc allocates.
		using value_type                             = typename Alloc::value_type;

		/// A variadic class of pointer-like types that points to
		/// types in `value_type`.
		///
		/// `Alloc::pointer` if the qualified-id `Alloc::pointer` is
		/// valid and denotes a type; otherwise, `pointer_to<value_type>`.
		using pointer                                = internal_::allocator_traits_pointer_t<Alloc>;

		/// A variadic class of const pointer-like types that points to
		/// const types in `value_type`.
		///
		/// `Alloc::const_pointer` if the qualified-id `Alloc::const_pointer` is
		/// valid and denotes a type; otherwise,
		/// `pointer_traits<pointer>::rebind<add_const_t<value_type>>`.
		using const_pointer                          = internal_::allocator_traits_const_pointer_t<Alloc>;

		/// A variadic class of void pointer-like types.
		///
		/// `Alloc::void_pointer` if the qualified-id `Alloc::const_pointer` is
		/// valid and denotes a type; otherwise,
		/// `pointer_traits<pointer>::rebind<void_t<value_type>>`.
		using void_pointer                           = internal_::allocator_traits_void_pointer_t<Alloc>;

		/// A variadic class of const void pointer-like types.
		///
		/// `Alloc::const_void_pointer` if the qualified-id `Alloc::const_pointer` is
		/// valid and denotes a type; otherwise,
		/// `pointer_traits<pointer>::rebind<void_t<add_const_t<value_type>>>`.
		using const_void_pointer                     = internal_::allocator_traits_const_void_pointer_t<Alloc>;

		/// The difference type used by `Alloc`.
		///
		/// `Alloc::difference_type` if the qualified-id `Alloc::difference_type` is
		/// valid and denotes a type; otherwise, `pointer_traits<pointer>::difference_type`.
		using difference_type                        = internal_::allocator_traits_difference_type_t<Alloc>;

		/// The size type used by `Alloc`.
		///
		/// `Alloc::size_type` if the qualified-id `Alloc::size_type` is
		/// valid and denotes a type; otherwise, `pointer_traits<pointer>::size_type`.
		using size_type                              = internal_::allocator_traits_size_type_t<Alloc>;

		/// Provides a member constant `value` equal to `true` if
		/// container allocators of type `Alloc` should be replaced when
		/// copying the container; otherwise `value` equals `false`.
		///
		/// `Alloc::propagate_on_container_copy_assignment` if the
		/// qualified-id `Alloc::propagate_on_container_copy_assignment` is valid and
		/// denotes a type; otherwise `std::false_type`.
		using propagate_on_container_copy_assignment = internal_::allocator_traits_propagate_on_container_copy_assignment_t<Alloc>;

		/// Provides a member constant `value` equal to `true` if
		/// container allocators of type `Alloc` should be replaced when
		/// moving the container; otherwise `value` equals `false`.
		///
		/// `Alloc::propagate_on_container_move_assignment` if the
		/// qualified-id `Alloc::propagate_on_container_move_assignment` is valid and
		/// denotes a type; otherwise `std::false_type`.
		using propagate_on_container_move_assignment = internal_::allocator_traits_propagate_on_container_move_assignment_t<Alloc>;

		/// Provides a member constant `value` equal to `true` if
		/// container allocators of type `Alloc` should be replaced when
		/// swapping the container; otherwise `value` equals `false`.
		///
		/// `Alloc::propagate_on_container_swap` if the
		/// qualified-id `Alloc::propagate_on_container_swap` is valid and
		/// denotes a type; otherwise `std::false_type`.
		using propagate_on_container_swap            = internal_::allocator_traits_propagate_on_container_swap_t<Alloc>;

		/// Provides a member constant `value` equal to `true` if
		/// allocators of type `Alloc` are always equal (stateless);
		/// otherwise `value` equals `false`.
		///
		/// `Alloc::is_always_equal` if the qualified-id `Alloc::is_always_equal` is
		/// valid and denotes a type; otherwise `std::is_empty<Alloc>::type`.
		using is_always_equal                        = internal_::allocator_traits_is_always_equal_t<Alloc>;

		/// Provides a member typedef `type` that names the type
		/// of `Alloc` rebound to `T...`.
		///
		/// `Alloc::rebind<T...>::other` if the qualified-id `Alloc::rebind<T...>::other` is
		/// valid and denotes a type; otherwise, `Alloc<T...>` if `Alloc` is a
		/// class template instantiation of the form `Alloc<T...>`.
		template <typename... T> using rebind_alloc  = internal_::allocator_traits_rebind_alloc_t<Alloc, T...>;

		/// Provides a member typedef `type` that names the type
		/// of this allocator_traits rebound to `rebind_alloc<T...>`.
		template <typename... T> using rebind_traits = allocator_traits<rebind_alloc<T...>>;

		/// Returns `a.allocate(n)`.
		[[nodiscard]] static constexpr pointer allocate(Alloc& a, const size_type n)
		{ return a.allocate(n); }

		/// Calls `a.deallocate(p, n)`.
		static constexpr void deallocate(Alloc& a, const pointer& p, const size_type n)
		{ a.deallocate(p, n); }

		/// Calls `a.construct(p, std::forward<Args>(args)...)`
		/// if that call is well-formed.
		template <typename... T, typename... Args>
		static constexpr void construct(Alloc& a, const agnes::pointer<T*...>& p, Args&&... args)
			requires requires { a.construct(p, std::forward<Args>(args)...); }
		{ a.construct(p, std::forward<Args>(args)...); }

		/// Calls `construct_at(p, std::forward<Args>(args)...)`.
		/// if `a.construct(p, std::forward<Args>(args)...)` is not well-formed.
		template <typename... T, typename... Args>
		static constexpr void construct(Alloc& a, const agnes::pointer<T*...>& p, Args&&... args)
			requires (!requires { a.construct(p, std::forward<Args>(args)...); })
		{ construct_at(p, std::forward<Args>(args)...); }

		/// Calls `a.destroy(p)`.
		/// if that call is well-formed.
		template <typename... T>
		static constexpr void destroy(Alloc& a, const agnes::pointer<T*...>& p)
			requires requires { a.destroy(p); }
		{ a.destroy(p); }

		/// Calls `destroy_at(p)`.
		/// if `a.destroy(p)` is not well-formed.
		template <typename... T>
		static constexpr void destroy(Alloc& a, const agnes::pointer<T*...>& p)
			requires (!requires { a.destroy(p); })
		{ destroy_at(p); }

		/// Returns `a.max_size()`.
		/// if that call is well-formed.
		static constexpr size_type max_size(const Alloc& a) noexcept
			requires requires { a.max_size(); }
		{ return a.max_size(); }

		/// Returns `std::numeric_limits<size_type>::max()/allocation_size_v<value_type>`.
		/// if `a.max_size()` is not well-formed.
		static constexpr size_type max_size(const Alloc& a) noexcept
			requires (!requires { a.max_size(); })
		{ return std::numeric_limits<size_type>::max()/allocation_size_v<value_type>; }

		/// Returns `rhs.select_on_container_copy_construction()`.
		/// if that call is well-formed.
		static constexpr Alloc select_on_container_copy_construction(const Alloc& rhs)
			requires requires { rhs.select_on_container_copy_construction(); }
		{ return rhs.select_on_container_copy_construction(); }

		/// Returns `rhs`.
		/// if `rhs.select_on_container_copy_construction()` is not well-formed.
		static constexpr Alloc select_on_container_copy_construction(const Alloc& rhs)
			requires (!requires { rhs.select_on_container_copy_construction(); })
		{ return rhs; }
	};

	/// The default allocator used by agnes containers.
	template <typename... T>
	class allocator
	{
		static_assert(sizeof...(T) > 0, "agnes::allocator must be instantiated with at least one template parameter.");

		static_assert(( std::is_object_v  <T> && ...), "All allocation types must be objects."        );
		static_assert((!std::is_const_v   <T> && ...), "No allocation type may be const-qualified."   );
		static_assert((!std::is_volatile_v<T> && ...), "No allocation type may be volatile-qualified.");

	public:
		/// A tag template that carries the types allocated by the default allocator.
		using value_type                             = fields<T...>;

		/// The size type the default allocator uses.
		using size_type                              = std::size_t;

		/// The difference type the default allocator uses.
		using difference_type                        = std::ptrdiff_t;

		/// The default allocator propagates on move assignment.
		using propagate_on_container_move_assignment = std::true_type;

		/// The default allocator is always equal (stateless).
		using is_always_equal                        = std::true_type;

		/// Constructs a default allocator.
		constexpr allocator() noexcept = default;

		/// Copy-constructs a default allocator.
		constexpr allocator(const allocator&) noexcept = default;

		/// Copy-constructs a default allocator.
		template <typename... U>
		constexpr allocator(const allocator<U...>&) noexcept {}

		/// Destroys a default allocator.
		constexpr ~allocator() = default;

		/// For each `T...`, allocates `n*sizeof(T)` bytes of uninitialized storage
		/// by calling `::operator new(std::size_t, std::align_val_t)`.
		///
		/// When constant-evaluated, `sizeof...(T)` blocks of memory are allocated.
		///
		/// When runtime-evaluated, a single block of memory is allocated to
		/// hold all types arranged sequentially by type and may include
		/// padding between areas of memory belonging to each type.
		[[nodiscard]] constexpr pointer<T*...> allocate(const std::size_t n)
		{
			if (std::is_constant_evaluated())
			{
				return multi_block_allocate(n);
			}
			else
			{
				return single_block_allocate(n);
			}
		}

		/// Deallocates storage referenced by `p` that was obtained from
		/// a call to `allocate(n)`.
		constexpr void deallocate(const pointer<T*...>& p, const std::size_t n)
		{
			if (std::is_constant_evaluated())
			{
				multi_block_deallocate(p, n, indexes);
			}
			else
			{
				single_block_deallocate(p, n);
			}
		}

	private:
		static constexpr auto indexes = std::index_sequence_for<T...>{};

		using aligned_unit = aligned_unit_t<T...>;

		static constexpr pointer<T*...> multi_block_allocate(const std::size_t n)
		{ return {std::allocator<T>{}.allocate(n)...}; }

		template <std::size_t... I>
		static constexpr void multi_block_deallocate(
			const pointer<T*...>& p,
			const std::size_t     n,
			std::index_sequence<I...>)
		{ return (std::allocator<T>{}.deallocate(get<I>(p), n), ...); }

		static constexpr pointer<T*...> single_block_allocate(const std::size_t n)
		{
			const auto layout = aligned_unit_layout<T...>{n, get_default_allocator_cache_configuration()};

			auto* const bytes = std::allocator<aligned_unit>{}.allocate(layout.size());

			return layout.pointer_to(bytes);
		}

		static constexpr void single_block_deallocate(const pointer<T*...>& p, const std::size_t n)
		{
			std::allocator<aligned_unit>{}.deallocate(
				reinterpret_cast<aligned_unit*>(get<0>(p)),
				aligned_unit_layout_size<T...>(n, get_default_allocator_cache_configuration()));
		}
	};

	/// Compares two default allocators and always returns `true`.
	template <typename... T, typename... U>
	constexpr bool operator==(const allocator<T...>&, const allocator<U...>&) noexcept
	{ return true; }

	/// Returns the actual addresses of the objects referenced by `arg`.
	template <typename... T> constexpr pointer<T*...> addressof(const reference<T&...>& arg) noexcept
	{ return &arg; }

	/// Default-constructs `T...` objects at the given `locations`.
	template <typename... T>
		requires internal_::can_construct_at_v<pointer<T*...>>
	constexpr pointer<T*...> construct_at(const pointer<T*...>& locations)
	{
		[&locations]<std::size_t... I>(std::index_sequence<I...>)
		{
			auto count = std::size_t{0};

			try
			{
				((std::construct_at(get<I>(locations)), ++count), ...);
			}
			catch (...)
			{
				constexpr auto lastIndex = sizeof...(I) - 1;

				(((count > (lastIndex - I)) && (std::destroy_at(get<lastIndex - I>(locations)), true)), ...);

				throw;
			}
		}(std::index_sequence_for<T...>{});

		return locations;
	}

	/// Constructs `T...` objects at the given `locations` by forwarding a
	/// single argument from `args` to each.
	template <typename... T, typename... Args>
		requires internal_::can_construct_at_from_args_v<pointer<T*...>, Args...>
	constexpr pointer<T*...> construct_at(const pointer<T*...>& locations, Args&&... args)
	{
		[&locations]<std::size_t... I>(std::index_sequence<I...>, auto&&... args)
		{
			auto count = std::size_t{0};

			try
			{
				((std::construct_at(get<I>(locations), std::forward<decltype(args)>(args)), ++count), ...);
			}
			catch (...)
			{
				constexpr auto lastIndex = sizeof...(I) - 1;

				(((count > (lastIndex - I)) && (std::destroy_at(get<lastIndex - I>(locations)), true)), ...);

				throw;
			}
		}(std::index_sequence_for<T...>{}, std::forward<Args>(args)...);

		return locations;
	}

	/// Constructs `T...` objects at the given `locations` by forwarding a
	/// single argument from `args` to each.
	template <typename... T, typename Reference>
		requires internal_::can_construct_at_from_reference_v<pointer<T*...>, Reference>
	constexpr pointer<T*...> construct_at(const pointer<T*...>& locations, Reference&& args)
	{
		return [&locations]<std::size_t... I>(std::index_sequence<I...>, auto&& args)
		{
			return construct_at(locations, get<I>(std::forward<decltype(args)>(args))...);
		}(std::index_sequence_for<T...>{}, std::forward<Reference>(args));
	}

	/// Constructs `T...` objects at the given `locations` by forwarding a
	/// single argument from `args` to each.
	template <typename... T, internal_::std_tuple_like TupleLike>
		requires internal_::can_construct_at_from_std_tuple_like_v<pointer<T*...>, TupleLike>
	constexpr pointer<T*...> construct_at(const pointer<T*...>& locations, TupleLike&& args)
	{
		return [&locations]<std::size_t... I>(std::index_sequence<I...>, auto&& args)
		{
			return construct_at(locations, std::get<I>(std::forward<decltype(args)>(args))...);
		}(std::index_sequence_for<T...>{}, std::forward<TupleLike>(args));
	}

	/// Constructs `T...` objects at the given `locations` by forwarding the
	/// elements of each tuple of zero or more args contained in tuples.
	template <typename... T, typename... Tuples>
		requires internal_::can_construct_at_piecewise_v<pointer<T*...>, Tuples...>
	constexpr pointer<T*...> construct_at(
		const pointer<T*...>& locations,
		std::piecewise_construct_t,
		Tuples&&...           tuples)
	{
		[&locations]<std::size_t... I>(std::index_sequence<I...>, auto&&... tuples)
		{
			auto count = std::size_t{0};

			try
			{
				(([]<std::size_t... J>(std::index_sequence<J...>, auto* const location, auto&& t)
				{
					return std::construct_at(location, get<J>(std::forward<decltype(t)>(t))...);
				}(std::make_index_sequence<std::tuple_size_v<std::remove_reference_t<decltype(tuples)>>>{},
					get<I>(locations),
					std::forward<decltype(tuples)>(tuples)), ++count), ...);
			}
			catch (...)
			{
				constexpr auto lastIndex = sizeof...(I) - 1;

				(((count > (lastIndex - I)) && (std::destroy_at(get<lastIndex - I>(locations)), true)), ...);

				throw;
			}
		}(std::index_sequence_for<T...>{}, std::forward<Tuples>(tuples)...);

		return locations;
	}

	/// Destroys the objects at the given `locations`.
	template <typename... T>
	constexpr void destroy_at(const pointer<T...>& locations)
	{
		[&locations]<std::size_t... I>(std::index_sequence<I...>)
		{
			(std::destroy_at(get<I>(locations)), ...);
		}(std::index_sequence_for<T...>{});
	}
}
