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
#include <string_view>
#include <vector>

#include "CodeGenerator.hpp"
#include "Parser.hpp"
#include "Tokenizer.hpp"
#include "Util.hpp"

using namespace std;

int main(int argc, char* argv[])
{
	using namespace std::literals;

	std::string input_file_name, output_file_name;

	for ( int i = 1; i < argc; )
	{
		if ( argv[ i ] == "--in"sv )
		{
			input_file_name = argv[ i + 1 ];
			i += 2;
			continue;
		}
		if ( argv[ i ] == "--out"sv )
		{
			output_file_name = argv[ i + 1 ];
			i += 2;
			continue;
		}
		std::cerr << "Unknown flag: " << argv[ i ] << "\n";
		return 1;
	}

	ifstream iSource( input_file_name );
	if (!iSource.is_open())
	{
		cerr << "Unable to open input file " << input_file_name << "\n";
		return 1;
	}
	string input{istreambuf_iterator<char>(iSource),
	             istreambuf_iterator<char>()};
	if (iSource.bad())
	{
		cerr << "Read error on file " << input_file_name << "\n";
		return 1;
	}

	ParsedFile f = Parse(Tokenize(input));

	ofstream oHeader( output_file_name );
	if (!oHeader.is_open())
	{
		cerr << "Unable to open output file " << output_file_name << "\n";
		return 1;
	}

	CodeFormatter code;
	GenerateHeaderForFile(code, f);
	oHeader << code.getFormattedCode();

	oHeader.flush();
	if (oHeader.bad())
	{
		cerr << "Write error on file " << output_file_name << "\n";
		return 1;
	}

	return 0;
}
