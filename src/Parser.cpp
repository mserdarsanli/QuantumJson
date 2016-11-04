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
#include <iostream>
#include <stdexcept>

#include "Attributes.hpp"
#include "Parser.hpp"
#include "Tokenizer.hpp"

using namespace std;

using TokenIt = vector<Token>::const_iterator;

void AssertToken(TokenIt it, Token::Type t)
{
	if (it->type != t)
	{
		cerr << "Assertion failed: Expected token " << (int)t << "\n";
		exit(1);
	}
}

void AssertToken(TokenIt it, Token::Type t, string val)
{
	if (it->type != t || it->strValue != val)
	{
		cerr << "Assertion failed: Expected token " << (int)t << " " << val << "\n";
		exit(1);
	}
}

// If the type is one of the reserved ones like `string`
// It will be converted to C++ equivalent like `std::string`
static void MakeStandardType(VariableTypeDef* type)
{
	if (type->typeName == "string" && type->of.size() == 0)
	{
		type->typeName = "std::string";
		return;
	}
	if (type->typeName == "vector" && type->of.size() > 0)
	{
		type->typeName = "std::vector";
		return;
	}
	if (type->typeName == "map" && type->of.size() > 0)
	{
		type->typeName = "std::map";
		return;
	}
}

TokenIt ParseVariableType(TokenIt it, TokenIt end, VariableTypeDef* vtOut)
{
	AssertToken(it, Token::Type::Name);
	vtOut->typeName = it->strValue;
	++it;

	if (it->type == Token::Type::TemplateOpen)
	{
		++it;
		vtOut->of.emplace_back();
		it = ParseVariableType(it, end, &vtOut->of.back());

		while (it->type == Token::Type::Comma)
		{
			++it;
			vtOut->of.emplace_back();
			it = ParseVariableType(it, end, &vtOut->of.back());
		}

		AssertToken(it++, Token::Type::TemplateClose);
	}

	MakeStandardType(vtOut);

	return it;
}


// [[ attr1("val1"), attr2("val2") ]]
TokenIt ParseAttributes(TokenIt it, TokenIt end, map< string, AttributeDef > *attributes)
{
	attributes->clear();
	AssertToken(it, Token::Type::AttributeOpen);
	++it;

	bool firstAttr = true;

	while (it->type != Token::Type::AttributeClose)
	{
		if (firstAttr)
		{
			firstAttr = false;
		}
		else
		{
			AssertToken(it, Token::Type::Comma);
			++it;
		}

		AttributeDef attr;
		AssertToken(it, Token::Type::Name);
		attr.name = it->strValue;
		++it;

		// Check if the attribute is known
		const AttributeInfo *knownAttribute = GetKnownAttribute(attr.name);
		if (!knownAttribute)
		{
			throw runtime_error("Unknown attribute: [" + attr.name + "]");
		}
		if (attributes->find(attr.name) != attributes->end())
		{
			throw runtime_error("Duplicate attribute: [" + attr.name + "]");
		}

		if (it->type == Token::Type::ParenthesesOpen)
		{
			++it;

			AssertToken(it, Token::Type::String);
			string arg = it->strValue;
			++it;
			attr.args.push_back(arg);

			while (it->type == Token::Type::Comma)
			{
				++it;

				AssertToken(it, Token::Type::String);
				string arg = it->strValue;
				++it;
				attr.args.push_back(arg);
			}

			AssertToken(it, Token::Type::ParenthesesClose);
			++it;
		}

		if (attr.args.size() != knownAttribute->arg_count)
		{
			throw runtime_error("Unexpected number of args for attribute: [" + attr.name + "]");
		}

		(*attributes)[attr.name] = attr;
	}

	AssertToken(it, Token::Type::AttributeClose);
	++it;
	return it;
}

TokenIt ParseVariableDef(TokenIt it, TokenIt end, VariableDef *vOut)
{
	it = ParseVariableType(it, end, &vOut->type);

	AssertToken(it, Token::Type::Name);
	vOut->name = it->strValue;
	++it;

	if (it->type == Token::Type::AttributeOpen)
	{
		it = ParseAttributes(it, end, &vOut->attributes);
	}

	while (it->type != Token::Type::Semicolon)
	{
		++it;
	}

	AssertToken(it++, Token::Type::Semicolon);
	return it;
}

TokenIt ParseStructDef(TokenIt it, TokenIt end, StructDef* sOut)
{
	AssertToken(it++, Token::Type::Name, "struct");
	AssertToken(it, Token::Type::Name);
	sOut->name = it->strValue;
	++it;
	AssertToken(it++, Token::Type::BracesOpen);

	// Assume struct defns can not be nested
	while (it->type != Token::Type::BracesClose)
	{
		// Anything inside a struct should be a variable definition ?
		sOut->variables.resize(sOut->variables.size() + 1);
		it = ParseVariableDef(it, end, &sOut->variables[sOut->variables.size() - 1]);
	}

	AssertToken(it++, Token::Type::BracesClose);
	AssertToken(it++, Token::Type::Semicolon);
	return it; // Return next token
}

// Parses a tokenized file, and returns extracted `ParsedFile`
ParsedFile Parse(const vector<Token> &tokens)
{
	ParsedFile f;
	TokenIt begin = tokens.begin();
	TokenIt end   = tokens.end();

	while (begin != end)
	{
		const Token &nextToken = *begin;

		if (nextToken.type == Token::Type::Name &&
		    nextToken.strValue == "struct")
		{
			f.structs.resize(f.structs.size() + 1);
			begin = ParseStructDef(begin, end, &f.structs[f.structs.size() - 1]);
			continue;
		}

		cerr << "Unable to understand next token: " << (int)begin->type << ": " << begin->strValue << "\n";
		exit(1);
	}

	return f;
}
