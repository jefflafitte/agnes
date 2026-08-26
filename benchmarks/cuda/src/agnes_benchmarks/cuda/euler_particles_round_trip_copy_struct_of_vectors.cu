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

#include "agnes_benchmarks/cuda/euler_particles_round_trip_copy_struct_of_vectors.cuh"

#include <nvbench/nvbench.cuh>

#include "agnes_benchmarks/particle_struct_of_vectors.h"

#include "agnes_benchmarks/cuda/euler_particles_kernel.cuh"
#include "agnes_benchmarks/cuda/particle_struct_of_device_vectors.cuh"

namespace agnes_benchmarks::cuda
{
	void euler_particles_round_trip_copy_struct_of_vectors(nvbench::state& state)
	{
		const auto count           = static_cast<std::size_t>(state.get_int64("Count"));
		const auto threadsPerBlock = static_cast<std::size_t>(state.get_int64("ThreadsPerBlock"));
		const auto blocksPerGrid   = static_cast<unsigned int>((count + threadsPerBlock - 1)/threadsPerBlock);

		state.add_element_count(count, "Particles");

		state.add_global_memory_reads<float>(9*count, "H2D");
		state.add_global_memory_writes<float>(6*count, "D2H");

		auto hostParticles = pva_particle_struct_of_vectors_float{count};

		cudaHostRegister(hostParticles.px.data(), count*sizeof(float), cudaHostRegisterDefault);
		cudaHostRegister(hostParticles.py.data(), count*sizeof(float), cudaHostRegisterDefault);
		cudaHostRegister(hostParticles.pz.data(), count*sizeof(float), cudaHostRegisterDefault);
		cudaHostRegister(hostParticles.vx.data(), count*sizeof(float), cudaHostRegisterDefault);
		cudaHostRegister(hostParticles.vy.data(), count*sizeof(float), cudaHostRegisterDefault);
		cudaHostRegister(hostParticles.vz.data(), count*sizeof(float), cudaHostRegisterDefault);
		cudaHostRegister(hostParticles.ax.data(), count*sizeof(float), cudaHostRegisterDefault);
		cudaHostRegister(hostParticles.ay.data(), count*sizeof(float), cudaHostRegisterDefault);
		cudaHostRegister(hostParticles.az.data(), count*sizeof(float), cudaHostRegisterDefault);

		auto deviceParticles = pva_particle_struct_of_device_vectors{count};

		state.exec(
			nvbench::exec_tag::sync,
			[count, threadsPerBlock, blocksPerGrid, &hostParticles, &deviceParticles](nvbench::launch& launch)
		{
			const auto& stream = launch.get_stream();

			const auto byteCount = count*sizeof(float);

			{
				const auto copyHostToDevice = [&stream, byteCount](auto& deviceVector, const auto& hostVector)
				{
					cudaMemcpyAsync(
						deviceVector.data().get(),
						hostVector.data(),
						byteCount,
						cudaMemcpyHostToDevice,
						stream);
				};

				copyHostToDevice(deviceParticles.px, hostParticles.px);
				copyHostToDevice(deviceParticles.px, hostParticles.py);
				copyHostToDevice(deviceParticles.px, hostParticles.pz);
				copyHostToDevice(deviceParticles.vx, hostParticles.vx);
				copyHostToDevice(deviceParticles.vy, hostParticles.vy);
				copyHostToDevice(deviceParticles.vz, hostParticles.vz);
				copyHostToDevice(deviceParticles.ax, hostParticles.ax);
				copyHostToDevice(deviceParticles.ay, hostParticles.ay);
				copyHostToDevice(deviceParticles.az, hostParticles.az);
			}

			euler_particles_kernel<<<blocksPerGrid, threadsPerBlock, 0, launch.get_stream()>>>(
				deviceParticles.px.data().get(),
				deviceParticles.py.data().get(),
				deviceParticles.pz.data().get(),
				deviceParticles.vx.data().get(),
				deviceParticles.vy.data().get(),
				deviceParticles.vz.data().get(),
				deviceParticles.ax.data().get(),
				deviceParticles.ay.data().get(),
				deviceParticles.az.data().get(),
				count,
				1.0f);

			{
				const auto copyDeviceToHost = [&stream, byteCount](auto& hostVector, const auto& deviceVector)
				{
					cudaMemcpyAsync(
						hostVector.data(),
						deviceVector.data().get(),
						byteCount,
						cudaMemcpyDeviceToHost,
						stream);
				};

				copyDeviceToHost(hostParticles.px, deviceParticles.px);
				copyDeviceToHost(hostParticles.py, deviceParticles.py);
				copyDeviceToHost(hostParticles.pz, deviceParticles.pz);
				copyDeviceToHost(hostParticles.vx, deviceParticles.vx);
				copyDeviceToHost(hostParticles.vy, deviceParticles.vy);
				copyDeviceToHost(hostParticles.vz, deviceParticles.vz);
			}

			cudaStreamSynchronize(stream);
		});

		cudaHostUnregister(hostParticles.px.data());
		cudaHostUnregister(hostParticles.py.data());
		cudaHostUnregister(hostParticles.pz.data());
		cudaHostUnregister(hostParticles.vx.data());
		cudaHostUnregister(hostParticles.vy.data());
		cudaHostUnregister(hostParticles.vz.data());
		cudaHostUnregister(hostParticles.ax.data());
		cudaHostUnregister(hostParticles.ay.data());
		cudaHostUnregister(hostParticles.az.data());
	}
}
