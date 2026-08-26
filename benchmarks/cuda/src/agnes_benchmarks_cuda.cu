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

#include <nvbench/nvbench.cuh>

#include"agnes_benchmarks/cuda/euler_particles_agnes_device_vector.cuh"
#include"agnes_benchmarks/cuda/euler_particles_device_vector_of_structs.cuh"
#include"agnes_benchmarks/cuda/euler_particles_round_trip_copy_agnes_vector.cuh"
#include"agnes_benchmarks/cuda/euler_particles_round_trip_copy_agnes_vector2.cuh"
#include"agnes_benchmarks/cuda/euler_particles_round_trip_copy_struct_of_vectors.cuh"
#include"agnes_benchmarks/cuda/euler_particles_round_trip_copy_vector_of_structs.cuh"
#include"agnes_benchmarks/cuda/euler_particles_struct_of_device_vectors.cuh"

using namespace agnes_benchmarks::cuda;

NVBENCH_BENCH(euler_particles_device_vector_of_structs).add_int64_axis("Count", {10'000'000}).add_int64_axis("ThreadsPerBlock", {128});
NVBENCH_BENCH(euler_particles_struct_of_device_vectors).add_int64_axis("Count", {10'000'000}).add_int64_axis("ThreadsPerBlock", {128});
NVBENCH_BENCH(euler_particles_agnes_device_vector     ).add_int64_axis("Count", {10'000'000}).add_int64_axis("ThreadsPerBlock", {128});

NVBENCH_BENCH(euler_particles_round_trip_copy_vector_of_structs).add_int64_axis("Count", {10'000'000}).add_int64_axis("ThreadsPerBlock", {128});
NVBENCH_BENCH(euler_particles_round_trip_copy_struct_of_vectors).add_int64_axis("Count", {10'000'000}).add_int64_axis("ThreadsPerBlock", {128});
NVBENCH_BENCH(euler_particles_round_trip_copy_agnes_vector     ).add_int64_axis("Count", {10'000'000}).add_int64_axis("ThreadsPerBlock", {128});
NVBENCH_BENCH(euler_particles_round_trip_copy_agnes_vector2    ).add_int64_axis("Count", {10'000'000}).add_int64_axis("ThreadsPerBlock", {128});
