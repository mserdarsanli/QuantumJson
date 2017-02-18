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

#include <cstdlib>
#include <fstream>
#include <iostream>
#include <iterator>
#include <string>
#include <vector>

#include "CodeGenerator.hpp"
#include "Parser.hpp"
#include "Tokenizer.hpp"

#include "src/CommandLineFlags.hpp"

using namespace std;

int main(int argc, char* argv[])
{
	gengetopt_args_info args;
	if (cmdline_parser (argc, argv, &args) != 0)
	{
		exit(1);
	}

	ifstream iSource(args.in_arg);
	if (!iSource.is_open())
	{
		cerr << "Unable to open input file " << args.in_arg << "\n";
		return 1;
	}
	string input{istreambuf_iterator<char>(iSource),
	             istreambuf_iterator<char>()};
	if (iSource.bad())
	{
		cerr << "Read error on file " << args.in_arg << "\n";
		return 1;
	}

	ParsedFile f = Parse(Tokenize(input));

	ofstream oHeader(args.out_arg);
	if (!oHeader.is_open())
	{
		cerr << "Unable to open output file " << args.out_arg << "\n";
		return 1;
	}

	GenerateHeaderForFile(oHeader, f);

	oHeader.flush();
	if (oHeader.bad())
	{
		cerr << "Write error on file " << args.out_arg << "\n";
		return 1;
	}

	return 0;
}