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
#include <vector>

namespace agnes_benchmarks
{
	template <typename T>
	struct particle_struct_of_vectors
	{
		void resize(const std::size_t size)
		{
			px.resize(size);
			py.resize(size);
			pz.resize(size);
			vx.resize(size);
			vy.resize(size);
			vz.resize(size);
			ax.resize(size);
			ay.resize(size);
			az.resize(size);
		}

		void clear()
		{
			px.clear();
			py.clear();
			pz.clear();
			vx.clear();
			vy.clear();
			vz.clear();
			ax.clear();
			ay.clear();
			az.clear();
		}

		void shrink_to_fit()
		{
			px.shrink_to_fit();
			py.shrink_to_fit();
			pz.shrink_to_fit();
			vx.shrink_to_fit();
			vy.shrink_to_fit();
			vz.shrink_to_fit();
			ax.shrink_to_fit();
			ay.shrink_to_fit();
			az.shrink_to_fit();
		}

		std::size_t size() const { return px.size(); }

		std::vector<T> px; std::vector<T> py; std::vector<T> pz;
		std::vector<T> vx; std::vector<T> vy; std::vector<T> vz;
		std::vector<T> ax; std::vector<T> ay; std::vector<T> az;
	};
}
