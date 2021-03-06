// The MIT License (MIT)
//
// Copyright (c) 2016 Mustafa Serdar Sanli
// Copyright (c) 2015 Markus Kuhn
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

// Tests cases for
// "UTF-8 decoder capability and stress test"
// authored by Markus Kuhn
// https://www.cl.cam.ac.uk/~mgk25/ucs/examples/UTF-8-test.txt
//
// a copy of the file can be found under tests/doc/

#include <iostream>

#include "lib/Common.hpp"

#include <catch2/catch.hpp>

using namespace std;

// Helper macros since we don't have overloads for Parse(unsigned char*)
#define PARSE(data) \
    QuantumJson::Parse( reinterpret_cast<const char*>(data) , \
                        reinterpret_cast<const char*>(data) + sizeof(data) )


// Since QuantumJson::Parse is a proxy, it needs to be assigned to a variable
// to trigger parsing.
#define PARSE_AS_STR(data) \
    string s = QuantumJson::Parse( reinterpret_cast<const char*>(data) , \
                                   reinterpret_cast<const char*>(data) + sizeof(data) )

TEST_CASE("Case 1")
{
	unsigned char input[] = { '"',
	    0xce, 0xba, 0xe1, 0xbd, 0xb9, 0xcf, 0x83, 0xce, 0xbc, 0xce, 0xb5,
	'"', };

	string out = PARSE(input);
	REQUIRE( out == u8"κόσμε" );
}

TEST_CASE("Case 2.1.1")
{
	unsigned char input[] = { '"',
	    0x00,
	'"', };

	REQUIRE_THROWS_WITH( [&](){ PARSE_AS_STR(input); }(), "Control Character In String" );
}

TEST_CASE("Case 2.1.2")
{
	unsigned char input[] = { '"',
	    0xc2, 0x80,
	'"', };

	string out = PARSE(input);
	REQUIRE( out == "\u0080" );
}

TEST_CASE("Case 2.1.3")
{
	unsigned char input[] = { '"',
	    0xe0, 0xa0, 0x80,
	'"', };

	string out = PARSE(input);
	REQUIRE( out == "\u0800" );
}

TEST_CASE("Case 2.1.4")
{
	unsigned char input[] = { '"',
	    0xf0, 0x90, 0x80, 0x80,
	'"', };

	string out = PARSE(input);
	REQUIRE( out == "\U00010000" );
}

TEST_CASE("Case 2.1.5")
{
	unsigned char input[] = { '"',
	    0xf8, 0x88, 0x80, 0x80, 0x80,
	'"', };

	string out = PARSE(input);
	REQUIRE( out == "\U00200000" );
}

TEST_CASE("Case 2.1.6")
{
	unsigned char input[] = { '"',
	    0xfc, 0x84, 0x80, 0x80, 0x80, 0x80,
	'"', };

	string out = PARSE(input);
	REQUIRE( out == "\U04000000" );
}

TEST_CASE("Case 2.2.1")
{
	unsigned char input[] = { '"',
	    0x7f,
	'"', };

	string out = PARSE(input);
	REQUIRE( out == "\u007f" );
}

TEST_CASE("Case 2.2.2")
{
	unsigned char input[] = { '"',
	    0xdf, 0xbf,
	'"', };

	string out = PARSE(input);
	REQUIRE( out == "\u07ff" );
}

TEST_CASE("Case 2.2.3")
{
	unsigned char input[] = { '"',
	    0xef, 0xbf, 0xbf,
	'"', };

	string out = PARSE(input);
	REQUIRE( out == "\uffff" );
}

TEST_CASE("Case 2.2.4")
{
	unsigned char input[] = { '"',
	    0xf7, 0xbf, 0xbf, 0xbf,
	'"', };

	string out = PARSE(input);
	REQUIRE( out == "\U001fffff" );
}

TEST_CASE("Case 2.2.5")
{
	unsigned char input[] = { '"',
	    0xfb, 0xbf, 0xbf, 0xbf, 0xbf,
	'"', };

	string out = PARSE(input);
	REQUIRE( out == "\U03ffffff" );
}

TEST_CASE("Case 2.2.6")
{
	unsigned char input[] = { '"',
	    0xfd, 0xbf, 0xbf, 0xbf, 0xbf, 0xbf,
	'"', };

	string out = PARSE(input);
	REQUIRE( out == "\U7fffffff" );
}

TEST_CASE("Case 2.3.1")
{
	unsigned char input[] = { '"',
	    0xed, 0x9f, 0xbf,
	'"', };

	string out = PARSE(input);
	REQUIRE( out == "\ud7ff" );
}

TEST_CASE("Case 2.3.2")
{
	unsigned char input[] = { '"',
	    0xee, 0x80, 0x80,
	'"', };

	string out = PARSE(input);
	REQUIRE( out == "\ue000" );
}

TEST_CASE("Case 2.3.3")
{
	unsigned char input[] = { '"',
	    0xef, 0xbf, 0xbd,
	'"', };

	string out = PARSE(input);
	REQUIRE( out == "\ufffd" );
}

TEST_CASE("Case 2.3.4")
{
	unsigned char input[] = { '"',
	    0xf4, 0x8f, 0xbf, 0xbf,
	'"', };

	string out = PARSE(input);
	REQUIRE( out == "\U0010FFFF" );
}

TEST_CASE("Case 2.3.5")
{
	unsigned char input[] = { '"',
	    0xf4, 0x90, 0x80, 0x80,
	'"', };

	string out = PARSE(input);
	REQUIRE( out == "\U00110000" );
}

TEST_CASE("Case 3.1.1")
{
	unsigned char input[] = { '"',
	    0x80,
	'"', };

	REQUIRE_THROWS_WITH( [&](){ PARSE_AS_STR(input); }(), "Invalid UTF-8 Sequence" );
}

TEST_CASE("Case 3.1.2")
{
	unsigned char input[] = { '"',
	    0xbf,
	'"', };

	REQUIRE_THROWS_WITH( [&](){ PARSE_AS_STR(input); }(), "Invalid UTF-8 Sequence" );
}

TEST_CASE("Case 3.1.3")
{
	unsigned char input[] = { '"',
	    0x80, 0xbf,
	'"', };

	REQUIRE_THROWS_WITH( [&](){ PARSE_AS_STR(input); }(), "Invalid UTF-8 Sequence" );
}

TEST_CASE("Case 3.1.4")
{
	unsigned char input[] = { '"',
	    0x80, 0xbf, 0x80,
	'"', };

	REQUIRE_THROWS_WITH( [&](){ PARSE_AS_STR(input); }(), "Invalid UTF-8 Sequence" );
}

TEST_CASE("Case 3.1.5")
{
	unsigned char input[] = { '"',
	    0x80, 0xbf, 0x80, 0xbf,
	'"', };

	REQUIRE_THROWS_WITH( [&](){ PARSE_AS_STR(input); }(), "Invalid UTF-8 Sequence" );
}

TEST_CASE("Case 3.1.6")
{
	unsigned char input[] = { '"',
	    0x80, 0xbf, 0x80, 0xbf, 0x80,
	'"', };

	REQUIRE_THROWS_WITH( [&](){ PARSE_AS_STR(input); }(), "Invalid UTF-8 Sequence" );
}

TEST_CASE("Case 3.1.7")
{
	unsigned char input[] = { '"',
	    0x80, 0xbf, 0x80, 0xbf, 0x80, 0xbf,
	'"', };

	REQUIRE_THROWS_WITH( [&](){ PARSE_AS_STR(input); }(), "Invalid UTF-8 Sequence" );
}

TEST_CASE("Case 3.1.8")
{
	unsigned char input[] = { '"',
	    0x80, 0xbf, 0x80, 0xbf, 0x80, 0xbf, 0x80,
	'"', };

	REQUIRE_THROWS_WITH( [&](){ PARSE_AS_STR(input); }(), "Invalid UTF-8 Sequence" );
}

TEST_CASE("Case 3.1.9")
{
	unsigned char input[] = { '"',
	    0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
	    0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f,
	    0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97,
	    0x98, 0x99, 0x9a, 0x9b, 0x9c, 0x9d, 0x9e, 0x9f,
	    0xa0, 0xa1, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7,
	    0xa8, 0xa9, 0xaa, 0xab, 0xac, 0xad, 0xae, 0xaf,
	    0xb0, 0xb1, 0xb2, 0xb3, 0xb4, 0xb5, 0xb6, 0xb7,
	    0xb8, 0xb9, 0xba, 0xbb, 0xbc, 0xbd, 0xbe, 0xbf,
	'"', };

	REQUIRE_THROWS_WITH( [&](){ PARSE_AS_STR(input); }(), "Invalid UTF-8 Sequence" );
}

TEST_CASE("Case 3.2.1")
{
	for (int i = 0xc0; i <= 0xdf; ++i)
	{
		unsigned char input[] = { '"',
		    static_cast<unsigned char>(i), 0x20,
		'"', };
		REQUIRE_THROWS_WITH( [&](){ PARSE_AS_STR(input); }(), "Invalid UTF-8 Sequence" );
	}
}

TEST_CASE("Case 3.2.2")
{
	for (int i = 0xe0; i <= 0xef; ++i)
	{
		unsigned char input[] = { '"',
		    static_cast<unsigned char>(i), 0x20,
		'"', };
		REQUIRE_THROWS_WITH( [&](){ PARSE_AS_STR(input); }(), "Invalid UTF-8 Sequence" );
	}
}

TEST_CASE("Case 3.2.3")
{
	for (int i = 0xf0; i <= 0xf7; ++i)
	{
		unsigned char input[] = { '"',
		    static_cast<unsigned char>(i), 0x20,
		'"', };
		REQUIRE_THROWS_WITH( [&](){ PARSE_AS_STR(input); }(), "Invalid UTF-8 Sequence" );
	}
}

TEST_CASE("Case 3.2.4")
{
	for (int i = 0xf8; i <= 0xfb; ++i)
	{
		unsigned char input[] = { '"',
		    static_cast<unsigned char>(i), 0x20,
		'"', };
		REQUIRE_THROWS_WITH( [&](){ PARSE_AS_STR(input); }(), "Invalid UTF-8 Sequence" );
	}
}

TEST_CASE("Case 3.2.5")
{
	for (int i = 0xfc; i <= 0xfd; ++i)
	{
		unsigned char input[] = { '"',
		    static_cast<unsigned char>(i), 0x20,
		'"', };
		REQUIRE_THROWS_WITH( [&](){ PARSE_AS_STR(input); }(), "Invalid UTF-8 Sequence" );
	}
}

TEST_CASE("Case 3.3.1")
{
	unsigned char input[] = { '"',
	    0xc0,
	'"', };

	REQUIRE_THROWS_WITH( [&](){ PARSE_AS_STR(input); }(), "Invalid UTF-8 Sequence" );
}

TEST_CASE("Case 3.3.2")
{
	unsigned char input[] = { '"',
	    0xe0, 0x80,
	'"', };

	REQUIRE_THROWS_WITH( [&](){ PARSE_AS_STR(input); }(), "Invalid UTF-8 Sequence" );
}

TEST_CASE("Case 3.3.3")
{
	unsigned char input[] = { '"',
	    0xf0, 0x80, 0x80,
	'"', };

	REQUIRE_THROWS_WITH( [&](){ PARSE_AS_STR(input); }(), "Invalid UTF-8 Sequence" );
}

TEST_CASE("Case 3.3.4")
{
	unsigned char input[] = { '"',
	    0xf8, 0x80, 0x80, 0x80,
	'"', };

	REQUIRE_THROWS_WITH( [&](){ PARSE_AS_STR(input); }(), "Invalid UTF-8 Sequence" );
}

TEST_CASE("Case 3.3.5")
{
	unsigned char input[] = { '"',
	    0xfc, 0x80, 0x80, 0x80, 0x80,
	'"', };

	REQUIRE_THROWS_WITH( [&](){ PARSE_AS_STR(input); }(), "Invalid UTF-8 Sequence" );
}

TEST_CASE("Case 3.3.6")
{
	unsigned char input[] = { '"',
	    0xdf,
	'"', };

	REQUIRE_THROWS_WITH( [&](){ PARSE_AS_STR(input); }(), "Invalid UTF-8 Sequence" );
}

TEST_CASE("Case 3.3.7")
{
	unsigned char input[] = { '"',
	    0xef, 0xbf,
	'"', };

	REQUIRE_THROWS_WITH( [&](){ PARSE_AS_STR(input); }(), "Invalid UTF-8 Sequence" );
}

TEST_CASE("Case 3.3.8")
{
	unsigned char input[] = { '"',
	    0xf7, 0xbf, 0xbf,
	'"', };

	REQUIRE_THROWS_WITH( [&](){ PARSE_AS_STR(input); }(), "Invalid UTF-8 Sequence" );
}

TEST_CASE("Case 3.3.9")
{
	unsigned char input[] = { '"',
	    0xfb, 0xbf, 0xbf, 0xbf,
	'"', };

	REQUIRE_THROWS_WITH( [&](){ PARSE_AS_STR(input); }(), "Invalid UTF-8 Sequence" );
}

TEST_CASE("Case 3.3.10")
{
	unsigned char input[] = { '"',
	    0xfd, 0xbf, 0xbf, 0xbf, 0xbf,
	'"', };

	REQUIRE_THROWS_WITH( [&](){ PARSE_AS_STR(input); }(), "Invalid UTF-8 Sequence" );
}

// TODO Add case 3.4

TEST_CASE("Case 3.5.1")
{
	unsigned char input[] = { '"',
	    0xfe,
	'"', };

	REQUIRE_THROWS_WITH( [&](){ PARSE_AS_STR(input); }(), "Invalid UTF-8 Sequence" );
}

TEST_CASE("Case 3.5.2")
{
	unsigned char input[] = { '"',
	    0xff,
	'"', };

	REQUIRE_THROWS_WITH( [&](){ PARSE_AS_STR(input); }(), "Invalid UTF-8 Sequence" );
}

TEST_CASE("Case 3.5.3")
{
	unsigned char input[] = { '"',
	    0xfe, 0xfe, 0xff, 0xff,
	'"', };

	REQUIRE_THROWS_WITH( [&](){ PARSE_AS_STR(input); }(), "Invalid UTF-8 Sequence" );
}

// TODO Add case 4.1.1
// TODO Add case 4.1.2
// TODO Add case 4.1.3
// TODO Add case 4.1.4
// TODO Add case 4.1.5
// TODO Add case 4.2.1
// TODO Add case 4.2.2
// TODO Add case 4.2.3
// TODO Add case 4.2.4
// TODO Add case 4.2.5
// TODO Add case 4.3.1
// TODO Add case 4.3.2
// TODO Add case 4.3.3
// TODO Add case 4.3.4
// TODO Add case 4.3.5
// TODO Add case 5.1.1
// TODO Add case 5.1.2
// TODO Add case 5.1.3
// TODO Add case 5.1.4
// TODO Add case 5.1.5
// TODO Add case 5.1.6
// TODO Add case 5.1.7
// TODO Add case 5.2.1
// TODO Add case 5.2.2
// TODO Add case 5.2.3
// TODO Add case 5.2.4
// TODO Add case 5.2.5
// TODO Add case 5.2.6
// TODO Add case 5.2.7
// TODO Add case 5.2.8
// TODO Add case 5.3.1
// TODO Add case 5.3.2
// TODO Add case 5.3.3
// TODO Add case 5.3.4
