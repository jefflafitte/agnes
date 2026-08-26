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
#include "agnes_benchmarks/particle_struct_of_arrays.h"

#include "agnes_benchmarks/cpu/euler_fixture.h"
#include "agnes_benchmarks/cpu/vectorization.h"

namespace agnes_benchmarks::cpu
{
	template <typename T>
	class particle_struct_of_arrays_fixture :
		public euler_fixture<agnes_benchmarks::particle_struct_of_arrays<T>, T> {};

	BENCHMARK_TEMPLATE_METHOD_F(
		particle_struct_of_arrays_fixture,
		euler_struct_of_arrays_iterated)(benchmark::State& state)
	{
		auto& particles = this->get_container();

		const auto count = particles.size();

		for (auto _ : state)
		{
			benchmark::DoNotOptimize(particles.px.get());
			benchmark::DoNotOptimize(particles.py.get());
			benchmark::DoNotOptimize(particles.pz.get());
			benchmark::DoNotOptimize(particles.vx.get());
			benchmark::DoNotOptimize(particles.vy.get());
			benchmark::DoNotOptimize(particles.vz.get());

			auto*       ppx = particles.px.get();
			auto*       ppy = particles.py.get();
			auto*       ppz = particles.pz.get();
			auto*       pvx = particles.vx.get();
			auto*       pvy = particles.vy.get();
			auto*       pvz = particles.vz.get();
			const auto* pax = particles.ax.get();
			const auto* pay = particles.ay.get();
			const auto* paz = particles.az.get();

			const auto* const pxEnd = ppx + count;

			AGNES_COND_DISABLE_VECTORIZATION

			while (ppx != pxEnd)
			{
				euler(*ppx, *ppy, *ppz, *pvx, *pvy, *pvz, *pax, *pay, *paz, Base::time);

				++ppx; ++ppy; ++ppz; ++pvx; ++pvy; ++pvz; ++pax; ++pay; ++paz;
			}

			benchmark::ClobberMemory();
		}
	}

	BENCHMARK_TEMPLATE_METHOD_F(
		particle_struct_of_arrays_fixture,
		euler_struct_of_arrays_indexed)(benchmark::State& state)
	{
		auto& particles = this->get_container();

		const auto count = particles.size();

		for (auto _ : state)
		{
			benchmark::DoNotOptimize(particles.px.get());
			benchmark::DoNotOptimize(particles.py.get());
			benchmark::DoNotOptimize(particles.pz.get());
			benchmark::DoNotOptimize(particles.vx.get());
			benchmark::DoNotOptimize(particles.vy.get());
			benchmark::DoNotOptimize(particles.vz.get());

			auto*       ppx = particles.px.get();
			auto*       ppy = particles.py.get();
			auto*       ppz = particles.pz.get();
			auto*       pvx = particles.vx.get();
			auto*       pvy = particles.vy.get();
			auto*       pvz = particles.vz.get();
			const auto* pax = particles.ax.get();
			const auto* pay = particles.ay.get();
			const auto* paz = particles.az.get();

			AGNES_COND_DISABLE_VECTORIZATION

			for (auto i = std::size_t{0}; i < count; ++i)
			{
				euler(ppx[i], ppy[i], ppz[i], pvx[i], pvy[i], pvz[i], pax[i], pay[i], paz[i], Base::time);
			}

			benchmark::ClobberMemory();
		}
	}

	BENCHMARK_TEMPLATE_METHOD_F(
		particle_struct_of_arrays_fixture,
		euler_struct_of_arrays_restricted)(benchmark::State& state)
	{
		auto& particles = this->get_container();

		const auto count = particles.size();

		for (auto _ : state)
		{
			benchmark::DoNotOptimize(particles.px.get());
			benchmark::DoNotOptimize(particles.py.get());
			benchmark::DoNotOptimize(particles.pz.get());
			benchmark::DoNotOptimize(particles.vx.get());
			benchmark::DoNotOptimize(particles.vy.get());
			benchmark::DoNotOptimize(particles.vz.get());

			auto* const       AGNES_RESTRICT ppx = particles.px.get();
			auto* const       AGNES_RESTRICT ppy = particles.py.get();
			auto* const       AGNES_RESTRICT ppz = particles.pz.get();
			auto* const       AGNES_RESTRICT pvx = particles.vx.get();
			auto* const       AGNES_RESTRICT pvy = particles.vy.get();
			auto* const       AGNES_RESTRICT pvz = particles.vz.get();
			const auto* const AGNES_RESTRICT pax = particles.ax.get();
			const auto* const AGNES_RESTRICT pay = particles.ay.get();
			const auto* const AGNES_RESTRICT paz = particles.az.get();

			AGNES_COND_DISABLE_VECTORIZATION

			for (auto i = std::size_t{0}; i < count; ++i)
			{
				euler(ppx[i], ppy[i], ppz[i], pvx[i], pvy[i], pvz[i], pax[i], pay[i], paz[i], Base::time);
			}

			benchmark::ClobberMemory();
		}
	}
}
