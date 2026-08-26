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

#include "agnes_benchmarks/cuda/particle_struct_of_device_vectors.cuh"

namespace agnes_benchmarks::cuda
{
	pv_particle_struct_of_device_vectors::pv_particle_struct_of_device_vectors(const std::size_t size) :
		px(size), py(size), pz(size),
		vx(size), vy(size), vz(size) {}

	std::size_t pv_particle_struct_of_device_vectors::size() const { return px.size(); }

	pva_particle_struct_of_device_vectors::pva_particle_struct_of_device_vectors(const std::size_t size) :
		pv_particle_struct_of_device_vectors{size},
		ax(size), ay(size), az(size) {}
}
