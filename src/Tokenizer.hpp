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

#pragma once

#include <iostream>
#include <string>
#include <vector>

struct Token
{
	enum class Type
	{
		Unknown,
		EndOfFile, // Helper token to prevent dereferencing invalid iterator
		Name,
		TemplateOpen,
		TemplateClose,
		ParenthesesOpen,
		ParenthesesClose,
		BracesOpen,
		BracesClose,
		Semicolon,
		AttributeOpen,
		AttributeClose,
		String,
		Comma,
		NamespaceSeparator,
		KeywordNamespace,
		KeywordStruct,
	};

	Type type = Type::Unknown;
	// For tokens that has value, like string or name tokens
	std::string strValue;

	int line = -1;
	int col = -1;
};

std::ostream& operator<<(std::ostream &out, const Token &t);
std::vector<Token> Tokenize(const std::string &in);
