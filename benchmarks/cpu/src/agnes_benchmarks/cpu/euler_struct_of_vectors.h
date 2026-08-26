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
#include "agnes_benchmarks/particle_struct_of_vectors.h"

#include "agnes_benchmarks/cpu/euler_fixture.h"
#include "agnes_benchmarks/cpu/vectorization.h"

namespace agnes_benchmarks::cpu
{
	template <typename T>
	class particle_struct_of_vectors_fixture :
		public euler_fixture<agnes_benchmarks::particle_struct_of_vectors<T>, T> {};

	BENCHMARK_TEMPLATE_METHOD_F(
		particle_struct_of_vectors_fixture,
		euler_struct_of_vectors_iterated)(benchmark::State& state)
	{
		auto& particles = this->get_container();

		for (auto _ : state)
		{
			benchmark::DoNotOptimize(particles.px.data());
			benchmark::DoNotOptimize(particles.py.data());
			benchmark::DoNotOptimize(particles.pz.data());
			benchmark::DoNotOptimize(particles.vx.data());
			benchmark::DoNotOptimize(particles.vy.data());
			benchmark::DoNotOptimize(particles.vz.data());

			auto pxIter = particles.px.begin();
			auto pyIter = particles.py.begin();
			auto pzIter = particles.pz.begin();
			auto vxIter = particles.vx.begin();
			auto vyIter = particles.vy.begin();
			auto vzIter = particles.vz.begin();
			auto axIter = particles.ax.cbegin();
			auto ayIter = particles.ay.cbegin();
			auto azIter = particles.az.cbegin();

			AGNES_COND_DISABLE_VECTORIZATION

			while (pxIter != particles.px.end())
			{
				euler(*pxIter, *pyIter, *pzIter, *vxIter, *vyIter, *vzIter, *axIter, *ayIter, *azIter, Base::time);

				++pxIter; ++pyIter; ++pzIter; ++vxIter; ++vyIter; ++vzIter; ++axIter; ++ayIter; ++azIter;
			}

			benchmark::ClobberMemory();
		}
	}

	BENCHMARK_TEMPLATE_METHOD_F(
		particle_struct_of_vectors_fixture,
		euler_struct_of_vectors_indexed)(benchmark::State& state)
	{
		auto& particles = this->get_container();

		const auto count = particles.size();

		for (auto _ : state)
		{
			benchmark::DoNotOptimize(particles.px.data());
			benchmark::DoNotOptimize(particles.py.data());
			benchmark::DoNotOptimize(particles.pz.data());
			benchmark::DoNotOptimize(particles.vx.data());
			benchmark::DoNotOptimize(particles.vy.data());
			benchmark::DoNotOptimize(particles.vz.data());

			AGNES_COND_DISABLE_VECTORIZATION

			for (auto i = std::size_t{0}; i < count; ++i)
			{
				euler(
					particles.px[i],
					particles.py[i],
					particles.pz[i],
					particles.vx[i],
					particles.vy[i],
					particles.vz[i],
					particles.ax[i],
					particles.ay[i],
					particles.az[i],
					Base::time);
			}

			benchmark::ClobberMemory();
		}
	}

	BENCHMARK_TEMPLATE_METHOD_F(
		particle_struct_of_vectors_fixture,
		euler_struct_of_vectors_restricted)(benchmark::State& state)
	{
		auto& particles = this->get_container();

		const auto count = particles.size();

		for (auto _ : state)
		{
			benchmark::DoNotOptimize(particles.px.data());
			benchmark::DoNotOptimize(particles.py.data());
			benchmark::DoNotOptimize(particles.pz.data());
			benchmark::DoNotOptimize(particles.vx.data());
			benchmark::DoNotOptimize(particles.vy.data());
			benchmark::DoNotOptimize(particles.vz.data());

			auto* const       AGNES_RESTRICT ppx = particles.px.data();
			auto* const       AGNES_RESTRICT ppy = particles.py.data();
			auto* const       AGNES_RESTRICT ppz = particles.pz.data();
			auto* const       AGNES_RESTRICT pvx = particles.vx.data();
			auto* const       AGNES_RESTRICT pvy = particles.vy.data();
			auto* const       AGNES_RESTRICT pvz = particles.vz.data();
			const auto* const AGNES_RESTRICT pax = particles.ax.data();
			const auto* const AGNES_RESTRICT pay = particles.ay.data();
			const auto* const AGNES_RESTRICT paz = particles.az.data();

			AGNES_COND_DISABLE_VECTORIZATION

			for (auto i = std::size_t{0}; i < count; ++i)
			{
				euler(ppx[i], ppy[i], ppz[i], pvx[i], pvy[i], pvz[i], pax[i], pay[i], paz[i], Base::time);
			}

			benchmark::ClobberMemory();
		}
	}
}
