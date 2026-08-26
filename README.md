# Agnes - C++ Library for Column-Oriented Data

![C++](https://img.shields.io/badge/C++-00599C.svg?&logo=c%2B%2B)
[![Documentation](https://img.shields.io/badge/docs-online-blue)](https://jefflafitte.github.io/agnes/)
[![License: AGPL v3](https://img.shields.io/badge/License-AGPL_v3-blue.svg)](https://www.gnu.org/licenses/agpl-3.0)

Agnes is a C++ header-only library providing resizable containers that store heterogeneous types sequentially by type.

## Container Types

### agnes::vector

`template <typename... T> vector` is a resizable sequence container. Values of each heterogeneous type `T...` are stored sequentially by type.

In other words, the memory layout of `agnes::vector<type_a, type_b, type_c>` is

	begin type_a        end type_a    begin type_b        end type_b    begin type_c        end type_c
	|                   |             |                   |             |                   |
	v                   v             v                   v             v                   v
	+------+-----+------+-------------+------+-----+------+-------------+------+-----+------+-------------+
	| a[0] | ... | a[N] |  [reserve]  | b[0] | ... | b[N] |  [reserve]  | c[0] | ... | c[N] |  [reserve]  |
	+------+-----+------+-------------+------+-----+------+-------------+------+-----+------+-------------+

Values in an `agnes::vector` are accessed with the function template `agnes::get<I>()` much like accessing values in a `std::tuple`:

	type_a& valueA = get<0>(myVector[0]);
	type_b& valueB = get<1>(myVector[0]);
	type_c& valueC = get<2>(myVector[0]);

### agnes::table

`template <typename... T> table` is a resizable sequence container. Each heterogeneous type `T...` represents a column in the table. Each column exposes a value type and values of these types are stored sequentially by type.

In other words, the memory layout of `agnes::table<column_a, column_b, column_c>` is

	begin column_a      end column_a  begin column_b      end column_b  begin column_c      end column_c
	|                   |             |                   |             |                   |
	v                   v             v                   v             v                   v
	+------+-----+------+-------------+------+-----+------+-------------+------+-----+------+-------------+
	| a[0] | ... | a[N] |  [reserve]  | b[0] | ... | b[N] |  [reserve]  | c[0] | ... | c[N] |  [reserve]  |
	+------+-----+------+-------------+------+-----+------+-------------+------+-----+------+-------------+

Columns are user-defined classes and helper macros are provided to automate column definitions.

Values for each column are accessed through data members defined by the column classes:

	auto& valueA = myTable[0].a;
	auto& valueB = myTable[0].b;
	auto& valueC = myTable[0].c;

## Quickstart

### agnes::vector

	#include <cassert>

	#include <agnes/vector.h>

	int main()
	{
		agnes::vector<int, float> v{{0, 0.0f}, {1, 1.0f}};

		v.push_back(2, 2.0f);

		assert(get<0>(v[0]) == 0);
		assert(get<0>(v[1]) == 1);
		assert(get<0>(v[2]) == 2);

		assert(get<1>(v[0]) == 0.0f);
		assert(get<1>(v[1]) == 1.0f);
		assert(get<1>(v[2]) == 2.0f);
	}

### agnes::table

	#include <cassert>

	#include <agnes/table.h>

	AGNES_COLUMN(int  , a);
	AGNES_COLUMN(float, b);

	int main()
	{
		agnes::table<column_a, column_b> v{{0, 0.0f}, {1, 1.0f}};

		v.push_back(2, 2.0f);

		assert(v[0].a == 0);
		assert(v[1].a == 1);
		assert(v[2].a == 2);

		assert(v[0].b == 0.0f);
		assert(v[1].b == 1.0f);
		assert(v[2].b == 2.0f);
	}

## License

Agnes is licensed under the [GNU Affero General Public License v3.0 (AGPL-3.0)](LICENSE).

