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

#include "agnes_benchmarks/cache_configuration.h"

#include <stdexcept>

#if defined(_WIN32)

#include <vector>

#include <windows.h>

#elif defined(__linux__)

#include <unistd.h>

#endif

namespace agnes_benchmarks
{
#if defined(_WIN32)

	agnes::cache_configuration get_cache_configuration()
	{
		auto length = DWORD{0};

		GetLogicalProcessorInformation(nullptr, &length);

		auto buffer = std::vector<SYSTEM_LOGICAL_PROCESSOR_INFORMATION>(
			length/sizeof(SYSTEM_LOGICAL_PROCESSOR_INFORMATION));

		if (!GetLogicalProcessorInformation(buffer.data(), &length))
		{
			throw std::runtime_error{"GetLogicalProcessorInformation() failed."};
		}

		for (const auto& info : buffer)
		{
			if ((info.Relationship == RelationCache) && (info.Cache.Level == 1) && (info.Cache.Type == CacheData))
			{
				return {info.Cache.Size, info.Cache.Associativity};
			}
		}

		throw std::runtime_error{"Could not find L1 data cache information."};
	}

#elif defined(__linux__)

	agnes::cache_configuration get_cache_configuration()
	{
		const auto size          = sysconf(_SC_LEVEL1_DCACHE_SIZE);
		const auto associativity = sysconf(_SC_LEVEL1_DCACHE_ASSOC);

		if ((size <= 0) || (associativity <= 0))
		{
			throw std::runtime_error{"sysconf() could not determine L1 data cache configuration."};
		}

		return {static_cast<std::size_t>(size), static_cast<std::size_t>(associativity)};
	}

#else

#error "get_cache_configuration() is not implemented for this platform."

#endif
}
