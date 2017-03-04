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

#include <iostream>
#include <string>
#include <vector>

#include "Tokenizer.hpp"

using namespace std;

vector<Token> Tokenize(const string &in)
{
	vector<Token> res;

	for (size_t i = 0; i < in.size(); ++i)
	{
		if (isspace(in[i]))
		{
			continue;
		}

		// Skip Comments
		if (in[i] == '/' && in[i+1] == '/')
		{
			while (in[i] && in[i] != '\n')
				++i;
			continue;
		}

		if (in[i] == '/' && in[i+1] == '*')
		{
			i += 2;

			while (1)
			{
				if (!in[i] || !in[i+1])
				{
					cerr << "Unexpected EOF in multi line comment\n";
					std::exit(1);
				}
				if (in[i] == '*' && in[i+1] == '/')
				{
					++i;
					goto end_comment;
				}
				++i;
			}

			end_comment:
			continue;
		}


		// A typename or a keyword
		if (isalpha(in[i]))
		{
			string word(1, in[i]);
			while (isalnum(in[i+1]) || '_' == in[i+1])
			{
				word.push_back(in[++i]);
			}

			// Handle keywords
			if (word == "namespace")
			{
				res.emplace_back(Token::Type::KeywordNamespace);
			}
			else if (word == "struct")
			{
				res.emplace_back(Token::Type::KeywordStruct);
			}
			else
			{
				res.emplace_back(Token::Type::Name, word);
			}

			continue;
		}

		// A lot of tokects are single character, this macro helps
		// preventing code repetition
		#define CHAR_TOKEN(c, type) \
		if (in[i] == c) \
		{ \
			res.emplace_back(type); \
			continue; \
		}
		CHAR_TOKEN('<', Token::Type::TemplateOpen);
		CHAR_TOKEN('>', Token::Type::TemplateClose);
		CHAR_TOKEN('(', Token::Type::ParenthesesOpen);
		CHAR_TOKEN(')', Token::Type::ParenthesesClose);
		CHAR_TOKEN('{', Token::Type::BracesOpen);
		CHAR_TOKEN('}', Token::Type::BracesClose);
		CHAR_TOKEN(';', Token::Type::Semicolon);
		CHAR_TOKEN(',', Token::Type::Comma);
		#undef CHAR_TOKEN

		if (in[i] == '[' && in[i+1] == '[')
		{
			++i;
			res.emplace_back(Token::Type::AttributeOpen);
			continue;
		}

		if (in[i] == ']' && in[i+1] == ']')
		{
			++i;
			res.emplace_back(Token::Type::AttributeClose);
			continue;
		}

		if (in[i] == ':' && in[i+1] == ':')
		{
			++i;
			res.emplace_back(Token::Type::NamespaceSeparator);
			continue;
		}

		if (in[i] == '"')
		{
			string str;
			++i;

			// TODO escapes are not handled
			// TODO unclosed strings are not handled
			while (in[i] != '"')
			{
				str.push_back(in[i]);
				++i;
			}

			res.emplace_back(Token::Type::String, str);
			continue;
		}

		cerr << "Unexpected token\n";
		exit(1);
	}

	res.emplace_back(Token::Type::EndOfFile);
	return res;
}
