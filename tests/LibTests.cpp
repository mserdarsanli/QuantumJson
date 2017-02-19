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

#include "lib/Common.hpp"

#define CATCH_CONFIG_MAIN
#include <catch.hpp>

using namespace std;

TEST_CASE("Simple string")
{
	string out = QuantumJson::Parse("\"asd\"");
	REQUIRE(out == "asd");
}

TEST_CASE("Unterminated string")
{
	REQUIRE_THROWS_WITH(
	    string out = QuantumJson::Parse("\"asd"),
	    "Unexpected EOF");
}

TEST_CASE("Escape Characters")
{
	string in = R"("\\\/\b\f\n\r\t")";
	string expectedOut = "\\/\b\f\n\r\t";
	string out = QuantumJson::Parse(in);
	REQUIRE(out == expectedOut);
}

TEST_CASE("Invalid Escape")
{
	string in = R"("\q")";
	REQUIRE_THROWS_WITH(
	    string out = QuantumJson::Parse(in),
	    "Invalid Escape");
}

TEST_CASE("Unterminated Escape")
{
	string in = R"("qweewqew\)";
	string out;
	REQUIRE_THROWS_WITH(
	    string out = QuantumJson::Parse(in),
	    "Unexpected EOF");
}

TEST_CASE("Unicode")
{
	string in = u8"\"ğüşiöçÖÇŞİĞÜIı\"";
	string out = QuantumJson::Parse(in);
	REQUIRE(out == u8"ğüşiöçÖÇŞİĞÜIı");
}

TEST_CASE("Invalid Unicode Sequence")
{
	// String ends with no continuation character
	uint8_t invalid_utf8[] = { '"', 196, '"', 0 };
	string in(reinterpret_cast<const char*>(invalid_utf8));
	REQUIRE_THROWS_WITH(
	    string out = QuantumJson::Parse(in),
	    "Invalid UTF-8 Sequence");
}

TEST_CASE("Unicode Escape")
{
	SECTION("Simple test in BMP (lowercase hexadecimal)")
	{
		string in = R"("\u011e\u011f")";
		string out = QuantumJson::Parse(in);
		REQUIRE(out == u8"Ğğ");
	}

	SECTION("Simple test in BMP (uppercase hexadecimal)")
	{
		string in = R"("\u011E\u011F")";
		string out = QuantumJson::Parse(in);
		REQUIRE(out == u8"Ğğ");
	}

	SECTION("Simple surrogate pair")
	{
		// MATHEMATICAL BOLD CAPITAL A
		string in = R"("\uD835\uDC00")";
		string out = QuantumJson::Parse(in);
		REQUIRE(out == u8"𝐀");
	}
}

TEST_CASE("Invalid Unicode Escape")
{
	SECTION("Incomplete escape")
	{
		string in = R"("\uD8)";
		REQUIRE_THROWS_WITH(
		    string out = QuantumJson::Parse(in),
		    "Unexpected EOF");
	}

	SECTION("Invalid escape")
	{
		string in = R"("\uD8")";
		REQUIRE_THROWS_WITH(
		    string out = QuantumJson::Parse(in),
		    "Unexpected Char");
	}
}

TEST_CASE("Invalid surrogates")
{
	// Utf-16 surrogate pairs have range (0xD800..0xDBFF, 0xDC00..0xDFFF)
	//                                    <----high---->  <-----low---->

	SECTION("Unpaired high surrogate")
	{
		string in = R"("\uD835")";
		REQUIRE_THROWS_WITH(
		    string out = QuantumJson::Parse(in),
		    "Unexpected Char");
	}

	SECTION("Low surrogate at start")
	{
		string in = R"("\uDC00")";
		REQUIRE_THROWS_WITH(
		    string out = QuantumJson::Parse(in),
		    "Invalid Surrogate");
	}

	SECTION("High surrogate followed by high surrogate")
	{
		string in = R"("\uD835\uD835")";
		REQUIRE_THROWS_WITH(
		    string out = QuantumJson::Parse(in),
		    "Invalid Surrogate");
	}
}

TEST_CASE("Parse List")
{
	string in = R"(["val1", "val2"])";
	vector<string> out = QuantumJson::Parse(in);
	REQUIRE(out.size() == 2);
	REQUIRE(out[0] == "val1");
	REQUIRE(out[1] == "val2");
}

TEST_CASE("Parse string map")
{
	string in = R"({
	    "key1": "val1",
	    "key2": "val2"
	})";
	map<string, string> out = QuantumJson::Parse(in);
	REQUIRE(out.size() == 2);
	REQUIRE(out.at("key1") == "val1");
	REQUIRE(out.at("key2") == "val2");
}

TEST_CASE("Parse list map")
{
	string in = R"({
	    "key1": ["val1", "val2"],
	    "key2": ["val6", "val7", "val8"]
	})";
	map<string, vector<string>> out = QuantumJson::Parse(in);
	REQUIRE(out.size() == 2);
	REQUIRE(out.at("key1").size() == 2);
	REQUIRE(out.at("key1")[0] == "val1");
	REQUIRE(out.at("key1")[1] == "val2");
	REQUIRE(out.at("key2").size() == 3);
	REQUIRE(out.at("key2")[0] == "val6");
	REQUIRE(out.at("key2")[1] == "val7");
	REQUIRE(out.at("key2")[2] == "val8");
}
