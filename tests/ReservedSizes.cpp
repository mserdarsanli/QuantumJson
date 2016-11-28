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

#include "lib/Common.hpp"

#define CATCH_CONFIG_MAIN
#include <catch.hpp>

using namespace std;

TEST_CASE("Basic element type allocations check")
{
	string listValue16 = "[1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16]";
	string listValue17 = "[1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17]";


	SECTION("Prereserve 16")
	{
		vector<int> obj;

		// TODO this part should be automatic
		QuantumJsonImpl__::PreAllocator<string::const_iterator> preAllocator(
		    listValue16.begin(), listValue16.end());
		preAllocator.ReserveSpaceIn(obj);

		CHECK( obj.capacity() == 16 );
	}

	SECTION("Prereserve 17")
	{
		vector<int> obj;

		// TODO this part should be automatic
		QuantumJsonImpl__::PreAllocator<string::const_iterator> preAllocator(
		    listValue17.begin(), listValue17.end());
		preAllocator.ReserveSpaceIn(obj);

		CHECK( obj.capacity() == 17 );
	}
}

TEST_CASE("Recursive list reserve")
{
	string listValue = "["
	    "[1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16],"
	    "[],"
	    "[1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17]"
	    "]";


	SECTION("Prereserve recursive")
	{
		vector< vector<int> > obj;

		// TODO this part should be automatic
		QuantumJsonImpl__::PreAllocator<string::const_iterator> preAllocator(
		    listValue.begin(), listValue.end());
		preAllocator.ReserveSpaceIn(obj);

		CHECK( obj.capacity() == 3 );
		CHECK( obj[0].capacity() == 16 );
		CHECK( obj[1].capacity() == 0 );
		CHECK( obj[2].capacity() == 17 );
	}
}

TEST_CASE("Reserve list of strings")
{
	string listValue = "["
	    "\"xxxxxxxxxxxxxxxxxxxxx\","
	    "\"\","
	    "\"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx\""
	    "]";

	size_t emptyStringCapacity = -1;
	{
		string a;
		emptyStringCapacity = a.capacity();
	}

	SECTION("Prereserve recursive")
	{
		vector< string > obj;

		// TODO this part should be automatic
		QuantumJsonImpl__::PreAllocator<string::const_iterator> preAllocator(
		    listValue.begin(), listValue.end());
		preAllocator.ReserveSpaceIn(obj);

		CHECK( obj.capacity() == 3 );
		// Following tests are dependent on standard library implementation
		// Still useful to have tests rather than nothing.
		CHECK( obj[0].capacity() > emptyStringCapacity );
		CHECK( obj[1].capacity() == emptyStringCapacity );
		CHECK( obj[2].capacity() > obj[0].capacity() );
	}
}
