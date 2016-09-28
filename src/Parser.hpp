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

#include <map>
#include <string>
#include <vector>

#include "Tokenizer.hpp"

struct VariableTypeDef
{
	std::string typeName;
	std::vector<VariableTypeDef> of; // template parameters

	void Print(int indent) const
	{
		std::cout << std::string(indent, ' ') << typeName << "\n";
		for (const auto &o : of)
			o.Print(indent + 4);
	}

	void Print() const
	{
		Print(0);
	}
};

struct VariableDef
{
	VariableTypeDef type;
	std::string name;

	std::map< std::string, std::string > attributes;
};

struct StructDef
{
	std::string name;
	std::vector<VariableDef> variables;

	void Print() const
	{
		std::cout << "Struct [" << name << "]\n";
		for (const VariableDef &v : variables)
		{
			std::cout << "   " << v.name << ":\n";
			v.type.Print(5);
		}
	}
};

struct ParsedFile
{
	std::vector<StructDef> structs;

	void Print() const
	{
		std::cout << "Parsed file\n";
		for (const StructDef &s : structs)
		{
			s.Print();
		}
	}
};

ParsedFile Parse(const std::vector<Token> &tokens);
