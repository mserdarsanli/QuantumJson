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

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

namespace pt = boost::property_tree;

void Benchmark(int repeat, const std::string &input)
{
	BENCHMARK_BEGIN;

	for (int i = 0; i < repeat; ++i)
	{
		BENCHMARK_LOOP_BEGIN;

		// TODO boost property_tree does not read from string directly.
		// So this may not be a fair comparison.
		std::stringstream ss;
		ss.str(input);

		pt::ptree root;
		pt::read_json(ss, root);

		#ifdef BENCHMARK_CHECK_CORRECTNESS

		std::string url1, url25;
		int score1, score25;

		// boost_pt has a horrible api, not allowing random access to
		// list elements.
		int n = 0;
		for (const auto &ch : root.get_child("data.children"))
		{
			++n;
			if (n == 1)
			{
				url1 = ch.second.get<std::string>("data.url");
				score1 = ch.second.get<int>("data.score");
			}
			if (n == 25)
			{
				url25 = ch.second.get<std::string>("data.url");
				score25 = ch.second.get<int>("data.score");
			}
		}

		// Check first and last values
		CHECK(url1 == "http://i.imgur.com/RkeezA0.jpg");
		CHECK(score1 == 6607);
		CHECK(url25 == "https://www.youtube.com/watch?v=PMNFaAUs2mo");
		CHECK(score25 == 4679);

		#endif

		BENCHMARK_LOOP_END;
	}

	BENCHMARK_END;
}
