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


struct GlobalObj
{
};

namespace foo {

struct ObjInFoo
{

};

namespace bar {

struct ObjInFooBar
{
};


} // namespace bar
} // namespace foo

namespace foo::bar {

namespace a::b {
} // namespace a::b

namespace baz {

struct ObjInFooBarBaz
{
};

} // namespace baz

struct ObjInFooBar2
{
	GlobalObj   go_1;
	::GlobalObj go_2;

	foo::ObjInFoo f_1;
	::foo::ObjInFoo f_2;

	foo::bar::ObjInFooBar fb_1;
	::foo::bar::ObjInFooBar fb_2;
	ObjInFooBar fb_3;


	foo::bar::baz::ObjInFooBarBaz fbb_1;
	::foo::bar::baz::ObjInFooBarBaz fbb_2;
	baz::ObjInFooBarBaz fbb_3;

	string s_1;
	std::string s_2;

	std::vector< std::string > vs_1;
	vector<string> vs_2;
};

} // namespace foo::bar
