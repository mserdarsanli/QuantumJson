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

#define CATCH_CONFIG_MAIN
#include "third_party/catch.hpp"

using namespace std;

#define PARSE(data) ParseJsonString(reinterpret_cast<const char*>(data), \
                                    reinterpret_cast<const char*>(data) + sizeof(data) )

string ParseJsonString(const char *beg, const char *end)
{
	string out;

	QuantumJsonImpl__::Parser<const char *> p(beg, end);
	p.ParseValueInto(out);
	if (p.errorCode != QuantumJsonImpl__::ErrorCode::NoError)
	{
		throw p.errorCode;
	}
	return out;
}

TEST_CASE("Case 1", "[utf8,decoder]")
{
	unsigned char input[] = { '"',
	    0xce, 0xba, 0xe1, 0xbd, 0xb9, 0xcf, 0x83, 0xce, 0xbc, 0xce, 0xb5,
	'"', };

	string out = PARSE(input);
	REQUIRE( out == u8"κόσμε" );
}

// TODO Add case 2.1
// TODO Add case 2.1.1
// TODO Add case 2.1.2
// TODO Add case 2.1.3
// TODO Add case 2.1.4
// TODO Add case 2.1.5
// TODO Add case 2.1.6
// TODO Add case 2.2.1
// TODO Add case 2.2.2
// TODO Add case 2.2.3
// TODO Add case 2.2.4
// TODO Add case 2.2.5
// TODO Add case 2.2.6
// TODO Add case 2.3.1
// TODO Add case 2.3.2
// TODO Add case 2.3.3
// TODO Add case 2.3.4
// TODO Add case 2.3.5
// TODO Add case 3.1.1
// TODO Add case 3.1.2
// TODO Add case 3.1.3
// TODO Add case 3.1.4
// TODO Add case 3.1.5
// TODO Add case 3.1.6
// TODO Add case 3.1.7
// TODO Add case 3.1.8
// TODO Add case 3.1.9
// TODO Add case 3.2.1
// TODO Add case 3.2.2
// TODO Add case 3.2.3
// TODO Add case 3.2.4
// TODO Add case 3.2.5
// TODO Add case 3.3.1
// TODO Add case 3.3.2
// TODO Add case 3.3.3
// TODO Add case 3.3.4
// TODO Add case 3.3.5
// TODO Add case 3.3.6
// TODO Add case 3.3.7
// TODO Add case 3.3.8
// TODO Add case 3.3.9
// TODO Add case 3.3.10
// TODO Add case 3.4
// TODO Add case 3.5.1
// TODO Add case 3.5.2
// TODO Add case 3.5.3
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
