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

#include <catch2/catch.hpp>

#include "Util.hpp"

using namespace std;

TEST_CASE("CodeFormatter")
{
	CodeFormatter code;

	code.EmitLine("int sum(int a, int b)");
	code.EmitLine("{");
		code.EmitLine("return a + %d + b;", 111);
	code.EmitLine("}");
	code.EmitLine("// done");

	string expectedOut = 1 + R"(
int sum(int a, int b)
{
	return a + 111 + b;
}
// done
)";

	REQUIRE(expectedOut == code.getFormattedCode());
}

TEST_CASE("CodeFormatter indent")
{
	CodeFormatter code;

	code.EmitLine("{");
	code.EmitLine("{");
	code.EmitLine("{");
	code.EmitLine("}");
	code.EmitLine("}");
	code.EmitLine("}");
	string expectedOut = 1 + R"(
{
	{
		{
		}
	}
}
)";

	REQUIRE(expectedOut == code.getFormattedCode());
}
