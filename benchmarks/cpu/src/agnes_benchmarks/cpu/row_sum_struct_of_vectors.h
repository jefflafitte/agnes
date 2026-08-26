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

#include "agnes_benchmarks/row_struct_of_vectors.h"
#include "agnes_benchmarks/row_sum.h"

#include "agnes_benchmarks/cpu/row_sum_fixture.h"
#include "agnes_benchmarks/cpu/vectorization.h"

namespace agnes_benchmarks::cpu
{
	template <typename T, typename ColumnCount, typename Indexes>
	class row_struct_of_vectors_fixture :
		public row_sum_fixture<agnes_benchmarks::row_struct_of_vectors<T, ColumnCount::value>, T, Indexes> {};

	BENCHMARK_TEMPLATE_METHOD_F(
		row_struct_of_vectors_fixture,
		row_sum_struct_of_vectors_iterated)(benchmark::State& state)
	{
		[this, &state]<std::size_t... I, std::size_t... J>(std::index_sequence<I...>, std::index_sequence<J...>)
		{
			const auto& rows = this->get_container();

			for (auto _ : state)
			{
				decltype(rows.columns[0].begin()) iter[] = {rows.columns[I].begin()...};
				const auto                        end0   = rows.columns[0].end();

				AGNES_COND_DISABLE_RANGE_BASED_VECTORIZATION

				while (iter[0] != end0)
				{
					benchmark::DoNotOptimize(row_sum(*iter[J]...));

					(++iter[J], ...);
				}
			}
		}(Base::indexes, std::make_index_sequence<Base::indexes.size()>{});
	}

	BENCHMARK_TEMPLATE_METHOD_F(
		row_struct_of_vectors_fixture,
		row_sum_struct_of_vectors_indexed)(benchmark::State& state)
	{
		[this, &state]<std::size_t... I>(std::index_sequence<I...>)
		{
			const auto& rows = this->get_container();

			const auto count = rows.size();

			for (auto _ : state)
			{
				AGNES_COND_DISABLE_RANGE_BASED_VECTORIZATION

				for (auto i = std::size_t{0}; i < count; ++i)
				{
					benchmark::DoNotOptimize(row_sum(rows.columns[I][i]...));
				}
			}
		}(Base::indexes);
	}

	BENCHMARK_TEMPLATE_METHOD_F(
		row_struct_of_vectors_fixture,
		row_sum_struct_of_vectors_restricted)(benchmark::State& state)
	{
		[this, &state]<std::size_t... I, std::size_t... J>(std::index_sequence<I...>, std::index_sequence<J...>)
		{
			const auto& rows = this->get_container();

			const auto count = rows.size();

			for (auto _ : state)
			{
				const typename Base::value_type* const AGNES_RESTRICT p[] = {rows.columns[I].data()...};

				AGNES_COND_DISABLE_RANGE_BASED_VECTORIZATION

				for (auto i = std::size_t{0}; i < count; ++i)
				{
					benchmark::DoNotOptimize(row_sum(p[J][i]...));
				}
			}
		}(Base::indexes, std::make_index_sequence<Base::indexes.size()>{});
	}
}
