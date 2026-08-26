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

#include "agnes_benchmarks/agnes_particle_vector.h"
#include "agnes_benchmarks/euler.h"

#include "agnes_benchmarks/cpu/euler_fixture.h"
#include "agnes_benchmarks/cpu/vectorization.h"

namespace agnes_benchmarks::cpu
{
	template <typename T>
	class agnes_particle_vector_fixture :
		public euler_fixture<agnes_benchmarks::agnes_particle_vector<T>, T> {};

	BENCHMARK_TEMPLATE_METHOD_F(
		agnes_particle_vector_fixture,
		euler_agnes_vector_range)(benchmark::State& state)
	{
		auto& particles = this->get_container();

		for (auto _ : state)
		{
			benchmark::DoNotOptimize(particles.data());

			AGNES_COND_DISABLE_RANGE_BASED_VECTORIZATION

			for (const auto& particle : particles)
			{
				euler(
					get<0>(particle),
					get<1>(particle),
					get<2>(particle),
					get<3>(particle),
					get<4>(particle),
					get<5>(particle),
					get<6>(particle),
					get<7>(particle),
					get<8>(particle),
					Base::time);
			}

			benchmark::ClobberMemory();
		}
	}

	BENCHMARK_TEMPLATE_METHOD_F(
		agnes_particle_vector_fixture,
		euler_agnes_vector_iterated)(benchmark::State& state)
	{
		auto& particles = this->get_container();

		for (auto _ : state)
		{
			benchmark::DoNotOptimize(particles.data());

			AGNES_COND_DISABLE_VECTORIZATION

			for (auto iter = particles.begin(); iter != particles.end(); ++iter)
			{
				euler(
					*get<0>(iter),
					*get<1>(iter),
					*get<2>(iter),
					*get<3>(iter),
					*get<4>(iter),
					*get<5>(iter),
					*get<6>(iter),
					*get<7>(iter),
					*get<8>(iter),
					Base::time);
			}

			benchmark::ClobberMemory();
		}
	}

	BENCHMARK_TEMPLATE_METHOD_F(
		agnes_particle_vector_fixture,
		euler_agnes_vector_indexed)(benchmark::State& state)
	{
		auto& particles = this->get_container();

		const auto count = particles.size();

		for (auto _ : state)
		{
			benchmark::DoNotOptimize(particles.data());

			AGNES_COND_DISABLE_VECTORIZATION

			for (auto i = std::size_t{0}; i < count; ++i)
			{
				const auto& particle = particles[i];

				euler(
					get<0>(particle),
					get<1>(particle),
					get<2>(particle),
					get<3>(particle),
					get<4>(particle),
					get<5>(particle),
					get<6>(particle),
					get<7>(particle),
					get<8>(particle),
					Base::time);
			}

			benchmark::ClobberMemory();
		}
	}

	BENCHMARK_TEMPLATE_METHOD_F(
		agnes_particle_vector_fixture,
		euler_agnes_vector_restricted)(benchmark::State& state)
	{
		auto& particles = this->get_container();

		const auto count = particles.size();

		for (auto _ : state)
		{
			benchmark::DoNotOptimize(particles.data());

			const auto p = particles.data();

			auto* const       AGNES_RESTRICT ppx = get<0>(p);
			auto* const       AGNES_RESTRICT ppy = get<1>(p);
			auto* const       AGNES_RESTRICT ppz = get<2>(p);
			auto* const       AGNES_RESTRICT pvx = get<3>(p);
			auto* const       AGNES_RESTRICT pvy = get<4>(p);
			auto* const       AGNES_RESTRICT pvz = get<5>(p);
			const auto* const AGNES_RESTRICT pax = get<6>(p);
			const auto* const AGNES_RESTRICT pay = get<7>(p);
			const auto* const AGNES_RESTRICT paz = get<8>(p);

			AGNES_COND_DISABLE_VECTORIZATION

			for (auto i = std::size_t{0}; i < count; ++i)
			{
				euler(ppx[i], ppy[i], ppz[i], pvx[i], pvy[i], pvz[i], pax[i], pay[i], paz[i], Base::time);
			}

			benchmark::ClobberMemory();
		}
	}
}
