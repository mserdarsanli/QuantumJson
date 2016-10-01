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

#ifndef QUANTUMJSON_LIB_IMPL_
#define QUANTUMJSON_LIB_IMPL_

#include <cstdlib>
#include <functional>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

#include <errno.h>
#include <unistd.h>

// TODO add library version checks?

// TODO find a better namespace name
namespace QuantumJsonImpl__ {

template <typename InputIteratorType>
struct Parser
{
	Parser(InputIteratorType begin, InputIteratorType end)
	  : it(begin), end(end)
	{
	}


	void SkipWhitespace()
	{
		while (it != end)
		{
			char c = *it;
			if (c == ' ' || c == '\t' || c == '\n' || c == '\r')
			{
				++it;
			}
			else
			{
				break;
			}
		}
	}

	void SkipHexDigit()
	{
		if (it == end)
		{
			throw std::runtime_error("Unexpected EOF");
		}

		if ((*it >= '0' && *it <= '9') || (*it >= 'a' && *it <= 'f') || (*it >= 'A' && *it <= 'F'))
		{
			++it;
			return;
		}

		throw std::runtime_error(std::string("Unexpected hex char [") + *it + "]");
	}

	void SkipString()
	{
		SkipChar('"');

		while (it != end)
		{
			if (*it == '"')
			{
				++it;
				return;
			}

			if (*it == '\\')
			{
				++it;

				if (it == end)
					throw std::runtime_error("Unexpected EOF");

				// TODO check escape character
				if (*it == '"' || *it == '\\' || *it == '/' || *it == 'b' || *it == 'f'
				    || *it == 'n' || *it == 'r' || *it == 't')
				{
					++it;
					continue;
				}

				if (*it == 'u')
				{
					++it;
					SkipHexDigit();
					SkipHexDigit();
					SkipHexDigit();
					SkipHexDigit();
					continue;
				}

				throw std::runtime_error("Invalid escape");
			}

			++it;
		}

		throw std::runtime_error("Unexpected EOF");
	}

	void SkipNumber()
	{
		// TODO implement correctly
		while (it != end && ((*it >= '0' && *it <='9') || *it == '.' || *it == '-'))
			++it;
	}

	void SkipNull()
	{
		if (it == end || *it != 'n') throw std::runtime_error("Unexpected token while parsing null");
		++it;
		if (it == end || *it != 'u') throw std::runtime_error("Unexpected token while parsing null");
		++it;
		if (it == end || *it != 'l') throw std::runtime_error("Unexpected token while parsing null");
		++it;
		if (it == end || *it != 'l') throw std::runtime_error("Unexpected token while parsing null");
		++it;
	}

	void SkipTrue()
	{
		if (it == end || *it != 't') throw std::runtime_error("Unexpected token while parsing null");
		++it;
		if (it == end || *it != 'r') throw std::runtime_error("Unexpected token while parsing null");
		++it;
		if (it == end || *it != 'u') throw std::runtime_error("Unexpected token while parsing null");
		++it;
		if (it == end || *it != 'e') throw std::runtime_error("Unexpected token while parsing null");
		++it;
	}

	void SkipFalse()
	{
		if (it == end || *it != 'f') throw std::runtime_error("Unexpected token while parsing null");
		++it;
		if (it == end || *it != 'a') throw std::runtime_error("Unexpected token while parsing null");
		++it;
		if (it == end || *it != 'l') throw std::runtime_error("Unexpected token while parsing null");
		++it;
		if (it == end || *it != 's') throw std::runtime_error("Unexpected token while parsing null");
		++it;
		if (it == end || *it != 'e') throw std::runtime_error("Unexpected token while parsing null");
		++it;
	}

	void SkipObject()
	{
		SkipChar('{');
		SkipWhitespace();

		if (it != end && *it != '}')
		{
			SkipString();
			SkipWhitespace();
			SkipChar(':');
			SkipWhitespace();
			SkipValue();
			SkipWhitespace();
		}

		while (it != end && *it != '}')
		{
			SkipChar(',');
			SkipWhitespace();
			SkipString();
			SkipWhitespace();
			SkipChar(':');
			SkipWhitespace();
			SkipValue();
			SkipWhitespace();
		}

		SkipChar('}');
	}

	void SkipList()
	{
		SkipChar('[');
		SkipWhitespace();

		if (it != end && *it != ']')
		{
			SkipValue();
			SkipWhitespace();
		}

		while (it != end && *it != ']')
		{
			SkipChar(',');
			SkipWhitespace();
			SkipValue();
			SkipWhitespace();
		}

		SkipChar(']');
	}

	// TODO prevent recursion by putting a depth limit?
	void SkipValue()
	{
		switch (*it)
		{
			case '"':
				return SkipString();
			case '-':
			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':
				return SkipNumber();
			case 'f':
				return SkipFalse();
			case 'n':
				return SkipNull();
			case 't':
				return SkipTrue();
			case '{':
				return SkipObject();
			case '[':
				return SkipList();

			// TODO implement others
			default:
				throw std::runtime_error( __FILE__ ":" + std::to_string(__LINE__) +
				   " Unexpected token: " + std::string(it, (end < it+20 ? end : it+20)));
		}
	}

	void SkipChar(char c)
	{
		if (it == end)
		{
			throw std::runtime_error("Unexpected EOF");
		}

		if (*it != c)
		{
			std::string errMsg = std::string("Unexpected char [") + *it + "] expected ["
			                   + c + "]";
			errMsg += "\nRemaining:\n" + std::string();
			throw std::runtime_error(errMsg);
		}

		++it;
	}

	void MaybeSkipNull(bool *skipped)
	{
		*skipped = false;

		if (it != end && *it == 'n')
		{
			++it;
			if (it == end || *it != 'u') throw "Unexpected";
			++it;
			if (it == end || *it != 'l') throw "Unexpected";
			++it;
			if (it == end || *it != 'l') throw "Unexpected";
			++it;

			*skipped = true;
		}
	}

	inline
	int fromHex(char c)
	{
		if (c >= '0' && c <= '9') return c - '0';
		if (c >= 'a' && c <= 'f') return c - 'a' + 10;
		if (c >= 'A' && c <= 'F') return c - 'A' + 10;
		throw std::runtime_error("Not a hexadecimal character");
	}

	inline
	void Utf8Append(int codePoint, std::string &str)
	{
		if (codePoint <= 0x7f)
		{
			str.push_back( codePoint );
			return;
		}
		if (codePoint <= 0x7ff)
		{
			str.push_back(0xC0 | (codePoint >> 6));
			str.push_back(0x80 | (codePoint & 0x3f));
			return;
		}
		if (codePoint <= 0xffff)
		{
			str.push_back(0xE0 | (codePoint >> 12));
			str.push_back(0x80 | ((codePoint >> 6) & 0x3f));
			str.push_back(0x80 | (codePoint & 0x3f));
			return;
		}
		if (codePoint <= 0x10ffff)
		{
			str.push_back(0xF0 | (codePoint >> 18));
			str.push_back(0x80 | ((codePoint >> 12) & 0x3f));
			str.push_back(0x80 | ((codePoint >> 6) & 0x3f));
			str.push_back(0x80 | (codePoint & 0x3f));
			return;
		}
		else
		{
			// TODO this check should be moved to surrogate pair parsing place
			throw std::runtime_error("Unsupported unicode range");
		}
	}

	void ParseValueInto(bool &obj)
	{
		if (it == end)
			goto fail;

		if (*it == 't')
		{
			++it;
			if (it == end || *it != 'r') goto fail;
			++it;
			if (it == end || *it != 'u') goto fail;
			++it;
			if (it == end || *it != 'e') goto fail;
			++it;
			obj = true;
			return;
		}
		else if (*it == 'f')
		{
			++it;
			if (it == end || *it != 'a') goto fail;
			++it;
			if (it == end || *it != 'l') goto fail;
			++it;
			if (it == end || *it != 's') goto fail;
			++it;
			if (it == end || *it != 'e') goto fail;
			++it;
			obj = false;
			return;
		}

		fail:
		throw std::runtime_error("boolp " + std::to_string(__LINE__) + " : "
		    "Unexpected token: " + std::string(it, (end < it+20 ? end : it+20)));
	}

	void ParseValueInto(std::string &obj)
	{
		SkipChar('"');

		// Parse string characters
		while (it != end && *it != '"')
		{
			if (*it == '\\')
			{
				++it;

				if (it == end)
				{
					throw std::runtime_error("Unexpected EOF");
				}

				switch (*(it++))
				{
					case '"':  obj.push_back('"');  break;
					case '\\': obj.push_back('\\'); break;
					case '/':  obj.push_back('/');  break;
					case 'b':  obj.push_back('\b'); break;
					case 'f':  obj.push_back('\f'); break;
					case 'n':  obj.push_back('\n'); break;
					case 'r':  obj.push_back('\r'); break;
					case 't':  obj.push_back('\t'); break;
					case 'u':
					{
						int cp = 0;
						// Code-point consists of 4 hexadecimal numbers

						cp |= fromHex(*(it++)) << 12;
						cp |= fromHex(*(it++)) << 8;
						cp |= fromHex(*(it++)) << 4;
						cp |= fromHex(*(it++));

						// Check if value is UTF-16 surrogate pair
						if (cp >= 0xD800 && cp <= 0xDFFF)
						{
							SkipChar('\\');
							SkipChar('u');

							int cp2 = 0;
							// TODO eof check
							cp2 |= fromHex(*(it++)) << 12;
							cp2 |= fromHex(*(it++)) << 8;
							cp2 |= fromHex(*(it++)) << 4;
							cp2 |= fromHex(*(it++));

							// TODO verify range of cp2
							// https://en.wikipedia.org/wiki/UTF-16#U.2BD800_to_U.2BDFFF
							cp = 0x10000 + ((cp & 0x3FF) << 10) + (cp2 & 0x3FF);
						}

						// Encode cp as utf-8
						Utf8Append(cp, obj);

						break;
					}

					// TODO this outputs after it is increased, so error is not visible
					default: throw std::runtime_error("Unknown escape: " + std::string());
				}
			}
			else if ((*it & 0b11000000) == 0b11000000)
			{
	#define SKIP_CONTINUATION_BYTE \
		if (it == end || ((*it & 0b11000000) != 0b10000000)) \
			throw std::runtime_error("Unexpected continuation character"); \
		obj.push_back(*it); \
		++it;
				// Unicode start byte
				if (*it >= 0b11111100)
				{
					obj.push_back(*it);
					++it;
					// 5 bytes should follow
					SKIP_CONTINUATION_BYTE
					SKIP_CONTINUATION_BYTE
					SKIP_CONTINUATION_BYTE
					SKIP_CONTINUATION_BYTE
					SKIP_CONTINUATION_BYTE
				}
				else if (*it >= 0b11111000)
				{
					obj.push_back(*it);
					++it;
					// 4 bytes should follow
					SKIP_CONTINUATION_BYTE
					SKIP_CONTINUATION_BYTE
					SKIP_CONTINUATION_BYTE
					SKIP_CONTINUATION_BYTE
				}
				else if (*it >= 0b11110000)
				{
					obj.push_back(*it);
					++it;
					// 3 bytes should follow
					SKIP_CONTINUATION_BYTE
					SKIP_CONTINUATION_BYTE
					SKIP_CONTINUATION_BYTE
				}
				else if (*it >= 0b11100000)
				{
					obj.push_back(*it);
					++it;
					// 2 bytes should follow
					SKIP_CONTINUATION_BYTE
					SKIP_CONTINUATION_BYTE
				}
				else // if (*it >= 0b11000000)
				{
					obj.push_back(*it);
					++it;
					// 1 byte should follow
					SKIP_CONTINUATION_BYTE
				}
	#undef SKIP_CONTINUATION_BYTE
			}
			else if ((*it & 0b11000000) == 0b10000000)
			{
				// Unexpected continuation character
				throw std::runtime_error("Unexpected continuation character");
			}
			else
			{
				obj.push_back(*it);
				++it;
			}
		}

		SkipChar('"');
	}

	// TODO FIXME it == end checks are not done here
	void ParseValueInto(double &obj)
	{
		if (*it != '-' && (*it < '0' || *it > '9'))
		{
			throw std::runtime_error("doublep " + std::to_string(__LINE__) + " : "
			                                + std::string(it, (end < it + 20 ? end : it+20)));
		}

		InputIteratorType begin = it;

		// skip_negative_sign:
		if (*it == '-') ++it;

		// skip_digits:
		if (*it == '0')
		{
			++it;
			goto skip_decimal_fractions;
		}
		else if (*it > '0' && *it <= '9')
		{
			++it;
			while (*it >= '0' && *it <= '9')
			{
				++it;
			}
		}
		else
		{
			throw std::runtime_error("doublep " + std::to_string(__LINE__) + " : "
			                                + std::string(it, (end < it + 20 ? end : it+20)));
		}

		skip_decimal_fractions:
		if (*it == '.')
		{
			++it;

			if (*it < '0' || *it > '9')
			{
				// There must be a number after separator
				throw std::runtime_error("doublep " + std::to_string(__LINE__) + " : "
				                                + std::string(it, (end < it + 20 ? end : it+20)));
			}
			++it;

			while (*it >= '0' && *it <= '9')
			{
				++it;
			}
		}

		// skip_exponent:
		if (*it == 'e' || *it == 'E')
		{
			++it;

			if (*it == '-' || *it == '+')
				++it;

			while (*it >= '0' && *it <= '9')
				++it;
		}

		// TODO FIXME strtod only works with c-strings?
		std::string doubleStr(begin, it);
		obj = strtod(doubleStr.c_str(), nullptr);

		// No error checks done here, as assuming the format should be good for strtod.
		// Clear possible ERANGE error from strtod.
		errno = 0;
	}

	// TODO add flag to accept double values like (1234.0) as integers?
	void ParseValueInto(int &obj)
	{
		std::string num;
		// TODO parse this correctly
		while (it != end)
		{
			if ((*it >= '0' && *it <= '9') || *it == '-' || *it == '+')
			{
				num.push_back(*it);
				++it;
				continue;
			}
			break;
		}


		if (num.size() == 0)
			throw std::runtime_error("intp " + std::to_string(__LINE__) + " : "
			                                + std::string(it, (end < it + 20 ? end : it+20)));

		obj = stoi(num);
	}

	template <typename ArrayElemType>
	void ParseValueInto(std::vector<ArrayElemType> &obj)
	{
		obj.clear();
		SkipChar('[');
		SkipWhitespace();

		while (it != end)
		{
			if (*it == ']')
			{
				++it;
				return;
			}

			if (obj.size() > 0)
			{
				SkipChar(',');
				SkipWhitespace();
			}

			ArrayElemType elem;
			ParseValueInto(elem);
			obj.push_back(elem); // TODO use move here? that wil also require creating move constructors.

			SkipWhitespace();
		}

		throw std::runtime_error("Whoa dudeee: " + std::string(it , (end < it+20 ? end : it+20)));
	}


	template <typename ObjectType>
	void ParseObject(ObjectType &obj)
	{
		SkipWhitespace();

		SkipChar('{');
		SkipWhitespace();

		if (it != end && *it != '}')
		{
			obj.ParseNextField(*this);
			SkipWhitespace();
		}

		while (it != end && *it != '}')
		{
			SkipChar(',');
			SkipWhitespace();

			obj.ParseNextField(*this);
			SkipWhitespace();
		}

		SkipChar('}');
	}

	template <typename ObjectType>
	void ParseValueInto(ObjectType &obj)
	{
		ParseObject(obj);
	}


	InputIteratorType it;
	InputIteratorType end;
};



}  // namespace JsonDef

#endif  // QUANTUMJSON_LIB_IMPL_
