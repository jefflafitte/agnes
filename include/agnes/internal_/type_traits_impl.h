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

#include <type_traits>

namespace agnes
{
	template <typename...> class pointer;
	template <typename...> class reference;
}

namespace agnes::internal_
{
	template <typename> struct is_pointer_impl_impl : std::false_type {};

	template <typename... T> struct is_pointer_impl_impl<pointer<T*...>> : std::true_type {};

	template <typename T> struct is_pointer_impl : is_pointer_impl_impl<std::remove_cvref_t<T>> {};

	template <typename> struct is_reference_impl_impl : std::false_type {};

	template <typename... T> struct is_reference_impl_impl<reference<T&...>> : std::true_type {};

	template <typename... T> struct is_reference_impl_impl<reference<T&&...>> : std::true_type {};

	template <typename T> struct is_reference_impl : is_reference_impl_impl<std::remove_cvref_t<T>> {};

	template <typename> struct make_void;

	template <template <typename...> typename T, typename... U>
	struct make_void<T<U...>> : std::type_identity<T<std::void_t<U>...>> {};
}
