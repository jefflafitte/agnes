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

#include "agnes_benchmarks/cuda/euler_particles_agnes_device_vector.cuh"

#include <nvbench/nvbench.cuh>

#include "agnes_benchmarks/cuda/agnes_particle_device_vector.cuh"
#include "agnes_benchmarks/cuda/euler_particles_kernel.cuh"

namespace agnes_benchmarks::cuda
{
	void euler_particles_agnes_device_vector(nvbench::state& state)
	{
		const auto count           = static_cast<std::size_t>(state.get_int64("Count"));
		const auto threadsPerBlock = static_cast<std::size_t>(state.get_int64("ThreadsPerBlock"));
		const auto blocksPerGrid   = static_cast<unsigned int>((count + threadsPerBlock - 1)/threadsPerBlock);

		state.add_element_count(count, "Particles");

		state.add_global_memory_reads<float>(9*count);
		state.add_global_memory_writes<float>(6*count);

		auto particles = agnes_pva_particle_device_vector(count);

		const auto particlesData = particles.data();

		state.exec([count, threadsPerBlock, blocksPerGrid, particlesData](nvbench::launch& launch)
		{
			euler_particles_kernel<<<blocksPerGrid, threadsPerBlock, 0, launch.get_stream()>>>(
				get<0>(particlesData),
				get<1>(particlesData),
				get<2>(particlesData),
				get<3>(particlesData),
				get<4>(particlesData),
				get<5>(particlesData),
				get<6>(particlesData),
				get<7>(particlesData),
				get<8>(particlesData),
				count,
				1.0f);
		});
	}
}
