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

#include "agnes_benchmarks/cuda/euler_particles_struct_of_device_vectors.cuh"

#include <nvbench/nvbench.cuh>

#include "agnes_benchmarks/cuda/euler_particles_kernel.cuh"
#include "agnes_benchmarks/cuda/particle_struct_of_device_vectors.cuh"

namespace agnes_benchmarks::cuda
{
	void euler_particles_struct_of_device_vectors(nvbench::state& state)
	{
		const auto count           = static_cast<std::size_t>(state.get_int64("Count"));
		const auto threadsPerBlock = static_cast<std::size_t>(state.get_int64("ThreadsPerBlock"));
		const auto blocksPerGrid   = static_cast<unsigned int>((count + threadsPerBlock - 1)/threadsPerBlock);

		state.add_element_count(count, "Particles");

		state.add_global_memory_reads<float>(9*count);
		state.add_global_memory_writes<float>(6*count);

		auto particles = pva_particle_struct_of_device_vectors{count};

		state.exec([count, threadsPerBlock, blocksPerGrid, &particles](nvbench::launch& launch)
		{
			euler_particles_kernel<<<blocksPerGrid, threadsPerBlock, 0, launch.get_stream()>>>(
				particles.px.data().get(), particles.py.data().get(), particles.pz.data().get(),
				particles.vx.data().get(), particles.vy.data().get(), particles.vz.data().get(),
				particles.ax.data().get(), particles.ay.data().get(), particles.az.data().get(),
				count,
				1.0f);
		});
	}
}
