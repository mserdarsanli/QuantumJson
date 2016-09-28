// The MIT License (MIT)
//
// Copyright (c) 2014 Milo Yip
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


// Note: TEST_DOUBLE lines below are copied from
// https://github.com/miloyip/nativejson-benchmark/blob/master/src/main.cpp

TEST_STRING("[\"\"]", "");
TEST_STRING("[\"Hello\"]", "Hello");
TEST_STRING("[\"Hello\\nWorld\"]", "Hello\nWorld");
TEST_STRING("[\"Hello\\u0000World\"]", "Hello\0World");
TEST_STRING("[\"\\\"\\\\/\\b\\f\\n\\r\\t\"]", "\"\\/\b\f\n\r\t");
TEST_STRING("[\"\\u0024\"]", "\x24");         // Dollar sign U+0024
TEST_STRING("[\"\\u00A2\"]", "\xC2\xA2");     // Cents sign U+00A2
TEST_STRING("[\"\\u20AC\"]", "\xE2\x82\xAC"); // Euro sign U+20AC
TEST_STRING("[\"\\uD834\\uDD1E\"]", "\xF0\x9D\x84\x9E"); // G clef sign U+1D11E
