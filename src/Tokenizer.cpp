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

const char* TokenTypeStr(Token::Type type)
{
	switch (type)
	{
		default:
		case Token::Type::Unknown:            return "Unknown";
		case Token::Type::EndOfFile:          return "EndOfFile";
		case Token::Type::Name:               return "Name";
		case Token::Type::TemplateOpen:       return "TemplateOpen";
		case Token::Type::TemplateClose:      return "TemplateClose";
		case Token::Type::ParenthesesOpen:    return "ParenthesesOpen";
		case Token::Type::ParenthesesClose:   return "ParenthesesClose";
		case Token::Type::BracesOpen:         return "BracesOpen";
		case Token::Type::BracesClose:        return "BracesClose";
		case Token::Type::Semicolon:          return "Semicolon";
		case Token::Type::AttributeOpen:      return "AttributeOpen";
		case Token::Type::AttributeClose:     return "AttributeClose";
		case Token::Type::String:             return "String";
		case Token::Type::Comma:              return "Comma";
		case Token::Type::NamespaceSeparator: return "NamespaceSeparator";
		case Token::Type::KeywordNamespace:   return "KeywordNamespace";
		case Token::Type::KeywordStruct:      return "KeywordStruct";
	}
}

std::ostream& operator<<(std::ostream &out, const Token &t)
{
	out << "[Token " << TokenTypeStr(t.type);
	if (t.strValue.size())
	{
		out << " \"" << t.strValue << "\"";
	}
	out << "]";
	return out;
}

vector<Token> Tokenize(const string &in)
{
	int currentLineNo = 1;
	size_t lineStardIdx = 0;
	size_t i = 0;

	vector<Token> res;

	auto EmitToken = [&](Token::Type type, const std::string &s = "")
	{
		Token token;
		token.type = type;
		token.strValue = s;
		token.line = currentLineNo;
		token.col = 1 + (i - lineStardIdx);
		res.push_back(token);
	};

	for (i = 0; i < in.size(); ++i)
	{
		if (isspace(in[i]))
		{
			if (in[i] == '\n')
			{
				++currentLineNo;
				lineStardIdx = i+1;
			}
			continue;
		}

		// Skip Comments
		if (in[i] == '/' && in[i+1] == '/')
		{
			while (in[i] && in[i] != '\n')
				++i;

			++currentLineNo;
			lineStardIdx = i+1;

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
				if (in[i] == '\n')
				{
					++currentLineNo;
					lineStardIdx = i+1;
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
				EmitToken(Token::Type::KeywordNamespace);
			}
			else if (word == "struct")
			{
				EmitToken(Token::Type::KeywordStruct);
			}
			else
			{
				EmitToken(Token::Type::Name, word);
			}

			continue;
		}

		switch (in[i])
		{
			case '<': EmitToken(Token::Type::TemplateOpen);     continue;
			case '>': EmitToken(Token::Type::TemplateClose);    continue;
			case '(': EmitToken(Token::Type::ParenthesesOpen);  continue;
			case ')': EmitToken(Token::Type::ParenthesesClose); continue;
			case '{': EmitToken(Token::Type::BracesOpen);       continue;
			case '}': EmitToken(Token::Type::BracesClose);      continue;
			case ';': EmitToken(Token::Type::Semicolon);        continue;
			case ',': EmitToken(Token::Type::Comma);            continue;
		}

		if (in[i] == '[' && in[i+1] == '[')
		{
			++i;
			EmitToken(Token::Type::AttributeOpen);
			continue;
		}

		if (in[i] == ']' && in[i+1] == ']')
		{
			++i;
			EmitToken(Token::Type::AttributeClose);
			continue;
		}

		if (in[i] == ':' && in[i+1] == ':')
		{
			++i;
			EmitToken(Token::Type::NamespaceSeparator);
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

			EmitToken(Token::Type::String, str);
			continue;
		}

		cerr << "Unexpected token\n";
		exit(1);
	}

	EmitToken(Token::Type::EndOfFile);
	return res;
}
