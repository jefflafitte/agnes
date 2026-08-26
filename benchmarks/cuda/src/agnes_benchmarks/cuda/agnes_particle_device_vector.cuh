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

#include <agnes/vector.h>

#include "agnes_benchmarks/cuda/agnes_device_allocator.cuh"

namespace agnes_benchmarks::cuda
{
	using agnes_pv_particle_device_allocator  = agnes_device_allocator<
		float, float, float,
		float, float, float>;

	using agnes_pva_particle_device_allocator = agnes_device_allocator<
		float, float, float,
		float, float, float,
		float, float, float>;

	using agnes_pv_particle_device_vector  = agnes::vector<
		float, float, float,
		float, float, float,
		agnes::allocate_with<agnes_pv_particle_device_allocator>>;

	using agnes_pva_particle_device_vector = agnes::vector<
		float, float, float,
		float, float, float,
		float, float, float,
		agnes::allocate_with<agnes_pva_particle_device_allocator>>;
}
