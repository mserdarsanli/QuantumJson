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

#include <fstream>
#include <iostream>
#include <iterator>
#include <string>
#include <vector>

// TODO Work-around not being able to include the base lib stand alone
#include "lib/Common.hpp"

using namespace std;

// Program to test skipping JSON values
// returns 0 for skipped json values, non-zero on failure.
int main(int argc, char *argv[])
{
	vector<string> args{ argv, argv+argc };

	bool exit_status = 0;
	for (size_t i = 1; i < args.size(); i += 2)
	{
		bool expect_pass;
		if (args[i] == "--pass")
		{
			expect_pass = true;
		}
		else if (args[i] == "--fail")
		{
			expect_pass = false;
		}
		else
		{
			cerr << "ERROR: Unknown option " << args[i] << "\n";
			return 1;
		}

		ifstream jsonFile( args[i+1] );
		if (!jsonFile.is_open())
		{
			cerr << "ERROR: Unable to open json file " << args[i+1] << "\n";
			return 1;
		}

		string jsonStr{ istream_iterator<char>(jsonFile),
		                istream_iterator<char>()};


		// Expect exception on incorrect values
		bool json_correct;

		QuantumJsonImpl__::Parser< decltype(jsonStr.begin()) > p(jsonStr.begin(), jsonStr.end());
		p.SkipValue();
		json_correct = (p.errorCode == QuantumJsonImpl__::ErrorCode::NoError);

		if (expect_pass == json_correct)
		{
			std::cout << "\033[32m[PASS]\033[0m json test " << args[i+1] << "\n";
		}
		else
		{
			std::cout << "\033[31m[FAIL]\033[0m json test " << args[i+1] << "\n";
			exit_status = 1;
		}
	}

	return exit_status;
}
