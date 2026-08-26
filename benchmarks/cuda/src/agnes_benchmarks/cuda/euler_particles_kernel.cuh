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

#include "agnes_benchmarks/particle.h"

namespace agnes_benchmarks::cuda
{
	__global__ void euler_particles_kernel(pv_particle_float* particle, std::size_t count, float t);

	__global__ void euler_particles_kernel(pva_particle_float* particle, std::size_t count, float t);

	__global__ void euler_particles_kernel(
		float*       ppx, float*       ppy, float*       ppz,
		const float* pvx, const float* pvy, const float* pvz,
		std::size_t count,
		float       t);

	__global__ void euler_particles_kernel(
		float*       ppx, float*       ppy, float*       ppz,
		float*       pvx, float*       pvy, float*       pvz,
		const float* pax, const float* pay, const float* paz,
		std::size_t count,
		float       t);
}
