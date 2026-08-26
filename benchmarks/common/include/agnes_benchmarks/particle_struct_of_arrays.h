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

namespace agnes_benchmarks
{
	template <typename T>
	struct particle_struct_of_arrays
	{
		void resize(const std::size_t size)
		{
			px = std::make_unique<T[]>(size);
			py = std::make_unique<T[]>(size);
			pz = std::make_unique<T[]>(size);
			vx = std::make_unique<T[]>(size);
			vy = std::make_unique<T[]>(size);
			vz = std::make_unique<T[]>(size);
			ax = std::make_unique<T[]>(size);
			ay = std::make_unique<T[]>(size);
			az = std::make_unique<T[]>(size);

			size_ = size;
		}

		void clear()
		{
			px.reset();
			py.reset();
			pz.reset();
			vx.reset();
			vy.reset();
			vz.reset();
			ax.reset();
			ay.reset();
			az.reset();

			size_ = 0;
		}

		void shrink_to_fit() {}

		std::size_t size() const { return size_; }

		std::unique_ptr<T[]> px; std::unique_ptr<T[]> py; std::unique_ptr<T[]> pz;
		std::unique_ptr<T[]> vx; std::unique_ptr<T[]> vy; std::unique_ptr<T[]> vz;
		std::unique_ptr<T[]> ax; std::unique_ptr<T[]> ay; std::unique_ptr<T[]> az;

	private:
		std::size_t size_ = 0;
	};
}
