// The MIT License (MIT)
//
// Copyright (c) 2016 Mustafa Serdar Sanli
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <iostream>
#include <iterator>
#include <sstream>

#include "lib/Common.hpp"

#include <catch2/catch.hpp>

using namespace std;

int statsAllocationCount = 0;
size_t statsLastAllocationSize = -1;

void resetAllocationStats()
{
	statsAllocationCount = 0;
	statsLastAllocationSize = -1;
}

// Override new/delete to keep track of allocation stats
void* operator new(size_t sz)
{
	statsAllocationCount++;
	statsLastAllocationSize = sz;
	return malloc(sz);
}

void operator delete(void* ptr) noexcept
{
	free(ptr);
}

// String large enough to trigger multiple allocations in std::string,
// when constructed via push_back calls (for inputs of unknown size)
const char largeJsonString[] = "\"asewqeeqwiewqeqwyuiwqywiuyeiuyeiqwyueyweq"
    "uieyquiyeuiqyeuwiqyequieyqwiyewiquyeiueyqiewuyequwiyeiueyiuewyieqwyiewuiq"
    "weyieuqwyeiyeqwuiyeiqyewiqeyquwiyewiqyewiyeiyewiqyeuirwyuiwrywqiuyruiyrqu"
    "wiweqefaegywruiyrwqiuyrwquiqwqwewqrqwqwrwqrwqrwqrqwrerrewrwqrrqeqewqewd\"";

const char largeJsonStringValue[] = "asewqeeqwiewqeqwyuiwqywiuyeiuyeiqwyueyweq"
    "uieyquiyeuiqyeuwiqyequieyqwiyewiquyeiueyqiewuyequwiyeiueyiuewyieqwyiewuiq"
    "weyieuqwyeiyeqwuiyeiqyewiqeyquwiyewiqyewiyeiyewiqyeuirwyuiwrywqiuyruiyrqu"
    "wiweqefaegywruiyrwqiuyrwquiqwqwewqrqwqwrwqrwqrwqrqwrerrewrwqrrqeqewqewd";

TEST_CASE("Minimal allocations check")
{
	size_t minimalAllocationSize = -1;

	// Get minimal allocation size, which is not the size of the string in libstdc++
	{
		resetAllocationStats();
		string allocationCheck(largeJsonStringValue);

		minimalAllocationSize = statsLastAllocationSize;
		int allocationCount = statsAllocationCount;

		// Check if the std::string implementation does only one
		// allocation when initialized with a value
		REQUIRE( allocationCount == 1 );
	}


	SECTION("Allocations with random access input are minimal - 1")
	{
		string in(largeJsonString);
		string out;

		resetAllocationStats();

		// TODO this part should be automatic
		QuantumJsonImpl__::PreAllocator<string::const_iterator> preAllocator(in.begin(), in.end());
		preAllocator.ReserveSpaceIn(out);

		QuantumJsonImpl__::Parser<string::const_iterator> p(in.begin(), in.end());
		p.ParseValueInto(out);

		int allocationCount = statsAllocationCount;
		int lastAllocationSize = statsLastAllocationSize;

		REQUIRE(out == largeJsonStringValue);

		// Following checks are dependent on standard library implementation,
		// though expected to hold on all of them.
		REQUIRE(allocationCount == 1);
		REQUIRE(lastAllocationSize == minimalAllocationSize);
	}

	// Pointer types should have random access iterator tag [1]. Here is a
	// test to make sure `const char *` inputs would execute the fast code
	// path for string parsing.
	// [1]: http://en.cppreference.com/w/cpp/iterator/iterator_traits
	SECTION("Allocations with random access input are minimal - 2")
	{
		string out;

		resetAllocationStats();
		const char *begin = largeJsonString;
		const char *end = largeJsonString + strlen(largeJsonString);

		// TODO this part should be automatic
		QuantumJsonImpl__::PreAllocator<const char*> preAllocator(begin, end);
		preAllocator.ReserveSpaceIn(out);

		QuantumJsonImpl__::Parser<const char*> p(begin, end);
		p.ParseValueInto(out);

		int allocationCount = statsAllocationCount;
		int lastAllocationSize = statsLastAllocationSize;

		REQUIRE(out == largeJsonStringValue);

		// Following checks are dependent on standard library implementation,
		// though expected to hold on all of them.
		REQUIRE(allocationCount == 1);
		REQUIRE(lastAllocationSize == minimalAllocationSize);
	}

	SECTION("Allocations with stream input are not minimal")
	{
		stringstream in(largeJsonString);
		string out;

		resetAllocationStats();
		QuantumJsonImpl__::Parser<istream_iterator<char>> p{
		    istream_iterator<char>(in), istream_iterator<char>()};
		p.ParseValueInto(out);

		int allocationCount = statsAllocationCount;
		int lastAllocationSize = statsLastAllocationSize;

		REQUIRE(out == largeJsonStringValue);

		// Following checks are dependent on standard library implementation,
		// though expected to hold on all of them.
		REQUIRE(allocationCount > 1);
		REQUIRE(lastAllocationSize > minimalAllocationSize);
	}
}

