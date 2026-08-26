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

#include "agnes_benchmarks/device.h"

namespace agnes_benchmarks
{
	template <typename T>
	AGNES_HOST AGNES_DEVICE void euler(
		T&      p,
		const T v,
		const T t)
	{
		p += v*t;
	}

	template <typename T>
	AGNES_HOST AGNES_DEVICE void euler(
		T&      p,
		T&      v,
		const T a,
		const T t)
	{
		v += a*t;
		p += v*t;
	}

	template <typename T>
	AGNES_HOST AGNES_DEVICE void euler(
		T&      px,
		T&      py,
		T&      pz,
		const T vx,
		const T vy,
		const T vz,
		const T t)
	{
		euler(px, vx, t);
		euler(py, vy, t);
		euler(pz, vz, t);
	}

	template <typename T>
	AGNES_HOST AGNES_DEVICE void euler(
		T&      px,
		T&      py,
		T&      pz,
		T&      vx,
		T&      vy,
		T&      vz,
		const T ax,
		const T ay,
		const T az,
		const T t)
	{
		euler(px, vx, ax, t);
		euler(py, vy, ay, t);
		euler(pz, vz, az, t);
	}
}
