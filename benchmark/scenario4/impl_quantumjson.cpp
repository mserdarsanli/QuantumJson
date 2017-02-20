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

#include <iostream>
#include "benchmark/schema/TFL.gen.hpp"

void Benchmark(int repeat, const std::string &input)
{
	BENCHMARK_BEGIN;

	for (int i = 0; i < repeat; ++i)
	{
		BENCHMARK_LOOP_BEGIN;

		std::vector<Accident> res = QuantumJson::Parse(input);

		#ifdef BENCHMARK_CHECK_CORRECTNESS

		std::string lastacc_dtype       = res[25192].data_type;
		std::string lastacc_date        = res[25192].date;
		double lastacc_lat              = res[25192].lat;
		double lastacc_lon              = res[25192].lon;
		std::string lastacc_v2_type     = res[25192].vehicles[1].type;
		std::string lastacc_cas1_mode   = res[25192].casualties[0].mode;
		int lastacc_cas1_age            = res[25192].casualties[0].age;

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
