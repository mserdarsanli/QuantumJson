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

#include <catch2/catch.hpp>

#include "Parser.hpp"
#include "Tokenizer.hpp"

using namespace std;

TEST_CASE( "Unknown attribute should be an error" )
{
	string input = R"(
	    struct Test
	    {
	        int x [[ attr_qwe("smth") ]];
	    };
	)";
	REQUIRE_THROWS_WITH( Parse(Tokenize(input)), "Unknown attribute: [attr_qwe]" );
}

TEST_CASE( "Duplicate attribute should be an error" )
{
	string input = R"(
	    struct Test
	    {
	        int x [[ on_null("smth"), json_field_name("q"), on_null("smth_else")]];
	    };
	)";
	REQUIRE_THROWS_WITH( Parse(Tokenize(input)), "Duplicate attribute: [on_null]" );
}

TEST_CASE( "Attribute with no args gets parsed correctly" )
{
	string input = R"(
	    struct Test
	    {
	        int x [[ deprecated, on_null("qwe") ]];
	    };
	)";

	ParsedFile f = Parse(Tokenize(input));

	const auto &varX = f.structs[0].variables[0];
	REQUIRE(varX.type.typeName == "int");
	REQUIRE(varX.name == "x");

	REQUIRE(varX.attributes.find("deprecated") != varX.attributes.end());
	REQUIRE(varX.attributes.at("deprecated").args.size() == 0);

	REQUIRE(varX.attributes.find("on_null") != varX.attributes.end());
	REQUIRE(varX.attributes.at("on_null").args.size() == 1);
	REQUIRE(varX.attributes.at("on_null").args[0] == "qwe");
}

TEST_CASE( "Attribute with multiple args gets parsed correctly" )
{
	string input = R"(
	    struct Test
	    {
	        int x [[ three_arg_attr("a1", "b2", "c3") ]];
	    };
	)";

	ParsedFile f = Parse(Tokenize(input));

	const auto &varX = f.structs[0].variables[0];
	REQUIRE(varX.type.typeName == "int");
	REQUIRE(varX.name == "x");

	REQUIRE(varX.attributes.find("three_arg_attr") != varX.attributes.end());
	REQUIRE(varX.attributes.at("three_arg_attr").args.size() == 3);
	REQUIRE(varX.attributes.at("three_arg_attr").args[0] == "a1");
	REQUIRE(varX.attributes.at("three_arg_attr").args[1] == "b2");
	REQUIRE(varX.attributes.at("three_arg_attr").args[2] == "c3");
}

TEST_CASE( "Attribute with incorect number of args should be an error" )
{
	string input = R"(
	    struct Test
	    {
	        int x [[ on_null ]];
	    };
	)";

	REQUIRE_THROWS_WITH( Parse(Tokenize(input)), "Unexpected number of args for attribute: [on_null]" );
}

// TODO add test for class attribute
