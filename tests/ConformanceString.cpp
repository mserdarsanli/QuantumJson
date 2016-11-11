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

#include <iostream>
#include <string>
#include <vector>

// TODO Work-around not being able to include the base lib stand alone
#include "lib/Common.hpp"

#define CATCH_CONFIG_MAIN
#include <catch.hpp>

using namespace std;

// http://stackoverflow.com/a/5641470/620438
#define S(x) #x
#define S_(x) S(x)
#define S__LINE__ S_(__LINE__)


#define TEST_STRING(json, expect) \
	TEST_CASE("String conformance test " S__LINE__) \
	{ \
		vector< string > v; \
		string jsonStr( json ); \
		QuantumJsonImpl__::Parser< std::string::const_iterator> p(jsonStr.begin(), jsonStr.end()); \
		p.ParseValueInto(v); \
		REQUIRE( v.size() == 1 ); \
		\
		string actualResult = v[0]; \
		string expectedResult(expect, expect + sizeof(expect) - 1); \
		REQUIRE( actualResult == expectedResult ); \
	}

#include "ConformanceStringData.hpp"
