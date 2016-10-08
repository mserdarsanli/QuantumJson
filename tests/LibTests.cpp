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
#include "third_party/catch.hpp"

using namespace std;

TEST_CASE("Simple string", "[extract,string]")
{
	string in = "\"asd\"";
	string out;
	QuantumJsonImpl__::Parser<string::const_iterator> p(in.begin(), in.end());
	p.ParseValueInto(out);
	REQUIRE(out == "asd");
}

TEST_CASE("Unterminated string", "[extract,string]")
{
	string in = "\"asd";
	string out;
	QuantumJsonImpl__::Parser<string::const_iterator> p(in.begin(), in.end());
	p.ParseValueInto(out);
	REQUIRE( p.errorCode == QuantumJsonImpl__::ErrorCode::UnexpectedEOF );
}

TEST_CASE("Escape Characters", "[extract,string]")
{
	string in = R"("\\\/\b\f\n\r\t")";
	string expectedOut = "\\/\b\f\n\r\t";
	string out;
	QuantumJsonImpl__::Parser<string::const_iterator> p(in.begin(), in.end());
	p.ParseValueInto(out);
	REQUIRE(out == expectedOut);
}

TEST_CASE("Invalid Escape", "[extract,string]")
{
	string in = R"("\q")";
	string out;
	QuantumJsonImpl__::Parser<string::const_iterator> p(in.begin(), in.end());
	p.ParseValueInto(out);
	REQUIRE( p.errorCode == QuantumJsonImpl__::ErrorCode::InvalidEscape );
}

TEST_CASE("Unterminated Escape", "[extract,string]")
{
	string in = R"("qweewqew\)";
	string out;
	QuantumJsonImpl__::Parser<string::const_iterator> p(in.begin(), in.end());
	p.ParseValueInto(out);
	REQUIRE( p.errorCode == QuantumJsonImpl__::ErrorCode::UnexpectedEOF );
}

TEST_CASE("Unicode", "[extract,string]")
{
	string in = u8"\"ğüşiöçÖÇŞİĞÜIı\"";
	string out;
	QuantumJsonImpl__::Parser<string::const_iterator> p(in.begin(), in.end());
	p.ParseValueInto(out);
	REQUIRE(out == u8"ğüşiöçÖÇŞİĞÜIı");
}

TEST_CASE("Invalid Unicode Sequence", "[extract,string]")
{
	// String ends with no continuation character
	uint8_t invalid_utf8[] = { '"', 196, '"', 0 };
	string in(reinterpret_cast<const char*>(invalid_utf8));
	string out;
	QuantumJsonImpl__::Parser<string::const_iterator> p(in.begin(), in.end());
	p.ParseValueInto(out);
	REQUIRE( p.errorCode == QuantumJsonImpl__::ErrorCode::InvalidUtf8Sequence );
}

TEST_CASE("Unicode Escape", "[extract,string]")
{
	// TODO implement \uxxxx and \uxxxx\uxxxx characters
}

TEST_CASE("Invalid Unicode Escape", "[extract,string]")
{
	// TODO test forbidden \uxxxx\uxxxx surrogate pairs
	// TODO test invalid escapes
}

TEST_CASE("Parse List", "[extract,list]")
{
	string in = R"(["val1", "val2"])";
	vector<string> out;
	QuantumJsonImpl__::Parser<string::const_iterator> p(in.begin(), in.end());
	p.ParseValueInto(out);
	REQUIRE(out.size() == 2);
	REQUIRE(out[0] == "val1");
	REQUIRE(out[1] == "val2");
}
