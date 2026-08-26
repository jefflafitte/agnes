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
#include <type_traits>

namespace agnes
{
	template <typename...> class reference;
	template <typename...> struct fields;
}

namespace agnes::internal_
{
	template <typename Ptr> struct pointer_traits_element_type;

	template <template <typename...> typename Ptr, typename... T>
	struct pointer_traits_element_type<Ptr<T...>> :
		std::type_identity<fields<std::remove_pointer_t<T>...>> {};

	template <typename Ptr>
		requires requires { typename Ptr::element_type; }
	struct pointer_traits_element_type<Ptr> :
		std::type_identity<typename Ptr::element_type> {};

	template <typename Ptr>
	struct pointer_traits_difference_type :
		std::type_identity<std::ptrdiff_t> {};

	template <typename Ptr>
		requires requires { typename Ptr::difference_type; }
	struct pointer_traits_difference_type<Ptr> :
		std::type_identity<typename Ptr::difference_type> {};

	template <typename Ptr> struct pointer_traits_reference_type;

	template <template <typename...> typename Ptr, typename... T>
	struct pointer_traits_reference_type<Ptr<T...>> :
		std::type_identity<agnes::reference<std::remove_pointer_t<T>&...>> {};

	template <typename Ptr>
		requires requires { typename Ptr::reference_type; }
	struct pointer_traits_reference_type<Ptr> :
		std::type_identity<typename Ptr::reference_type> {};

	template <typename Ptr, typename... U> struct pointer_traits_rebind_type;

	template <template <typename...> typename Ptr, typename... T, typename... U>
	struct pointer_traits_rebind_type<Ptr<T...>, U...> :
		std::type_identity<Ptr<U...>> {};

	template <typename Ptr, typename... U>
		requires requires { typename Ptr::template rebind<U...>; }
	struct pointer_traits_rebind_type<Ptr, U...> :
		std::type_identity<typename Ptr::template rebind<U...>> {};

	template <typename Ptr>
	using pointer_traits_element_type_t = typename pointer_traits_element_type<Ptr>::type;

	template <typename Ptr>
	using pointer_traits_difference_type_t = typename pointer_traits_difference_type<Ptr>::type;

	template <typename Ptr>
	using pointer_traits_reference_type_t = typename pointer_traits_reference_type<Ptr>::type;

	template <typename Ptr, typename... U>
	using pointer_traits_rebind_type_t = typename pointer_traits_rebind_type<Ptr, U...>::type;
}
