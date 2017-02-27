// The MIT License (MIT)
//
// Copyright (c) 2017 Mustafa Serdar Sanli
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

#define CATCH_CONFIG_MAIN
#include <catch.hpp>

#include "FieldParser.hpp"

using namespace std;

TEST_CASE("Test no duplicate fields")
{
	FieldParser fp;
	fp.addField("id", "parser.ParseValueInto(this->id);");
	fp.addField("name", "parser.ParseValueInto(this->name);");

	REQUIRE_THROWS_WITH(
	    fp.addField("name", "parser.ParseValueInto(this->qweqe);"),
	    "Field already present: name");
}

TEST_CASE("Test generated code simple")
{
	FieldParser fp;
	fp.addField("id", "parser.ParseValueInto(this->id);");
	fp.addField("name", "parser.ParseValueInto(this->name);");
	fp.addField("nam", "parser.ParseValueInto(this->nam);");
	fp.addField("two-words", "parser.ParseValueInto(this->twoWords);");

	string code = fp.generateFieldParserCode();

	// Simple sanity checks
	REQUIRE(code.find("parser.ParseValueInto(this->twoWords);") != string::npos);
	REQUIRE(code.find("state_unknown_field_matching:") != string::npos);
	REQUIRE(code.find("state_unknown_field_matched:") != string::npos);
}
