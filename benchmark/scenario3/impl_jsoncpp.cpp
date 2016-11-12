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

#include <json/json.h>

void Benchmark(int repeat, const std::string &input)
{
	BENCHMARK_BEGIN;

	for (int i = 0; i < repeat; ++i)
	{
		BENCHMARK_LOOP_BEGIN;

		Json::Value root;
		Json::Reader reader;
		reader.parse(input, root);

		#ifdef BENCHMARK_CHECK_CORRECTNESS

		std::string user1_name = root["items"][0]["display_name"].asString();
		std::string user1_image = root["items"][0]["profile_image"].asString();
		int user1_bronze = root["items"][0]["badge_counts"]["bronze"].asInt();
		std::string user30_name = root["items"][29]["display_name"].asString();
		std::string user30_image = root["items"][29]["profile_image"].asString();
		int user30_bronze = root["items"][29]["badge_counts"]["bronze"].asInt();

		// Check first and last values
		CHECK(user1_name == "Jon Skeet");
		CHECK(user1_image == "https://www.gravatar.com/avatar/6d8ebb117e8d83d74ea95fbdd0f87e13?s=128&d=identicon&r=PG");
		CHECK(user1_bronze == 7502);
		CHECK(user30_name == "Gumbo");
		CHECK(user30_image == "https://www.gravatar.com/avatar/cd501083459cbc21fccae78e2d03bee2?s=128&d=identicon&r=PG");
		CHECK(user30_bronze == 669);

		#endif

		BENCHMARK_LOOP_END;
	}

	BENCHMARK_END;
}
