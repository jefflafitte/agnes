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

#include "agnes_benchmarks/cuda/euler_particles_device_vector_of_structs.cuh"

#include <thrust/device_vector.h>

#include <nvbench/nvbench.cuh>

#include "agnes_benchmarks/agnes_particle_vector.h"

#include "agnes_benchmarks/cuda/agnes_particle_device_vector.cuh"
#include "agnes_benchmarks/cuda/euler_particles_kernel.cuh"

namespace agnes_benchmarks::cuda
{
	void euler_particles_round_trip_copy_agnes_vector2(nvbench::state& state)
	{
		const auto count           = static_cast<std::size_t>(state.get_int64("Count"));
		const auto threadsPerBlock = static_cast<std::size_t>(state.get_int64("ThreadsPerBlock"));
		const auto blocksPerGrid   = static_cast<unsigned int>((count + threadsPerBlock - 1)/threadsPerBlock);

		state.add_element_count(count, "Particles");

		state.add_global_memory_reads<float>(9*count, "H2D");
		state.add_global_memory_writes<float>(6*count, "D2H");

		auto hostParticles = agnes_pva_particle_vector_float(count);

		const auto hostParticlesData = hostParticles.data();
		const auto byteCount         = 9*hostParticles.capacity()*sizeof(float);

		cudaHostRegister(get<0>(hostParticlesData), byteCount, cudaHostRegisterDefault);

		auto deviceParticles = agnes_pva_particle_device_vector(count);

		const auto deviceParticlesData = deviceParticles.data();

		state.exec(
			nvbench::exec_tag::sync,
			[count, threadsPerBlock, blocksPerGrid, hostParticlesData, deviceParticlesData, byteCount](
				nvbench::launch& launch)
		{
			const auto& stream = launch.get_stream();

			cudaMemcpyAsync(
				get<0>(deviceParticlesData),
				get<0>(hostParticlesData),
				byteCount,
				cudaMemcpyHostToDevice,
				stream);

			euler_particles_kernel<<<blocksPerGrid, threadsPerBlock, 0, launch.get_stream()>>>(
				get<0>(deviceParticlesData),
				get<1>(deviceParticlesData),
				get<2>(deviceParticlesData),
				get<3>(deviceParticlesData),
				get<4>(deviceParticlesData),
				get<5>(deviceParticlesData),
				get<6>(deviceParticlesData),
				get<7>(deviceParticlesData),
				get<8>(deviceParticlesData),
				count,
				1.0f);

			cudaMemcpyAsync(
				get<0>(hostParticlesData),
				get<0>(deviceParticlesData),
				2*byteCount/3,
				cudaMemcpyDeviceToHost,
				stream);

			cudaStreamSynchronize(stream);
		});

		cudaHostUnregister(get<0>(hostParticlesData));
	}
}
