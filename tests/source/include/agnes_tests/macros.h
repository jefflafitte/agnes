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

#include <catch2/catch_template_test_macros.hpp>

#ifndef AGNES_CONST_EVAL_TESTS
#define AGNES_CONST_EVAL_TESTS 0
#endif

#if AGNES_CONST_EVAL_TESTS
#define AGNES_REQUIRE(...) if (!(__VA_ARGS__)) return false
#define AGNES_RUN(Test, ...) static_assert(Test(__VA_ARGS__)); REQUIRE(Test(__VA_ARGS__))
#else
#define AGNES_REQUIRE(...) REQUIRE(__VA_ARGS__)
#define AGNES_RUN(Test, ...) Test(__VA_ARGS__)
#endif
