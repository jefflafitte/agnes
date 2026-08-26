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

#include <tuple>
#include <type_traits>
#include <utility>

#include <agnes/reference.h>

namespace agnes_tests
{
	namespace internal_
	{
		template <typename, typename, typename = void>
		struct is_implicitly_constructible_impl : std::false_type {};

		template <typename T, typename U>
		struct is_implicitly_constructible_impl<
			T,
			U,
			std::void_t<decltype(std::declval<void(&)(T)>()(std::declval<U>()))>> : std::true_type {};

		template <typename, typename, typename = void>
		struct is_implicitly_nothrow_constructible_impl : std::false_type {};

		template <typename T, typename U>
		struct is_implicitly_nothrow_constructible_impl<
			T,
			U,
			std::void_t<decltype(std::declval<void(&)(T)noexcept>()(std::declval<U>()))>> : std::true_type {};
	}

	template <typename T, typename U>
	struct is_implicitly_constructible : internal_::is_implicitly_constructible_impl<T, U> {};

	template <typename T, typename U>
	inline constexpr bool is_implicitly_constructible_v = is_implicitly_constructible<T, U>::value;

	template <typename T, typename U>
	struct is_implicitly_nothrow_constructible : internal_::is_implicitly_nothrow_constructible_impl<T, U> {};

	template <typename T, typename U>
	inline constexpr bool is_implicitly_nothrow_constructible_v = is_implicitly_nothrow_constructible<T, U>::value;

	template <typename T, typename U, bool Expected>
	struct is_reference_consistently_implicitly_nothrow_constructible;

	template <typename T, typename U, bool Expected>
	inline constexpr bool is_reference_consistently_implicitly_nothrow_constructible_v =
		is_reference_consistently_implicitly_nothrow_constructible<T, U, Expected>::value;

	template <typename T, typename U, bool Expected>
	struct is_reference_consistently_implicitly_nothrow_constructible<T&, U&, Expected>
	{
		static constexpr bool builtin               = is_implicitly_nothrow_constructible_v<                 T& ,                        U&    >;
		static constexpr bool fromRef               = is_implicitly_nothrow_constructible_v<agnes::reference<T&>,                        U&    >;
		static constexpr bool fromRefToAgRef        = is_implicitly_nothrow_constructible_v<agnes::reference<T&>,       agnes::reference<U& >& >;
		static constexpr bool fromConstRefToAgRef   = is_implicitly_nothrow_constructible_v<agnes::reference<T&>, const agnes::reference<U& >& >;
		static constexpr bool fromRvRefToAgRef      = is_implicitly_nothrow_constructible_v<agnes::reference<T&>,       agnes::reference<U& >&&>;
		static constexpr bool fromConstRvRefToAgRef = is_implicitly_nothrow_constructible_v<agnes::reference<T&>, const agnes::reference<U& >&&>;
		static constexpr bool fromRefToAgRvRef      = is_implicitly_nothrow_constructible_v<agnes::reference<T&>,       agnes::reference<U&&>& >;
		static constexpr bool fromRefToConstAgRvRef = is_implicitly_nothrow_constructible_v<agnes::reference<T&>, const agnes::reference<U&&>& >;

		static_assert(builtin               == Expected);
		static_assert(fromRef               == Expected);
		static_assert(fromRefToAgRef        == Expected);
		static_assert(fromConstRefToAgRef   == Expected);
		static_assert(fromRvRefToAgRef      == Expected);
		static_assert(fromConstRvRefToAgRef == Expected);
		static_assert(fromRefToAgRvRef      == Expected);
		static_assert(fromRefToConstAgRvRef == Expected);

		static constexpr bool value =
			(builtin               == Expected) &&
			(fromRef               == Expected) &&
			(fromRefToAgRef        == Expected) &&
			(fromConstRefToAgRef   == Expected) &&
			(fromRvRefToAgRef      == Expected) &&
			(fromConstRvRefToAgRef == Expected) &&
			(fromRefToAgRvRef      == Expected) &&
			(fromRefToConstAgRvRef == Expected);
	};

	template <typename T, typename U, bool Expected>
	struct is_reference_consistently_implicitly_nothrow_constructible<T&&, U&, Expected>
	{
		static constexpr bool builtin               = is_implicitly_nothrow_constructible_v<                 T&&,                         U&    >;
		static constexpr bool fromRef               = is_implicitly_nothrow_constructible_v<agnes::reference<T&&>,                        U&    >;
		static constexpr bool fromRefToAgRef        = is_implicitly_nothrow_constructible_v<agnes::reference<T&&>,       agnes::reference<U& >& >;
		static constexpr bool fromConstRefToAgRef   = is_implicitly_nothrow_constructible_v<agnes::reference<T&&>, const agnes::reference<U& >& >;
		static constexpr bool fromRvRefToAgRef      = is_implicitly_nothrow_constructible_v<agnes::reference<T&&>,       agnes::reference<U& >&&>;
		static constexpr bool fromConstRvRefToAgRef = is_implicitly_nothrow_constructible_v<agnes::reference<T&&>, const agnes::reference<U& >&&>;
		static constexpr bool fromRefToAgRvRef      = is_implicitly_nothrow_constructible_v<agnes::reference<T&&>,       agnes::reference<U&&>& >;
		static constexpr bool fromRefToConstAgRvRef = is_implicitly_nothrow_constructible_v<agnes::reference<T&&>, const agnes::reference<U&&>& >;

		static_assert(builtin               == Expected);
		static_assert(fromRef               == Expected);
		static_assert(fromRefToAgRef        == Expected);
		static_assert(fromConstRefToAgRef   == Expected);
		static_assert(fromRvRefToAgRef      == Expected);
		static_assert(fromConstRvRefToAgRef == Expected);
		static_assert(fromRefToAgRvRef      == Expected);
		static_assert(fromRefToConstAgRvRef == Expected);

		static constexpr bool value =
			(builtin               == Expected) &&
			(fromRef               == Expected) &&
			(fromRefToAgRef        == Expected) &&
			(fromConstRefToAgRef   == Expected) &&
			(fromRvRefToAgRef      == Expected) &&
			(fromConstRvRefToAgRef == Expected) &&
			(fromRefToAgRvRef      == Expected) &&
			(fromRefToConstAgRvRef == Expected);
	};

	template <typename T, typename U, bool Expected>
	struct is_reference_consistently_implicitly_nothrow_constructible<T&, U&&, Expected>
	{
		static constexpr bool builtin                 = is_implicitly_nothrow_constructible_v<                 T&,                         U&&   >;
		static constexpr bool fromRvRef               = is_implicitly_nothrow_constructible_v<agnes::reference<T&>,                        U&&   >;
		static constexpr bool fromRvRefToAgRvRef      = is_implicitly_nothrow_constructible_v<agnes::reference<T&>,       agnes::reference<U&&>&&>;
		static constexpr bool fromConstRvRefToAgRvRef = is_implicitly_nothrow_constructible_v<agnes::reference<T&>, const agnes::reference<U&&>&&>;

		static_assert(builtin                 == Expected);
		static_assert(fromRvRef               == Expected);
		static_assert(fromRvRefToAgRvRef      == Expected);
		static_assert(fromConstRvRefToAgRvRef == Expected);

		static constexpr bool value =
			(builtin                 == Expected) &&
			(fromRvRef               == Expected) &&
			(fromRvRefToAgRvRef      == Expected) &&
			(fromConstRvRefToAgRvRef == Expected);
	};

	template <typename T, typename U, bool Expected>
	struct is_reference_consistently_implicitly_nothrow_constructible<T&&, U&&, Expected>
	{
		static constexpr bool builtin            = is_implicitly_nothrow_constructible_v<                 T&&,                   U&&   >;
		static constexpr bool fromRvRef          = is_implicitly_nothrow_constructible_v<agnes::reference<T&&>,                  U&&   >;
		static constexpr bool fromRvRefToAgRvRef = is_implicitly_nothrow_constructible_v<agnes::reference<T&&>, agnes::reference<U&&>&&>;

		static_assert(builtin            == Expected);
		static_assert(fromRvRef          == Expected);
		static_assert(fromRvRefToAgRvRef == Expected);

		static constexpr bool value =
			(builtin            == Expected) &&
			(fromRvRef          == Expected) &&
			(fromRvRefToAgRvRef == Expected);
	};

	namespace internal_
	{
		template <template <typename...> typename Test, typename T, typename U, bool Expected>
		struct is_reference_consistently_assignable_impl;

		template <template <typename...> typename Test, typename T, typename U, bool Expected>
		struct is_reference_consistently_assignable_impl<Test, T&, U&, Expected>
		{
			static constexpr bool builtin                = Test<std::type_identity_t<T&>&,                        U&    >::value;
			static constexpr bool tuple                  = Test<std::tuple          <T&>&, const std::tuple      <U& >& >::value;
			static constexpr bool toRefToAgRef           = Test<agnes::reference    <T&>&,       agnes::reference<U& >& >::value;
			static constexpr bool toConstRefToAgRef      = Test<agnes::reference    <T&>&, const agnes::reference<U& >& >::value;
			static constexpr bool toRvRefToAgRef         = Test<agnes::reference    <T&>&,       agnes::reference<U& >&&>::value;
			static constexpr bool toConstRvRefToAgRef    = Test<agnes::reference    <T&>&, const agnes::reference<U& >&&>::value;
			static constexpr bool toRefToAgRvRef         = Test<agnes::reference    <T&>&,       agnes::reference<U&&>& >::value;
			static constexpr bool toConstRefToAgRvRef    = Test<agnes::reference    <T&>&, const agnes::reference<U&&>& >::value;
			static constexpr bool toRefToTuple           = Test<agnes::reference    <T&>&,       std::tuple      <U  >& >::value;
			static constexpr bool toConstRefToTuple      = Test<agnes::reference    <T&>&, const std::tuple      <U  >& >::value;
			static constexpr bool toRvRefToTuple         = Test<agnes::reference    <T&>&,       std::tuple      <U  >&&>::value;
			static constexpr bool toConstRvRefToTuple    = Test<agnes::reference    <T&>&, const std::tuple      <U  >&&>::value;
			static constexpr bool toRefToRefTuple        = Test<agnes::reference    <T&>&,       std::tuple      <U& >& >::value;
			static constexpr bool toConstRefToRefTuple   = Test<agnes::reference    <T&>&, const std::tuple      <U& >& >::value;
			static constexpr bool toRvRefToRefTuple      = Test<agnes::reference    <T&>&,       std::tuple      <U& >&&>::value;
			static constexpr bool toConstRvRefToRefTuple = Test<agnes::reference    <T&>&, const std::tuple      <U& >&&>::value;

			static_assert(builtin                == Expected);
			static_assert(tuple                  == Expected);
			static_assert(toRefToAgRef           == Expected);
			static_assert(toConstRefToAgRef      == Expected);
			static_assert(toRvRefToAgRef         == Expected);
			static_assert(toConstRvRefToAgRef    == Expected);
			static_assert(toRefToAgRvRef         == Expected);
			static_assert(toConstRefToAgRvRef    == Expected);
			static_assert(toRefToTuple           == Expected);
			static_assert(toConstRefToTuple      == Expected);
			static_assert(toRvRefToTuple         == Expected);
			static_assert(toConstRvRefToTuple    == Expected);
			static_assert(toRefToRefTuple        == Expected);
			static_assert(toConstRefToRefTuple   == Expected);
			static_assert(toRvRefToRefTuple      == Expected);
			static_assert(toConstRvRefToRefTuple == Expected);

			static constexpr bool value =
				(builtin                == Expected) &&
				(tuple                  == Expected) &&
				(toRefToAgRef           == Expected) &&
				(toConstRefToAgRef      == Expected) &&
				(toRvRefToAgRef         == Expected) &&
				(toConstRvRefToAgRef    == Expected) &&
				(toRefToAgRvRef         == Expected) &&
				(toConstRefToAgRvRef    == Expected) &&
				(toRefToTuple           == Expected) &&
				(toConstRefToTuple      == Expected) &&
				(toRvRefToTuple         == Expected) &&
				(toConstRvRefToTuple    == Expected) &&
				(toRefToRefTuple        == Expected) &&
				(toConstRefToRefTuple   == Expected) &&
				(toRvRefToRefTuple      == Expected) &&
				(toConstRvRefToRefTuple == Expected);
		};

		template <template <typename...> typename Test, typename T, typename U, bool Expected>
		struct is_reference_consistently_assignable_impl<Test, T&&, U&, Expected>
		{
			static constexpr bool builtin                = Test<std::type_identity_t<T&&>&,                        U&    >::value;
			static constexpr bool tuple                  = Test<std::tuple          <T&&>&, const std::tuple      <U& >& >::value;
			static constexpr bool toRefToAgRef           = Test<agnes::reference    <T&&>&,       agnes::reference<U& >& >::value;
			static constexpr bool toConstRefToAgRef      = Test<agnes::reference    <T&&>&, const agnes::reference<U& >& >::value;
			static constexpr bool toRvRefToAgRef         = Test<agnes::reference    <T&&>&,       agnes::reference<U& >&&>::value;
			static constexpr bool toConstRvRefToAgRef    = Test<agnes::reference    <T&&>&, const agnes::reference<U& >&&>::value;
			static constexpr bool toRefToAgRvRef         = Test<agnes::reference    <T&&>&,       agnes::reference<U&&>& >::value;
			static constexpr bool toConstRefToAgRvRef    = Test<agnes::reference    <T&&>&, const agnes::reference<U&&>& >::value;
			static constexpr bool toRefToTuple           = Test<agnes::reference    <T&&>&,       std::tuple      <U  >& >::value;
			static constexpr bool toConstRefToTuple      = Test<agnes::reference    <T&&>&, const std::tuple      <U  >& >::value;
			static constexpr bool toRvRefToTuple         = Test<agnes::reference    <T&&>&,       std::tuple      <U  >&&>::value;
			static constexpr bool toConstRvRefToTuple    = Test<agnes::reference    <T&&>&, const std::tuple      <U  >&&>::value;
			static constexpr bool toRefToRefTuple        = Test<agnes::reference    <T&&>&,       std::tuple      <U& >& >::value;
			static constexpr bool toConstRefToRefTuple   = Test<agnes::reference    <T&&>&, const std::tuple      <U& >& >::value;
			static constexpr bool toRvRefToRefTuple      = Test<agnes::reference    <T&&>&,       std::tuple      <U& >&&>::value;
			static constexpr bool toConstRvRefToRefTuple = Test<agnes::reference    <T&&>&, const std::tuple      <U& >&&>::value;

			static_assert(builtin                == Expected);
			static_assert(tuple                  == Expected);
			static_assert(toRefToAgRef           == Expected);
			static_assert(toConstRefToAgRef      == Expected);
			static_assert(toRvRefToAgRef         == Expected);
			static_assert(toConstRvRefToAgRef    == Expected);
			static_assert(toRefToAgRvRef         == Expected);
			static_assert(toConstRefToAgRvRef    == Expected);
			static_assert(toRefToTuple           == Expected);
			static_assert(toConstRefToTuple      == Expected);
			static_assert(toRvRefToTuple         == Expected);
			static_assert(toConstRvRefToTuple    == Expected);
			static_assert(toRefToRefTuple        == Expected);
			static_assert(toConstRefToRefTuple   == Expected);
			static_assert(toRvRefToRefTuple      == Expected);
			static_assert(toConstRvRefToRefTuple == Expected);

			static constexpr bool value =
				(builtin                == Expected) &&
				(tuple                  == Expected) &&
				(toRefToAgRef           == Expected) &&
				(toConstRefToAgRef      == Expected) &&
				(toRvRefToAgRef         == Expected) &&
				(toConstRvRefToAgRef    == Expected) &&
				(toRefToAgRvRef         == Expected) &&
				(toConstRefToAgRvRef    == Expected) &&
				(toRefToTuple           == Expected) &&
				(toConstRefToTuple      == Expected) &&
				(toRvRefToTuple         == Expected) &&
				(toConstRvRefToTuple    == Expected) &&
				(toRefToRefTuple        == Expected) &&
				(toConstRefToRefTuple   == Expected) &&
				(toRvRefToRefTuple      == Expected) &&
				(toConstRvRefToRefTuple == Expected);
		};

		template <template <typename...> typename Test, typename T, typename U, bool Expected>
		struct is_reference_consistently_assignable_impl<Test, T&, U&&, Expected>
		{
			static constexpr bool builtin                  = Test<std::type_identity_t<T&>&,                        U&&   >::value;
			static constexpr bool tuple                    = Test<std::tuple          <T&>&,       std::tuple      <U&&>&&>::value;
			static constexpr bool toRvRefToAgRvRef         = Test<agnes::reference    <T&>&,       agnes::reference<U&&>&&>::value;
			static constexpr bool toConstRvRefToAgRvRef    = Test<agnes::reference    <T&>&, const agnes::reference<U&&>&&>::value;
			static constexpr bool toRvRefToRvRefTuple      = Test<agnes::reference    <T&>&,       std::tuple      <U&&>&&>::value;
			static constexpr bool toConstRvRefToRvRefTuple = Test<agnes::reference    <T&>&, const std::tuple      <U&&>&&>::value;

			static_assert(builtin                  == Expected);
			static_assert(tuple                    == Expected);
			static_assert(toRvRefToAgRvRef         == Expected);
			static_assert(toConstRvRefToAgRvRef    == Expected);
			static_assert(toRvRefToRvRefTuple      == Expected);
			static_assert(toConstRvRefToRvRefTuple == Expected);

			static constexpr bool value =
				(builtin                  == Expected) &&
				(tuple                    == Expected) &&
				(toRvRefToAgRvRef         == Expected) &&
				(toConstRvRefToAgRvRef    == Expected) &&
				(toRvRefToRvRefTuple      == Expected) &&
				(toConstRvRefToRvRefTuple == Expected);
		};

		template <template <typename...> typename Test, typename T, typename U, bool Expected>
		struct is_reference_consistently_assignable_impl<Test, T&&, U&&, Expected>
		{
			static constexpr bool builtin                  = Test<std::type_identity_t<T&&>&,                        U&&   >::value;
			static constexpr bool tuple                    = Test<std::tuple          <T&&>&,       std::tuple      <U&&>&&>::value;
			static constexpr bool toRvRefToAgRvRef         = Test<agnes::reference    <T&&>&,       agnes::reference<U&&>&&>::value;
			static constexpr bool toConstRvRefToAgRvRef    = Test<agnes::reference    <T&&>&, const agnes::reference<U&&>&&>::value;
			static constexpr bool toRvRefToRvRefTuple      = Test<agnes::reference    <T&&>&,       std::tuple      <U&&>&&>::value;
			static constexpr bool toConstRvRefToRvRefTuple = Test<agnes::reference    <T&&>&, const std::tuple      <U&&>&&>::value;

			static_assert(builtin                  == Expected);
			static_assert(tuple                    == Expected);
			static_assert(toRvRefToAgRvRef         == Expected);
			static_assert(toConstRvRefToAgRvRef    == Expected);
			static_assert(toRvRefToRvRefTuple      == Expected);
			static_assert(toConstRvRefToRvRefTuple == Expected);

			static constexpr bool value =
				(builtin                  == Expected) &&
				(tuple                    == Expected) &&
				(toRvRefToAgRvRef         == Expected) &&
				(toConstRvRefToAgRvRef    == Expected) &&
				(toRvRefToRvRefTuple      == Expected) &&
				(toConstRvRefToRvRefTuple == Expected);
		};
	}

	template <typename T, typename U, bool Expected>
	struct is_reference_consistently_assignable :
		internal_::is_reference_consistently_assignable_impl<std::is_assignable, T, U, Expected> {};

	template <typename T, typename U, bool Expected>
	inline constexpr bool is_reference_consistently_assignable_v =
		is_reference_consistently_assignable<T, U, Expected>::value;

	template <typename T, typename U, bool Expected>
	struct is_reference_consistently_nothrow_assignable :
		internal_::is_reference_consistently_assignable_impl<std::is_nothrow_assignable, T, U, Expected> {};

	template <typename T, typename U, bool Expected>
	inline constexpr bool is_reference_consistently_nothrow_assignable_v =
		is_reference_consistently_nothrow_assignable<T, U, Expected>::value;
}
