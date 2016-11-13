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

		std::string lastacc_dtype;
		std::string lastacc_date;
		double lastacc_lat = -1;
		double lastacc_lon = -1;
		std::string lastacc_v2_type;
		std::string lastacc_cas1_mode;
		int lastacc_cas1_age = -1;

		// boost_pt has a horrible api, not allowing random access to
		// list elements.
		int n = 0;
		for (const auto &ch : root)
		{
			++n;
			if (n == 25193)
			{
				lastacc_dtype = ch.second.get<std::string>("$type");
				lastacc_date = ch.second.get<std::string>("date");
				lastacc_lat = ch.second.get<double>("lat");
				lastacc_lon = ch.second.get<double>("lon");

				int i = 0;
				for (const auto &it : ch.second.get_child("vehicles"))
				{
					++i;
					if (i == 2)
					{
						lastacc_v2_type = it.second.get<std::string>("type");
					}
				}

				i = 0;
				for (const auto &it : ch.second.get_child("casualties"))
				{
					++i;
					if (i == 1)
					{
						lastacc_cas1_mode = it.second.get<std::string>("mode");
						lastacc_cas1_age = it.second.get<int>("age");
					}
				}
			}
		}

		// Check last accident
		CHECK(lastacc_dtype == "Tfl.Api.Presentation.Entities.AccidentStats.AccidentDetail, Tfl.Api.Presentation.Entities");
		CHECK(lastacc_date == "2015-11-18T13:45:00Z");
		CHECK(abs(lastacc_lat - 51.609686) < 0.000001);
		CHECK(abs(lastacc_lon - -0.102475) < 0.000001);
		CHECK(lastacc_v2_type == "LightGoodsVehicle");
		CHECK(lastacc_cas1_mode == "Car");
		CHECK(lastacc_cas1_age == 36);

		#endif

		BENCHMARK_LOOP_END;
	}

	BENCHMARK_END;
}
