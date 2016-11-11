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

#define CATCH_CONFIG_MAIN
#include <catch.hpp>

#include "Parser.hpp"
#include "Tokenizer.hpp"

using namespace std;

TEST_CASE( "Multiple template arguments" )
{
	string input = R"(
	    struct Elem
	    {
	        int a;
	    };

	    struct Test
	    {
	        map<string, string> x;
	        map< string, Elem > elems;
	    };
	)";

	ParsedFile f = Parse(Tokenize(input));

	const auto &varX = f.structs[1].variables[0];
	const auto &varE = f.structs[1].variables[1];

	REQUIRE(varX.name == "x");
	REQUIRE(varX.type.typeName == "std::map");
	REQUIRE(varX.type.of[0].typeName == "std::string");
	REQUIRE(varX.type.of[1].typeName == "std::string");

	REQUIRE(varE.name == "elems");
	REQUIRE(varE.type.typeName == "std::map");
	REQUIRE(varE.type.of[0].typeName == "std::string");
	REQUIRE(varE.type.of[1].typeName == "Elem");
}
