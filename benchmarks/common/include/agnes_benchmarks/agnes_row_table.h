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
#include <type_traits>

#include <agnes/column.h>
#include <agnes/table.h>

namespace agnes_benchmarks
{
	AGNES_COLUMN_TEMPLATE(col_00, col00);
	AGNES_COLUMN_TEMPLATE(col_01, col01);
	AGNES_COLUMN_TEMPLATE(col_02, col02);
	AGNES_COLUMN_TEMPLATE(col_03, col03);
	AGNES_COLUMN_TEMPLATE(col_04, col04);
	AGNES_COLUMN_TEMPLATE(col_05, col05);
	AGNES_COLUMN_TEMPLATE(col_06, col06);
	AGNES_COLUMN_TEMPLATE(col_07, col07);
	AGNES_COLUMN_TEMPLATE(col_08, col08);
	AGNES_COLUMN_TEMPLATE(col_09, col09);
	AGNES_COLUMN_TEMPLATE(col_10, col10);
	AGNES_COLUMN_TEMPLATE(col_11, col11);
	AGNES_COLUMN_TEMPLATE(col_12, col12);
	AGNES_COLUMN_TEMPLATE(col_13, col13);
	AGNES_COLUMN_TEMPLATE(col_14, col14);
	AGNES_COLUMN_TEMPLATE(col_15, col15);

	namespace detail
	{
		template <typename T>
		using columns = std::tuple<
			col_00<T>, col_01<T>, col_02<T>, col_03<T>, col_04<T>, col_05<T>, col_06<T>, col_07<T>,
			col_08<T>, col_09<T>, col_10<T>, col_11<T>, col_12<T>, col_13<T>, col_14<T>, col_15<T>>;

		template <typename, typename, std::size_t ColumnCount>
		struct make_agnes_row_table_impl;

		template <typename... Columns, typename T>
		struct make_agnes_row_table_impl<agnes::table<Columns...>, T, 0> :
			std::type_identity<agnes::table<T...>> {};

		template <typename... Columns, typename T, std::size_t ColumnCount>
		struct make_agnes_row_table_impl<agnes::table<Columns...>, T, ColumnCount> : make_agnes_row_table_impl<
			agnes::table<std::tuple_element_t<ColumnCount - 1, columns<T>>, Columns...>,
			T,
			ColumnCount - 1> {};

		template <typename T, std::size_t ColumnCount>
		struct make_agnes_row_table : make_agnes_row_table_impl<agnes::table<>, T, ColumnCount> {};
	}

	template <typename T, std::size_t ColumnCount>
	using agnes_row_table = typename detail::make_agnes_row_table<T, ColumnCount>::type;
}
