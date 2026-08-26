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
#include <memory>
#include <utility>

namespace agnes_benchmarks
{
	template <typename T, std::size_t ColumnCount>
	struct row_struct_of_arrays
	{
		void resize(const std::size_t size)
		{
			[this, size]<std::size_t... I>(std::index_sequence<I...>)
			{
				((columns[I] = std::make_unique<T[]>(size)), ...);
			}(std::make_index_sequence<ColumnCount>{});

			size_ = size;
		}

		void clear()
		{
			[this]<std::size_t... I>(std::index_sequence<I...>)
			{
				(columns[I].reset(), ...);
			}(std::make_index_sequence<ColumnCount>{});

			size_ = 0;
		}

		void shrink_to_fit() {}

		std::size_t size() const { return size_; }

		std::unique_ptr<T[]> columns[ColumnCount];

	private:
		std::size_t size_ = 0;
	};
}
