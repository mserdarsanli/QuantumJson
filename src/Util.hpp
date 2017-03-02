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

#ifndef UTIL_HPP
#define UTIL_HPP

#include <cstdarg>
#include <memory>
#include <sstream>
#include <string>

// Very simple class which behaves like an output stream.
//
//     CodeFormatter code;
//     code.EmitLine("int f(int a, int b)");
//     code.EmitLine("{"); // Magic line, increases indentation
//     code.EmitLine("return a+b;");
//     code.EmitLine("}"); // Magic line, decreases indentation
//     code.getFormattedCode();
//
// should return:
//
//     int f(int a, int b)
//     {
//         return a+b;
//     }
//
// note, actual indentation would use tabs.
//
// `EmitLine` also accepts `printf` style format:
//
//     code.EmitLine("goto state_%s", 123);
//
// This is usefue as `CodeFormatter` object can be passed by reference between
// contexts, and no explicit indentation/endl handling would be required.
class CodeFormatter
{
public:
	CodeFormatter() = default;
	CodeFormatter(const CodeFormatter &) = delete;
	CodeFormatter(CodeFormatter &&) = delete;
	CodeFormatter& operator=(const CodeFormatter &ot) = delete;

	void EmitLine(const char *fmt, ...)
	{
		int size;

		// Handle special cases
		if (fmt == std::string("{"))
		{
			codeBuffer << std::string(curIndent, '\t') << "{\n";
			++curIndent;
			return;
		}
		if (fmt == std::string("}") || fmt == std::string("};"))
		{
			--curIndent;
			codeBuffer << std::string(curIndent, '\t') << fmt << "\n";
			return;
		}

		// Get the size needed for format
		{
			va_list ap;
			va_start(ap, fmt);
			size = vsnprintf(NULL, 0, fmt, ap);
			va_end(ap);
		}

		std::unique_ptr<char> buf(new char[size+1]);

		{
			va_list ap;
			va_start(ap, fmt);
			vsnprintf(buf.get(), size+1, fmt, ap);
			va_end(ap);
		}

		codeBuffer << std::string(curIndent, '\t') << buf.get() << '\n';
	}

	void EmitCode(const std::string &snippet)
	{
		char lastCh = '\n';

		for (char c : snippet)
		{
			if (lastCh == '\n')
			{
				codeBuffer << std::string(curIndent, '\t');
			}

			codeBuffer << c;
			lastCh = c;
		}
	}

	std::string getFormattedCode()
	{
		return codeBuffer.str();
	}

private:
	int curIndent = 0;
	std::stringstream codeBuffer;
};

#endif // UTIL_HPP
