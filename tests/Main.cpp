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

#include "tests/Schema1.gen.hpp"

#define CATCH_CONFIG_MAIN
#include <catch.hpp>

using namespace std;

TEST_CASE("Simple Parser")
{
	string in = "{\"kind\":\"sadsda\"}";
	Listing l = QuantumJson::Parse(in.begin(), in.end());
	REQUIRE( l.kind == "sadsda" );
}

TEST_CASE("String escape sequences")
{
	string in = R"({"kind":"\"\\\/\b\f\n\r\t"})";
	Listing l = QuantumJson::Parse(in.begin(), in.end());

	REQUIRE( l.kind == "\"\\/\b\f\n\r\t" );
}

TEST_CASE("Unicode escapes")
{
	string in = R"({"kind":"\u002f"})";
	Listing l = QuantumJson::Parse(in.begin(), in.end());

	REQUIRE( l.kind == "/" );
}

TEST_CASE("Unicode surrogate pairs")
{
	// Test this arbitrary chinese character
	string in = R"({"kind":"\ud87e\udcb4"})";
	Listing l = QuantumJson::Parse(in.begin(), in.end());

	REQUIRE( l.kind == "扝" );
}

TEST_CASE("Attributes")
{
	string in = R"(
		{
		  "attr1": "val1",
		  "attr-2": "val2"
		}
	)";
	AttributeTester o = QuantumJson::Parse(in.begin(), in.end());

	REQUIRE( o.attr1 == "val1" );
	REQUIRE( o.attr2 == "val2" );
}

TEST_CASE("Nullable fields")
{
	SECTION( "null on nullable attribute" )
	{
		string in = R"(
			{
			  "attr_accepting_null": null,
			  "attr_regular": "val2"
			}
		)";
		SkipNullTester o = QuantumJson::Parse(in.begin(), in.end());

		REQUIRE( o.attr_accepting_null == "" );
		REQUIRE( o.attr_regular == "val2" );
	}

	SECTION( "null on non-nullable attribute" )
	{
		string json = R"(
			{
			  "attr_accepting_null": "val1",
			  "attr_regular": null
			}
		)";
		REQUIRE_THROWS_WITH(
		    SkipNullTester o = QuantumJson::Parse(json.begin(), json.end()),
		    "Unexpected Char" );
	}
}

TEST_CASE("Unknown Attributes")
{
	string in = R"(
		{
		  "unknown-attribute": 1321312.21,
		  "attr1": "wqwqeweqeq"
		}
	)";
	AttributeTester o = QuantumJson::Parse(in.begin(), in.end());

	// Check of parsing completes successfully
	REQUIRE( o.attr1 == "wqwqeweqeq" );
}
