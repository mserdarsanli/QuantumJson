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

#include <string>
#include <vector>

#include "tests/Schema1.gen.hpp"

#define CATCH_CONFIG_MAIN
#include <catch.hpp>

using namespace std;

// Tests are only valid when preallocation is enabled
#if QUANTUMJON_PREALLOCATE_ON_RANDOMACCESSITERATOR

TEST_CASE("Basic object allocation check")
{
	string itemsJson = "{\"i1\":{},\"i2\":{\"name\":\"qweqweqweqweqweq\"}}";

	SECTION("Prereserve struct")
	{
		Items obj;

		// TODO this part should be automatic
		QuantumJsonImpl__::PreAllocator<string::const_iterator> preAllocator(
		    itemsJson.begin(), itemsJson.end());
		preAllocator.ReserveSpaceIn(obj);

		CHECK( obj.i1.name.capacity() >= 0 );
		CHECK( obj.i2.name.capacity() >= 16 );
	}
}

TEST_CASE("Reserve lists of objects")
{
	string item1 = "{\"name\":\"qweqweqweqwe\"}";
	string item2 = "{\"name\":\"qweqweqweqweqqqjh\"}";
	string item3 = "{\"name\":\"qweqwe\"}";
	string item4 = "{\"name\":\"qweqkkkkkkkkkkkweqweqwe\"}";

	string itemListJson = "{\"items\":["
	    + item1 + "," + item2 + "," + item3 + "," + item4 + "]}";

	SECTION("Lists of complex structs")
	{
		ItemList obj;

		// TODO this part should be automatic
		QuantumJsonImpl__::PreAllocator<string::const_iterator> preAllocator(
		    itemListJson.begin(), itemListJson.end());
		preAllocator.ReserveSpaceIn(obj);

		CHECK( obj.items.size() == 4 );
		CHECK( obj.items[0].name.capacity() >= 12 );
		CHECK( obj.items[1].name.capacity() >= 17 );
		CHECK( obj.items[2].name.capacity() >=  6 );
		CHECK( obj.items[3].name.capacity() >= 23 );
	}
}

#endif
