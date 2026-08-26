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

#include <thrust/device_vector.h>

namespace agnes_benchmarks::cuda
{
	using float_device_vector    = thrust::device_vector<float>;

	struct pv_particle_struct_of_device_vectors
	{
		pv_particle_struct_of_device_vectors(std::size_t size);

		std::size_t size() const;

		float_device_vector px; float_device_vector py; float_device_vector pz;
		float_device_vector vx; float_device_vector vy; float_device_vector vz;
	};

	struct pva_particle_struct_of_device_vectors : pv_particle_struct_of_device_vectors
	{
		pva_particle_struct_of_device_vectors(std::size_t size);

		float_device_vector ax; float_device_vector ay; float_device_vector az;
	};
}
