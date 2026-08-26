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

#include <benchmark/benchmark.h>

#include "agnes_benchmarks/euler.h"
#include "agnes_benchmarks/particle_vector.h"

#include "agnes_benchmarks/cpu/euler_fixture.h"
#include "agnes_benchmarks/cpu/vectorization.h"

namespace agnes_benchmarks::cpu
{
	template <typename T>
	class particle_vector_fixture :
		public euler_fixture<agnes_benchmarks::particle_vector<T>, T> {};

	BENCHMARK_TEMPLATE_METHOD_F(
		particle_vector_fixture,
		euler_vector_of_structs_range)(benchmark::State& state)
	{
		auto& particles = this->get_container();

		for (auto _ : state)
		{
			benchmark::DoNotOptimize(particles.data());

			AGNES_COND_DISABLE_RANGE_BASED_VECTORIZATION

			for (auto& particle : particles)
			{
				euler(
					particle.px,
					particle.py,
					particle.pz,
					particle.vx,
					particle.vy,
					particle.vz,
					particle.ax,
					particle.ay,
					particle.az,
					Base::time);
			}

			benchmark::ClobberMemory();
		}
	}

	BENCHMARK_TEMPLATE_METHOD_F(
		particle_vector_fixture,
		euler_vector_of_structs_iterated)(benchmark::State& state)
	{
		auto& particles = this->get_container();

		for (auto _ : state)
		{
			benchmark::DoNotOptimize(particles.data());

			AGNES_COND_DISABLE_VECTORIZATION

			for (auto iter = particles.begin(); iter != particles.end(); ++iter)
			{
				euler(
					iter->px,
					iter->py,
					iter->pz,
					iter->vx,
					iter->vy,
					iter->vz,
					iter->ax,
					iter->ay,
					iter->az,
					Base::time);
			}

			benchmark::ClobberMemory();
		}
	}

	BENCHMARK_TEMPLATE_METHOD_F(
		particle_vector_fixture,
		euler_vector_of_structs_indexed)(benchmark::State& state)
	{
		auto& particles = this->get_container();

		const auto count = particles.size();

		for (auto _ : state)
		{
			benchmark::DoNotOptimize(particles.data());

			AGNES_COND_DISABLE_VECTORIZATION

			for (auto i = std::size_t{0}; i < count; ++i)
			{
				auto& particle = particles[i];

				euler(
					particle.px,
					particle.py,
					particle.pz,
					particle.vx,
					particle.vy,
					particle.vz,
					particle.ax,
					particle.ay,
					particle.az,
					Base::time);
			}

			benchmark::ClobberMemory();
		}
	}

	BENCHMARK_TEMPLATE_METHOD_F(
		particle_vector_fixture,
		euler_vector_of_structs_restricted)(benchmark::State& state)
	{
		auto& particles = this->get_container();

		const auto count = particles.size();

		for (auto _ : state)
		{
			benchmark::DoNotOptimize(particles.data());

			auto* const AGNES_RESTRICT p = particles.data();

			AGNES_COND_DISABLE_VECTORIZATION

			for (auto i = std::size_t{0}; i < count; ++i)
			{
				auto& particle = p[i];

				euler(
					particle.px,
					particle.py,
					particle.pz,
					particle.vx,
					particle.vy,
					particle.vz,
					particle.ax,
					particle.ay,
					particle.az,
					Base::time);
			}

			benchmark::ClobberMemory();
		}
	}
}
