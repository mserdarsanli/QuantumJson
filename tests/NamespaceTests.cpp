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

#include <iostream>

#include "tests/SchemaNS.gen.hpp"

#include <catch2/catch.hpp>

using namespace std;

TEST_CASE("Tests structs in namespaces are defined correctly")
{
	REQUIRE( sizeof(GlobalObj)                     > 0 );
	REQUIRE( sizeof(foo::ObjInFoo)                 > 0 );
	REQUIRE( sizeof(foo::bar::ObjInFooBar)         > 0 );
	REQUIRE( sizeof(foo::bar::ObjInFooBar2)        > 0 );
	REQUIRE( sizeof(foo::bar::baz::ObjInFooBarBaz) > 0 );
}

TEST_CASE("Test fields with types in namespaces")
{
	foo::bar::ObjInFooBar2 obj;

	using foo::ObjInFoo;
	using foo::bar::ObjInFooBar;
	using foo::bar::baz::ObjInFooBarBaz;

	REQUIRE( typeid(obj.go_1) == typeid(GlobalObj) );
	REQUIRE( typeid(obj.go_2) == typeid(GlobalObj) );

	REQUIRE( typeid(obj.f_1) == typeid(ObjInFoo) );
	REQUIRE( typeid(obj.f_2) == typeid(ObjInFoo) );

	REQUIRE( typeid(obj.fb_1) == typeid(ObjInFooBar) );
	REQUIRE( typeid(obj.fb_2) == typeid(ObjInFooBar) );
	REQUIRE( typeid(obj.fb_3) == typeid(ObjInFooBar) );

	REQUIRE( typeid(obj.fbb_1) == typeid(ObjInFooBarBaz) );
	REQUIRE( typeid(obj.fbb_2) == typeid(ObjInFooBarBaz) );
	REQUIRE( typeid(obj.fbb_3) == typeid(ObjInFooBarBaz) );

	REQUIRE( typeid(obj.s_1) == typeid(string) );
	REQUIRE( typeid(obj.s_2) == typeid(string) );

	REQUIRE( typeid(obj.vs_1) == typeid(vector<string>) );
	REQUIRE( typeid(obj.vs_2) == typeid(vector<string>) );
}
