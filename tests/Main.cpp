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
#include "third_party/catch.hpp"

using namespace std;

TEST_CASE( "Simple Parser", "[parser]" )
{
	Listing l;
	l.MergeFromJson("{\"kind\":\"sadsda\"}");

	REQUIRE( l.kind == "sadsda" );
}

TEST_CASE( "String escape sequences", "[parser]" )
{
	Listing l;
	l.MergeFromJson(R"({"kind":"\"\\\/\b\f\n\r\t"})");

	REQUIRE( l.kind == "\"\\/\b\f\n\r\t" );
}

TEST_CASE( "Unicode escapes", "[parser]" )
{
	Listing l;
	l.MergeFromJson(R"({"kind":"\u002f"})");
	REQUIRE( l.kind == "/" );
}

TEST_CASE( "Unicode surrogate pairs", "[parser]" )
{
	// Test this arbitrary chinese character
	Listing l;
	l.MergeFromJson(R"({"kind":"\ud87e\udcb4"})");
	REQUIRE( l.kind == "扝" );
}

TEST_CASE( "Attributes", "[attributes]" )
{
	AttributeTester o;
	o.MergeFromJson(R"(
		{
		  "attr1": "val1",
		  "attr-2": "val2"
		}
	)");

	REQUIRE( o.attr1 == "val1" );
	REQUIRE( o.attr2 == "val2" );
}

TEST_CASE( "Nullable fields", "[attributes]" )
{
	SkipNullTester o;

	SECTION( "null on nullable attribute" )
	{
		o.MergeFromJson(R"(
			{
			  "attr_accepting_null": null,
			  "attr_regular": "val2"
			}
		)");

		REQUIRE( o.attr_accepting_null == "" );
		REQUIRE( o.attr_regular == "val2" );
	}

	SECTION( "null on non-nullable attribute" )
	{
		const char json[] = R"(
			{
			  "attr_accepting_null": "val1",
			  "attr_regular": null
			}
		)";
		REQUIRE_THROWS_WITH( o.MergeFromJson(json), Catch::StartsWith("Unexpected") );
	}
}

TEST_CASE( "Unknown Attributes", "[attributes]" )
{
	AttributeTester o;

	o.MergeFromJson(R"(
		{
		  "unknown-attribute": 1321312.21,
		  "attr1": "wqwqeweqeq"
		}
	)");

	// Check of parsing completes successfully
	REQUIRE( o.attr1 == "wqwqeweqeq" );
}
