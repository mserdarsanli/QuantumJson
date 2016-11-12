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

#include "benchmark/Benchmark.hpp"

#include <rapidjson/document.h>

void Benchmark(int repeat, const std::string &input)
{
	BENCHMARK_BEGIN;

	for (int i = 0; i < repeat; ++i)
	{
		BENCHMARK_LOOP_BEGIN;

		rapidjson::Document document;
		document.Parse(input.c_str()); // TODO check if this is the right api

		#ifdef BENCHMARK_CHECK_CORRECTNESS
		std::string url1 = document["data"]["children"][0]["data"]["url"].GetString();
		std::string url25 = document["data"]["children"][24]["data"]["url"].GetString();
		int score1 = document["data"]["children"][0]["data"]["score"].GetInt();
		int score25 = document["data"]["children"][24]["data"]["score"].GetInt();

		CHECK(url1 == "http://i.imgur.com/RkeezA0.jpg");
		CHECK(score1 == 6607);
		CHECK(url25 == "https://www.youtube.com/watch?v=PMNFaAUs2mo");
		CHECK(score25 == 4679);
		#endif

		BENCHMARK_LOOP_END;
	}

	BENCHMARK_END;
}
