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
#include <tuple>

#include <agnes/fields.h>
#include <agnes/pointer.h>
#include <agnes/reference.h>

namespace agnes_tests
{
	template <typename... T>
	struct user_defined_pointer
	{
		using element_type    = agnes::fields<T...>;
		using difference_type = std::ptrdiff_t;

		template <typename... U> using rebind = user_defined_pointer<U...>;

		static constexpr user_defined_pointer pointer_to(const agnes::reference<T&...>& r)
		{ return user_defined_pointer{agnes::addressof(r)}; }

		agnes::pointer<T*...> p;
	};

	template <typename... T>
	bool operator==(const user_defined_pointer<T...>& lhs, const user_defined_pointer<T...>& rhs)
	{ return lhs.p == rhs.p; }

	template <typename... T>
	struct user_defined_pointer_no_defs
	{
		static constexpr user_defined_pointer_no_defs pointer_to(const agnes::reference<T&...>& r)
		{ return user_defined_pointer_no_defs{agnes::addressof(r)}; }

		agnes::pointer<T*...> p;
	};

	template <typename... T>
	bool operator==(const user_defined_pointer_no_defs<T...>& lhs, const user_defined_pointer_no_defs<T...>& rhs)
	{ return lhs.p == rhs.p; }
}
