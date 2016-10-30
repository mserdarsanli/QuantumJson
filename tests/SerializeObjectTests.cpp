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

#include <cmath>
#include <iostream>
#include <iterator>
#include <string>

#include "tests/Schema1.gen.hpp"

#define CATCH_CONFIG_MAIN
#include "third_party/catch.hpp"

TEST_CASE("Serialize object simple", "[serialize,object]")
{
	std::string out;

	Item obj;
	obj.name = "qwe";

	obj.SerializeTo(std::back_inserter(out));

	REQUIRE(out == "{\"name\":\"qwe\"}");
}

TEST_CASE("Serialize object nested", "[serialize,object]")
{
	std::string out;

	Items obj;
	obj.i1.name = "name1";
	obj.i2.name = "name2";

	obj.SerializeTo(std::back_inserter(out));

	REQUIRE(out == "{\"i1\":{\"name\":\"name1\"},\"i2\":{\"name\":\"name2\"}}");
}

TEST_CASE("Serialize object attr", "[serialize,object]")
{
	std::string out;

	AttributeTester obj;
	obj.attr1 = "a1";
	obj.attr2 = "a2";

	obj.SerializeTo(std::back_inserter(out));

	REQUIRE(out == "{\"attr1\":\"a1\",\"attr-2\":\"a2\"}");
}
