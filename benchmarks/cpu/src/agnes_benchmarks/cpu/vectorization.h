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

#if defined(_MSC_VER)
#define AGNES_RESTRICT __restrict
#define AGNES_DISABLE_VECTORIZATION _Pragma("loop(no_vector)")
#define AGNES_DISABLE_RANGE_BASED_VECTORIZATION
#elif defined(__clang__)
#define AGNES_RESTRICT __restrict__
#define AGNES_DISABLE_VECTORIZATION _Pragma("clang loop vectorize(disable)")
#define AGNES_DISABLE_RANGE_BASED_VECTORIZATION AGNES_DISABLE_VECTORIZATION
#elif defined(__GNUC__)
#define AGNES_RESTRICT __restrict__
#define AGNES_DISABLE_VECTORIZATION _Pragma("GCC novector")
#define AGNES_DISABLE_RANGE_BASED_VECTORIZATION AGNES_DISABLE_VECTORIZATION
#else
#define AGNES_RESTRICT
#define AGNES_DISABLE_VECTORIZATION
#define AGNES_DISABLE_RANGE_BASED_VECTORIZATION
#endif

#ifndef AGNES_NO_VECTORIZATION
#define AGNES_NO_VECTORIZATION 0
#endif

#if AGNES_NO_VECTORIZATION
#define AGNES_COND_DISABLE_VECTORIZATION AGNES_DISABLE_VECTORIZATION
#define AGNES_COND_DISABLE_RANGE_BASED_VECTORIZATION AGNES_DISABLE_RANGE_BASED_VECTORIZATION
#else
#define AGNES_COND_DISABLE_VECTORIZATION
#define AGNES_COND_DISABLE_RANGE_BASED_VECTORIZATION
#endif
