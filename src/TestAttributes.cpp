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
#include "third_party/catch.hpp"

#include "Parser.hpp"
#include "Tokenizer.hpp"

using namespace std;

TEST_CASE( "Unknown attribute should be an error" )
{
	string input = R"(
	    struct Test
	    {
	        int x [[ unknown_field_attribute("smth") ]];
	    };
	)";
	REQUIRE_THROWS_WITH( Parse(Tokenize(input)), "Unknown attribute" );
}

// TODO add test for conflicting attributes
// TODO add test for attribute with no value
// TODO add test for attribute with multiple values
// TODO add test for class attribute
