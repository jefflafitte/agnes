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

#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

#include <agnes/cache.h>

#include <benchmark/benchmark.h>

#include "agnes_benchmarks/cache_configuration.h"

#include "agnes_benchmarks/cpu/euler_agnes_table.h"
#include "agnes_benchmarks/cpu/euler_agnes_vector.h"
#include "agnes_benchmarks/cpu/euler_struct_of_arrays.h"
#include "agnes_benchmarks/cpu/euler_struct_of_vectors.h"
#include "agnes_benchmarks/cpu/euler_vector_of_structs.h"
#include "agnes_benchmarks/cpu/row_sum_agnes_vector.h"
#include "agnes_benchmarks/cpu/row_sum_struct_of_arrays.h"
#include "agnes_benchmarks/cpu/row_sum_struct_of_vectors.h"
#include "agnes_benchmarks/cpu/row_sum_vector_of_structs.h"

#define AGNES_BENCHMARK(fixture, benchmark, type, ...)                                   \
	BENCHMARK_TEMPLATE_INSTANTIATE_F(fixture, benchmark, type __VA_OPT__(,) __VA_ARGS__) \
		->Name(#benchmark "_" #type)                                                     \
		->ArgsProduct({counts})

#define AGNES_BENCHMARK_SUITE(fixture, benchmark, ...)        \
	AGNES_BENCHMARK(fixture, benchmark, float , __VA_ARGS__); \
	AGNES_BENCHMARK(fixture, benchmark, double, __VA_ARGS__);

#define AGNES_EULER_BENCHMARK(fixture, benchmark) AGNES_BENCHMARK_SUITE(fixture, benchmark)

#define AGNES_ROW_SUM_BENCHMARK(fixture, benchmark) \
	AGNES_BENCHMARK_SUITE(                          \
		fixture,                                    \
		benchmark,                                  \
		std::integral_constant<std::size_t, 16>,    \
		std::index_sequence<0, 2, 4, 6, 8, 10, 12, 14>)

using namespace agnes_benchmarks::cpu;

namespace
{
	const auto default_counts = std::vector<std::int64_t>{1000000};

	std::vector<std::int64_t> parse_counts(int& argc, char** argv)
	{
		constexpr std::string_view counts_option = "--counts=";

		std::vector<std::int64_t> counts;

		auto newArgc         = 1;
		auto consumingCounts = false;

		for (auto i = 1; i < argc; ++i)
		{
			std::string_view arg = argv[i];

			if (consumingCounts)
			{
				if (arg.starts_with("--"))
				{
					consumingCounts = false;
				}
				else
				{
					counts.push_back(std::stoll(std::string(arg)));

					continue;
				}
			}

			if (arg.starts_with(counts_option))
			{
				const auto range = arg.substr(counts_option.length());

				if (range.length() > 0)
				{
					counts.push_back(std::stoll(std::string(range)));
				}

				consumingCounts = true;

				continue;
			}

			argv[newArgc++] = argv[i];
		}

		argc = newArgc;

		if (counts.empty())
		{
			counts = default_counts;
		}

		return counts;
	}

	template <std::size_t ColumnCount, std::size_t... Indexes>
	struct row_sum_config
	{
		using column_count = std::integral_constant<std::size_t, ColumnCount>;
		using indexes      = std::index_sequence<Indexes...>;

		const char* name = nullptr;
	};

	void register_benchmarks(const std::vector<std::int64_t>& counts)
	{
		AGNES_EULER_BENCHMARK(particle_vector_fixture           , euler_vector_of_structs_range     );
		AGNES_EULER_BENCHMARK(particle_vector_fixture           , euler_vector_of_structs_iterated  );
		AGNES_EULER_BENCHMARK(particle_vector_fixture           , euler_vector_of_structs_indexed   );
		AGNES_EULER_BENCHMARK(particle_vector_fixture           , euler_vector_of_structs_restricted);

		AGNES_EULER_BENCHMARK(particle_struct_of_arrays_fixture , euler_struct_of_arrays_iterated   );
		AGNES_EULER_BENCHMARK(particle_struct_of_arrays_fixture , euler_struct_of_arrays_indexed    );
		AGNES_EULER_BENCHMARK(particle_struct_of_arrays_fixture , euler_struct_of_arrays_restricted );

		AGNES_EULER_BENCHMARK(particle_struct_of_vectors_fixture, euler_struct_of_vectors_iterated  );
		AGNES_EULER_BENCHMARK(particle_struct_of_vectors_fixture, euler_struct_of_vectors_indexed   );
		AGNES_EULER_BENCHMARK(particle_struct_of_vectors_fixture, euler_struct_of_vectors_restricted);

		AGNES_EULER_BENCHMARK(agnes_particle_vector_fixture     , euler_agnes_vector_range          );
		AGNES_EULER_BENCHMARK(agnes_particle_vector_fixture     , euler_agnes_vector_iterated       );
		AGNES_EULER_BENCHMARK(agnes_particle_vector_fixture     , euler_agnes_vector_indexed        );
		AGNES_EULER_BENCHMARK(agnes_particle_vector_fixture     , euler_agnes_vector_restricted     );

		AGNES_EULER_BENCHMARK(agnes_particle_table_fixture      , euler_agnes_table_range           );
		AGNES_EULER_BENCHMARK(agnes_particle_table_fixture      , euler_agnes_table_iterated        );
		AGNES_EULER_BENCHMARK(agnes_particle_table_fixture      , euler_agnes_table_indexed         );
		AGNES_EULER_BENCHMARK(agnes_particle_table_fixture      , euler_agnes_table_restricted      );

		AGNES_ROW_SUM_BENCHMARK(row_vector_fixture           , row_sum_vector_of_structs_range     );
		AGNES_ROW_SUM_BENCHMARK(row_vector_fixture           , row_sum_vector_of_structs_iterated  );
		AGNES_ROW_SUM_BENCHMARK(row_vector_fixture           , row_sum_vector_of_structs_indexed   );
		AGNES_ROW_SUM_BENCHMARK(row_vector_fixture           , row_sum_vector_of_structs_restricted);

		AGNES_ROW_SUM_BENCHMARK(row_struct_of_arrays_fixture , row_sum_struct_of_arrays_iterated   );
		AGNES_ROW_SUM_BENCHMARK(row_struct_of_arrays_fixture , row_sum_struct_of_arrays_indexed    );
		AGNES_ROW_SUM_BENCHMARK(row_struct_of_arrays_fixture , row_sum_struct_of_arrays_restricted );

		AGNES_ROW_SUM_BENCHMARK(row_struct_of_vectors_fixture, row_sum_struct_of_vectors_iterated  );
		AGNES_ROW_SUM_BENCHMARK(row_struct_of_vectors_fixture, row_sum_struct_of_vectors_indexed   );
		AGNES_ROW_SUM_BENCHMARK(row_struct_of_vectors_fixture, row_sum_struct_of_vectors_restricted);

		AGNES_ROW_SUM_BENCHMARK(agnes_row_vector_fixture     , row_sum_agnes_vector_range          );
		AGNES_ROW_SUM_BENCHMARK(agnes_row_vector_fixture     , row_sum_agnes_vector_iterated       );
		AGNES_ROW_SUM_BENCHMARK(agnes_row_vector_fixture     , row_sum_agnes_vector_indexed        );
		AGNES_ROW_SUM_BENCHMARK(agnes_row_vector_fixture     , row_sum_agnes_vector_restricted     );
	}
}

int main(int argc, char** argv)
{
	benchmark::MaybeReenterWithoutASLR(argc, argv);

	if constexpr (agnes::avoid_cache_conflicts_v)
	{
		agnes::set_default_allocator_cache_configuration(agnes_benchmarks::get_cache_configuration());
	}

	register_benchmarks(parse_counts(argc, argv));

	::benchmark::Initialize(&argc, argv);
	::benchmark::RunSpecifiedBenchmarks();
}
