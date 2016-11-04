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

#include "lib/Common.hpp"

#define CATCH_CONFIG_MAIN
#include "third_party/catch.hpp"

using namespace std;
using Serializer = QuantumJsonImpl__::Serializer<std::back_insert_iterator<std::string>>;

TEST_CASE("Serialize bool")
{
	string out;
	Serializer s(std::back_inserter(out));

	SECTION("true")
	{
		s.SerializeValue(true);
		REQUIRE(out == "true");
	}

	SECTION("false")
	{
		s.SerializeValue(false);
		REQUIRE(out == "false");
	}
}

TEST_CASE("Serialize int64")
{
	string out;
	Serializer s(std::back_inserter(out));

	SECTION("1")
	{
		int64_t v = 1;
		s.SerializeValue(v);
		REQUIRE(out == "1");
	}
	SECTION("0")
	{
		int64_t v = 0;
		s.SerializeValue(v);
		REQUIRE(out == "0");
	}
	SECTION("-1")
	{
		int64_t v = -1;
		s.SerializeValue(v);
		REQUIRE(out == "-1");
	}
	SECTION("min")
	{
		int64_t v = -9223372036854775807L;
		s.SerializeValue(v);
		REQUIRE(out == "-9223372036854775807");
	}
	SECTION("max")
	{
		int64_t v = 9223372036854775807L;
		s.SerializeValue(v);
		REQUIRE(out == "9223372036854775807");
	}
}

TEST_CASE("Serialize double")
{
	// Some of the values in this test are platform dependent
	// But since the significant digits are not maximal,
	// they should be same for most? of the platforms.
	string out;
	Serializer s(std::back_inserter(out));

	SECTION("0")
	{
		double v = 0.0;
		s.SerializeValue(v);
		REQUIRE(out == "0");
	}
	SECTION("1")
	{
		double v = 1.0;
		s.SerializeValue(v);
		REQUIRE(out == "1");
	}
	SECTION("-1")
	{
		double v = -1.0;
		s.SerializeValue(v);
		REQUIRE(out == "-1");
	}
	SECTION("100")
	{
		double v = 100.0;
		s.SerializeValue(v);
		REQUIRE(out == "100");
	}
	SECTION("-100")
	{
		double v = -100.0;
		s.SerializeValue(v);
		REQUIRE(out == "-100");
	}
	SECTION("0.01")
	{
		double v = 0.01;
		s.SerializeValue(v);
		REQUIRE(out == "0.01");
	}
	SECTION("-0.01")
	{
		double v = -0.01;
		s.SerializeValue(v);
		REQUIRE(out == "-0.01");
	}
	SECTION("0.3")
	{
		double v = 0.3;
		s.SerializeValue(v);
		REQUIRE(out == "0.3");
	}
	SECTION("0.4")
	{
		double v = 0.4;
		s.SerializeValue(v);
		REQUIRE(out == "0.4");
	}

	// Invalid numbers, represented as null
	SECTION("NaN")
	{
		double v = nan("");
		s.SerializeValue(v);
		REQUIRE(out == "null");
	}
	SECTION("+inf")
	{
		double v = numeric_limits<double>::infinity();
		s.SerializeValue(v);
		REQUIRE(out == "null");
	}
	SECTION("-inf")
	{
		double v = -numeric_limits<double>::infinity();
		s.SerializeValue(v);
		REQUIRE(out == "null");
	}

	// Min-Max numbers
	SECTION("min")
	{
		double v = numeric_limits<double>::min();
		s.SerializeValue(v);
		REQUIRE(out == "2.225073859e-308");
	}
	SECTION("-min")
	{
		double v = -numeric_limits<double>::min();
		s.SerializeValue(v);
		REQUIRE(out == "-2.225073859e-308");
	}
	SECTION("max")
	{
		double v = numeric_limits<double>::max();
		s.SerializeValue(v);
		REQUIRE(out == "1.797693135e+308");
	}
	SECTION("-max")
	{
		double v = -numeric_limits<double>::max();
		s.SerializeValue(v);
		REQUIRE(out == "-1.797693135e+308");
	}
	SECTION("epsilon")
	{
		double v = numeric_limits<double>::epsilon();
		s.SerializeValue(v);
		REQUIRE(out == "2.220446049e-16");
	}
	SECTION("-epsilon")
	{
		double v = -numeric_limits<double>::epsilon();
		s.SerializeValue(v);
		REQUIRE(out == "-2.220446049e-16");
	}
	SECTION("denorm_min")
	{
		double v = numeric_limits<double>::denorm_min();
		s.SerializeValue(v);
		REQUIRE(out == "4.940656458e-324");
	}
	SECTION("-denorm_min")
	{
		double v = -numeric_limits<double>::denorm_min();
		s.SerializeValue(v);
		REQUIRE(out == "-4.940656458e-324");
	}
}

TEST_CASE("Serialize string")
{
	string out;
	Serializer s(std::back_inserter(out));

	SECTION("empty")
	{
		std::string v = "";
		s.SerializeValue(v);
		REQUIRE(out == "\"\"");
	}
	SECTION("simple string")
	{
		std::string v = "qweqwqeweq";
		s.SerializeValue(v);
		REQUIRE(out == "\"qweqwqeweq\"");
	}
	SECTION("newline and tab")
	{
		std::string v = "r1c1\tr1c2\nr2c1\tr2c2\n";
		s.SerializeValue(v);
		REQUIRE(out == "\"r1c1\\tr1c2\\nr2c1\\tr2c2\\n\"");
	}
	SECTION("zero")
	{
		std::string v("asd\0qwe", 7);
		s.SerializeValue(v);
		REQUIRE(out == "\"asd\\u0000qwe\"");
	}
	SECTION("quote and backslash")
	{
		std::string v = "Quote: [\"] Backslash: [\\]";
		s.SerializeValue(v);
		REQUIRE(out == "\"Quote: [\\\"] Backslash: [\\\\]\"");
	}
	SECTION("other control chars")
	{
		std::string v = "\b\f\r";
		s.SerializeValue(v);
		REQUIRE(out == "\"\\b\\f\\r\"");
	}
}

TEST_CASE("Serialize list")
{
	string out;
	Serializer s(std::back_inserter(out));

	SECTION("empty")
	{
		vector<string> v;
		s.SerializeValue(v);
		REQUIRE(out == "[]");
	}
	SECTION("1 elem")
	{
		vector<string> v = { "qwe" };
		s.SerializeValue(v);
		REQUIRE(out == "[\"qwe\"]");
	}
	SECTION("2 elems")
	{
		vector<string> v = { "qwe", "asd" };
		s.SerializeValue(v);
		REQUIRE(out == "[\"qwe\",\"asd\"]");
	}
	SECTION("nested")
	{
		vector<vector<vector<string>>> v( 3, vector<vector<string>>( 2, vector<string>() ));
		v[1][0].push_back("a");
		v[2][1].push_back("b");
		v[2][1].push_back("c");
		s.SerializeValue(v);
		REQUIRE(out == "[[[],[]],[[\"a\"],[]],[[],[\"b\",\"c\"]]]");
	}
}

TEST_CASE("Serialize map")
{
	string out;
	Serializer s(std::back_inserter(out));

	SECTION("empty")
	{
		map<string,string> v;
		s.SerializeValue(v);
		REQUIRE(out == "{}");
	}
	SECTION("1 elem")
	{
		map<string, string> v = { { "k", "v" } };
		s.SerializeValue(v);
		REQUIRE(out == "{\"k\":\"v\"}");
	}
	SECTION("2 elems")
	{
		map<string, string> v = { { "k1", "v1" }, { "k2", "v2" } };
		s.SerializeValue(v);
		REQUIRE(out == "{\"k1\":\"v1\",\"k2\":\"v2\"}");
	}
	SECTION("map of lists")
	{
		map<string, vector<string>> v = { { "k1", {} }, { "k2", {"v1", "v2"} } };
		s.SerializeValue(v);
		REQUIRE(out == "{\"k1\":[],\"k2\":[\"v1\",\"v2\"]}");
	}
	SECTION("nested")
	{
		map<string, map<string, string>> v = { { "k1", {} }, { "k2", { {"k3", "v3"} } } };
		s.SerializeValue(v);
		REQUIRE(out == "{\"k1\":{},\"k2\":{\"k3\":\"v3\"}}");
	}
}
