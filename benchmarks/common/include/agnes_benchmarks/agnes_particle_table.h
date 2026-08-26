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

#include <agnes/column.h>
#include <agnes/table.h>

namespace agnes_benchmarks
{
	AGNES_COLUMN_TEMPLATE(col_px, px); AGNES_COLUMN_TEMPLATE(col_py, py); AGNES_COLUMN_TEMPLATE(col_pz, pz);
	AGNES_COLUMN_TEMPLATE(col_vx, vx); AGNES_COLUMN_TEMPLATE(col_vy, vy); AGNES_COLUMN_TEMPLATE(col_vz, vz);
	AGNES_COLUMN_TEMPLATE(col_ax, ax); AGNES_COLUMN_TEMPLATE(col_ay, ay); AGNES_COLUMN_TEMPLATE(col_az, az);

	template <typename T>
	using agnes_particle_table = agnes::table<
		col_px<T>, col_py<T>, col_pz<T>,
		col_vx<T>, col_vy<T>, col_vz<T>,
		col_ax<T>, col_ay<T>, col_az<T>>;
}
