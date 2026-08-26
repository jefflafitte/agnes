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

#include <type_traits>

#include <agnes/internal_/type_traits_impl.h>

namespace agnes
{
	/// Provides a member constant `value` equal to `true` if
	/// T is an agnes::pointer.
	template <typename T> struct is_pointer : internal_::is_pointer_impl<T> {};

	/// Provides a member constant `value` equal to `true` if
	/// T is an agnes::pointer.
	template <typename T> struct is_reference : internal_::is_reference_impl<T> {};

	template <typename> struct remove_cv;

	/// Provides a member typedef `type` that names the
	/// type `T<std::remove_cv_t<U>...>`.
	template <template <typename...> typename T, typename... U>
	struct remove_cv<T<U...>> : std::type_identity<T<std::remove_cv_t<U>...>> {};

	template <typename> struct add_const;

	/// Provides a member typedef `type` that names the
	/// type `T<std::add_const_t<U>...>`.
	template <template <typename...> typename T, typename... U>
	struct add_const<T<U...>> : std::type_identity<T<std::add_const_t<U>...>> {};

	/// Helper type that is an alias for `typename remove_cv<T>::type`.
	template <typename T> using remove_cv_t = typename remove_cv<T>::type;

	/// Helper type that is an alias for `typename add_const<T>::type`.
	template <typename T> using add_const_t = typename add_const<T>::type;

	/// Maps a class template `T<U...>` to the type `T<void...>`.
	template <typename T> using void_t = typename internal_::make_void<T>::type;

	/// Constant equal to `is_pointer<T>::value`.
	template <typename T> inline constexpr bool is_pointer_v = is_pointer<T>::value;

	/// Constant equal to `is_reference<T>::value`.
	template <typename T> inline constexpr bool is_reference_v = is_reference<T>::value;
}
