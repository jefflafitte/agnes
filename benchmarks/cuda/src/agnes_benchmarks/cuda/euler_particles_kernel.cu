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

#include "agnes_benchmarks/cuda/euler_particles_kernel.cuh"

#include "agnes_benchmarks/euler.h"

namespace agnes_benchmarks::cuda
{
	__global__ void euler_particles_kernel(pv_particle_float* const particles, const std::size_t count, const float t)
	{
		const auto i = blockIdx.x*blockDim.x + threadIdx.x;

		if (i < count)
		{
			auto& particle = particles[i];

			euler(
				particle.px, particle.py, particle.pz,
				particle.vx, particle.vy, particle.vz,
				t);
		}
	}

	__global__ void euler_particles_kernel(pva_particle_float* const particles, const std::size_t count, const float t)
	{
		const auto i = blockIdx.x*blockDim.x + threadIdx.x;

		if (i < count)
		{
			auto& particle = particles[i];

			euler(
				particle.px, particle.py, particle.pz,
				particle.vx, particle.vy, particle.vz,
				particle.ax, particle.ay, particle.az,
				t);
		}
	}

	__global__ void euler_particles_kernel(
		float* const       ppx, float* const       ppy, float* const       ppz,
		const float* const pvx, const float* const pvy, const float* const pvz,
		const std::size_t count,
		const float       t)
	{
		const auto i = blockIdx.x*blockDim.x + threadIdx.x;

		if (i < count)
		{
			euler(ppx[i], ppy[i], ppz[i], pvx[i], pvy[i], pvz[i], t);
		}
	}

	__global__ void euler_particles_kernel(
		float* const       ppx, float* const       ppy, float* const       ppz,
		float* const       pvx, float* const       pvy, float* const       pvz,
		const float* const pax, const float* const pay, const float* const paz,
		const std::size_t count,
		const float       t)
	{
		const auto i = blockIdx.x*blockDim.x + threadIdx.x;

		if (i < count)
		{
			euler(ppx[i], ppy[i], ppz[i], pvx[i], pvy[i], pvz[i], pax[i], pay[i], paz[i], t);
		}
	}
}
