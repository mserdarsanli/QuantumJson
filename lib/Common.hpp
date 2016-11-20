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

#include <cinttypes>
#include <cmath>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <iterator>
#include <map>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <vector>

#include <errno.h>
#include <unistd.h>

#if defined(__GNUC__) || defined(__clang__)
#define QUANTUMJSON_LIKELY(x) __builtin_expect(!!(x), 1)
#define QUANTUMJSON_UNLIKELY(x) __builtin_expect(!!(x), 0)
#else
#define QUANTUMJSON_LIKELY(x) (x)
#define QUANTUMJSON_UNLIKELY(x) (x)
#endif

// Exceptions seem to have a large (a few percent) performance cost by
// preventing inlining (though I am not really sure about why).
// So this simple alternative is used.
#define QUANTUMJSON_CHECK_ERROR_AND_PROPAGATE \
	if (QUANTUMJSON_UNLIKELY(this->errorCode != ErrorCode::NoError)) \
	{ \
		return; \
	}

#define QUANTUMJSON_CHECK_EOF_AND_PROPAGATE \
	if (QUANTUMJSON_UNLIKELY(this->it == this->end)) \
	{ \
		this->errorCode = ErrorCode::UnexpectedEOF; \
		return; \
	}

// TODO add library version checks?

// TODO find a better namespace name
namespace QuantumJsonImpl__ {

enum class ErrorCode
{
	NoError,

	UnexpectedEOF,
	UnexpectedChar,
	UnexpectedToken,
	InvalidEscape,
	InvalidUtf8Sequence,
	UnsupportedUnicodeRange,
	ControlCharacterInString,
};

// Make an exception class for better error reporting when uncaught
class JsonError : public std::exception
{
public:
	explicit JsonError(ErrorCode code)
	    : errorCode(code)
	{
	}

	// TODO provide more error details?
	const char* what() const noexcept override
	{
		switch (errorCode)
		{
		case ErrorCode::UnexpectedEOF:
			return "Unexpected EOF";
		case ErrorCode::UnexpectedChar:
			return "Unexpected Char";
		case ErrorCode::UnexpectedToken:
			return "Unexpected Token";
		case ErrorCode::InvalidEscape:
			return "Invalid Escape";
		case ErrorCode::InvalidUtf8Sequence:
			return "Invalid UTF-8 Sequence";
		case ErrorCode::UnsupportedUnicodeRange:
			return "Unsupported Unicode Range";
		case ErrorCode::ControlCharacterInString:
			return "Control Character In String";
		default:
			// Should not happen
			return "";
		}
	}

private:
	ErrorCode errorCode;
};

template <typename InputIteratorType>
struct InputProcessor
{
	InputProcessor(InputIteratorType begin, InputIteratorType end)
	  : it(begin), end(end)
	{
	}

	void SkipWhitespace()
	{
		while (it != end)
		{
			char c = *it;
			// Most JSON data are compact, so whitespace is marked as
			// unlikely
			if (QUANTUMJSON_UNLIKELY(c == ' ' || c == '\t' || c == '\n' || c == '\r'))
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
		QUANTUMJSON_CHECK_EOF_AND_PROPAGATE;

		if (QUANTUMJSON_LIKELY(
		       (*it >= '0' && *it <= '9')
		    || (*it >= 'a' && *it <= 'f')
		    || (*it >= 'A' && *it <= 'F')))
		{
			++it;
			return;
		}

		errorCode = ErrorCode::UnexpectedChar;
	}

	void SkipString()
	{
		SkipChar('"'); QUANTUMJSON_CHECK_ERROR_AND_PROPAGATE;

		while (1)
		{
			QUANTUMJSON_CHECK_EOF_AND_PROPAGATE;

			if (QUANTUMJSON_UNLIKELY((*it & 0b11100000) == 0))
			{
				errorCode = ErrorCode::ControlCharacterInString;
				return;
			}

			if (QUANTUMJSON_UNLIKELY(*it == '"'))
			{
				++it;
				return;
			}

			if (QUANTUMJSON_UNLIKELY(*it == '\\'))
			{
				++it;

				QUANTUMJSON_CHECK_EOF_AND_PROPAGATE;

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
					SkipHexDigit(); QUANTUMJSON_CHECK_ERROR_AND_PROPAGATE;
					SkipHexDigit(); QUANTUMJSON_CHECK_ERROR_AND_PROPAGATE;
					SkipHexDigit(); QUANTUMJSON_CHECK_ERROR_AND_PROPAGATE;
					SkipHexDigit(); QUANTUMJSON_CHECK_ERROR_AND_PROPAGATE;
					continue;
				}

				errorCode = ErrorCode::InvalidEscape;
				return;
			}

			++it;
		}
	}

	void SkipNumber()
	{
		// TODO implement correctly
		while (1)
		{
			QUANTUMJSON_CHECK_EOF_AND_PROPAGATE;
			if (QUANTUMJSON_LIKELY(*it >= '0' && *it <='9')
			    || *it == '.' || *it == '-')
			{
				++it;
			}
			else
			{
				break;
			}
		}
	}

	void SkipNull()
	{
		SkipChar('n'); QUANTUMJSON_CHECK_ERROR_AND_PROPAGATE;
		SkipChar('u'); QUANTUMJSON_CHECK_ERROR_AND_PROPAGATE;
		SkipChar('l'); QUANTUMJSON_CHECK_ERROR_AND_PROPAGATE;
		SkipChar('l'); QUANTUMJSON_CHECK_ERROR_AND_PROPAGATE;
	}

	void SkipTrue()
	{
		SkipChar('t'); QUANTUMJSON_CHECK_ERROR_AND_PROPAGATE;
		SkipChar('r'); QUANTUMJSON_CHECK_ERROR_AND_PROPAGATE;
		SkipChar('u'); QUANTUMJSON_CHECK_ERROR_AND_PROPAGATE;
		SkipChar('e'); QUANTUMJSON_CHECK_ERROR_AND_PROPAGATE;
	}

	void SkipFalse()
	{
		SkipChar('f'); QUANTUMJSON_CHECK_ERROR_AND_PROPAGATE;
		SkipChar('a'); QUANTUMJSON_CHECK_ERROR_AND_PROPAGATE;
		SkipChar('l'); QUANTUMJSON_CHECK_ERROR_AND_PROPAGATE;
		SkipChar('s'); QUANTUMJSON_CHECK_ERROR_AND_PROPAGATE;
		SkipChar('e'); QUANTUMJSON_CHECK_ERROR_AND_PROPAGATE;
	}

	void SkipObject()
	{
		SkipChar('{'); QUANTUMJSON_CHECK_ERROR_AND_PROPAGATE;
		SkipWhitespace();
		QUANTUMJSON_CHECK_EOF_AND_PROPAGATE;
		bool isFirstField = true;

		while (1)
		{
			QUANTUMJSON_CHECK_EOF_AND_PROPAGATE;
			if (QUANTUMJSON_UNLIKELY(*it == '}'))
			{
				SkipChar('}'); QUANTUMJSON_CHECK_ERROR_AND_PROPAGATE;
				return;
			}

			if (isFirstField)
			{
				isFirstField = false;
			}
			else
			{
				SkipChar(','); QUANTUMJSON_CHECK_ERROR_AND_PROPAGATE;
				SkipWhitespace();
			}

			SkipString(); QUANTUMJSON_CHECK_ERROR_AND_PROPAGATE;
			SkipWhitespace();
			SkipChar(':'); QUANTUMJSON_CHECK_ERROR_AND_PROPAGATE;
			SkipWhitespace();
			SkipValue(); QUANTUMJSON_CHECK_ERROR_AND_PROPAGATE;
			SkipWhitespace();
		}
	}

	void SkipList()
	{
		SkipChar('['); QUANTUMJSON_CHECK_ERROR_AND_PROPAGATE;
		SkipWhitespace();
		QUANTUMJSON_CHECK_EOF_AND_PROPAGATE;
		bool isFirstField = true;

		while (1)
		{
			QUANTUMJSON_CHECK_EOF_AND_PROPAGATE;
			if (QUANTUMJSON_UNLIKELY(*it == ']'))
			{
				SkipChar(']'); QUANTUMJSON_CHECK_ERROR_AND_PROPAGATE;
				return;
			}

			if (isFirstField)
			{
				isFirstField = false;
			}
			else
			{
				SkipChar(','); QUANTUMJSON_CHECK_ERROR_AND_PROPAGATE;
				SkipWhitespace();
			}

			SkipValue(); QUANTUMJSON_CHECK_ERROR_AND_PROPAGATE;
			SkipWhitespace();
		}

		SkipChar(']'); QUANTUMJSON_CHECK_ERROR_AND_PROPAGATE;
	}

	// TODO prevent recursion by putting a depth limit?
	void SkipValue()
	{
		switch (*it)
		{
			case '"':
				SkipString();
				return;
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
				SkipNumber();
				return;
			case 'f':
				SkipFalse();
				return;
			case 'n':
				SkipNull();
				return;
			case 't':
				SkipTrue();
				return;
			case '{':
				SkipObject();
				return;
			case '[':
				SkipList();
				return;

			// TODO implement others
			default:
				errorCode = ErrorCode::UnexpectedToken;
				return;
		}
	}

	void SkipChar(char c)
	{
		QUANTUMJSON_CHECK_EOF_AND_PROPAGATE;

		if (QUANTUMJSON_UNLIKELY(*it != c))
		{
			errorCode = ErrorCode::UnexpectedChar;
			return;
		}

		++it;
	}

	void MaybeSkipNull(bool *skipped)
	{
		*skipped = false;

		QUANTUMJSON_CHECK_EOF_AND_PROPAGATE;
		if (QUANTUMJSON_UNLIKELY(*it == 'n'))
		{
			SkipNull(); QUANTUMJSON_CHECK_ERROR_AND_PROPAGATE;
			QUANTUMJSON_CHECK_ERROR_AND_PROPAGATE;
			*skipped = true;
		}
	}


	ErrorCode errorCode = ErrorCode::NoError;
	InputIteratorType it;
	InputIteratorType end;
};


template <typename InputIteratorType>
struct Parser : InputProcessor<InputIteratorType>
{
	Parser(InputIteratorType begin, InputIteratorType end)
	  : InputProcessor<InputIteratorType>(begin, end)
	{
	}

	inline
	void Utf8Append(int codePoint, std::string &str)
	{
		if (QUANTUMJSON_LIKELY(codePoint <= 0x7f))
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
			this->errorCode = ErrorCode::UnsupportedUnicodeRange;
			return;
		}
	}

	void ParseValueInto(bool &obj)
	{
		QUANTUMJSON_CHECK_EOF_AND_PROPAGATE;

		if (QUANTUMJSON_LIKELY(*(this->it) == 't'))
		{
			this->SkipTrue(); QUANTUMJSON_CHECK_ERROR_AND_PROPAGATE;
			obj = true;
			return;
		}
		else if (QUANTUMJSON_LIKELY(*(this->it) == 'f'))
		{
			this->SkipFalse(); QUANTUMJSON_CHECK_ERROR_AND_PROPAGATE;
			obj = false;
			return;
		}

		this->errorCode = ErrorCode::UnexpectedToken;
	}

	inline
	int fromHex(char c)
	{
		if (QUANTUMJSON_LIKELY(c >= '0' && c <= '9')) return c - '0';
		if (QUANTUMJSON_LIKELY(c >= 'a' && c <= 'f')) return c - 'a' + 10;
		if (QUANTUMJSON_LIKELY(c >= 'A' && c <= 'F')) return c - 'A' + 10;
		this->errorCode = ErrorCode::UnexpectedChar;
		return -1;
	}

	void ParseValueIntoStringImpl(std::string &obj)
	{
		this->SkipChar('"'); QUANTUMJSON_CHECK_ERROR_AND_PROPAGATE;

		// Parse string characters
		while (QUANTUMJSON_LIKELY(this->it != this->end && *(this->it) != '"'))
		{
			if ((*(this->it) & 0b11100000) == 0)
			{
				this->errorCode = ErrorCode::ControlCharacterInString;
				return;
			}

			if (*(this->it) == '\\')
			{
				++this->it;

				QUANTUMJSON_CHECK_EOF_AND_PROPAGATE;

				switch (*(this->it++))
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

						// TODO EOF checks neede here
						cp |= fromHex(*(this->it++)) << 12;
						cp |= fromHex(*(this->it++)) << 8;
						cp |= fromHex(*(this->it++)) << 4;
						cp |= fromHex(*(this->it++));

						// Check if value is UTF-16 surrogate pair
						if (cp >= 0xD800 && cp <= 0xDFFF)
						{
							this->SkipChar('\\'); QUANTUMJSON_CHECK_ERROR_AND_PROPAGATE;
							this->SkipChar('u'); QUANTUMJSON_CHECK_ERROR_AND_PROPAGATE;

							int cp2 = 0;
							// TODO eof check
							cp2 |= fromHex(*(this->it++)) << 12;
							cp2 |= fromHex(*(this->it++)) << 8;
							cp2 |= fromHex(*(this->it++)) << 4;
							cp2 |= fromHex(*(this->it++));

							// TODO verify range of cp2
							// https://en.wikipedia.org/wiki/UTF-16#U.2BD800_to_U.2BDFFF
							cp = 0x10000 + ((cp & 0x3FF) << 10) + (cp2 & 0x3FF);
						}

						// Encode cp as utf-8
						Utf8Append(cp, obj);

						break;
					}

					default:
						this->errorCode = ErrorCode::InvalidEscape;
						return;
				}
			}
			else if ((*(this->it) & 0b11000000) == 0b11000000)
			{
	#define SKIP_CONTINUATION_BYTE \
		if (this->it == this->end || ((*(this->it) & 0b11000000) != 0b10000000)) \
		{ \
			this->errorCode = ErrorCode::InvalidUtf8Sequence; \
			return; \
		} \
		obj.push_back(*(this->it)); \
		++this->it;
				// Unicode start byte
				if ((*(this->it) & 0b11111100) == 0b11111100)
				{
					obj.push_back(*(this->it));
					++this->it;
					// 5 bytes should follow
					SKIP_CONTINUATION_BYTE
					SKIP_CONTINUATION_BYTE
					SKIP_CONTINUATION_BYTE
					SKIP_CONTINUATION_BYTE
					SKIP_CONTINUATION_BYTE
				}
				else if ((*(this->it) & 0b11111000) == 0b11111000)
				{
					obj.push_back(*(this->it));
					++this->it;
					// 4 bytes should follow
					SKIP_CONTINUATION_BYTE
					SKIP_CONTINUATION_BYTE
					SKIP_CONTINUATION_BYTE
					SKIP_CONTINUATION_BYTE
				}
				else if ((*(this->it) & 0b11110000) == 0b11110000)
				{
					obj.push_back(*(this->it));
					++this->it;
					// 3 bytes should follow
					SKIP_CONTINUATION_BYTE
					SKIP_CONTINUATION_BYTE
					SKIP_CONTINUATION_BYTE
				}
				else if ((*(this->it) & 0b11100000) == 0b11100000)
				{
					obj.push_back(*(this->it));
					++this->it;
					// 2 bytes should follow
					SKIP_CONTINUATION_BYTE
					SKIP_CONTINUATION_BYTE
				}
				else // if ((*(this->it) & 0b11000000) == 0b11000000)
				{
					obj.push_back(*(this->it));
					++this->it;
					// 1 byte should follow
					SKIP_CONTINUATION_BYTE
				}
	#undef SKIP_CONTINUATION_BYTE
			}
			else if ((*(this->it) & 0b11000000) == 0b10000000)
			{
				// Unexpected continuation character
				this->errorCode = ErrorCode::InvalidUtf8Sequence;
				return;
			}
			else
			{
				obj.push_back(*(this->it));
				++this->it;
			}
		}

		this->SkipChar('"'); QUANTUMJSON_CHECK_ERROR_AND_PROPAGATE;
	}

	// Following two functions use SFINAE to provide two different string
	// parsers, allowing random access iterators one to measure and reserve
	// size before, to prevent unnecessary allocations.
	// InputIterator alternative is using default std::string growth.

	// Faster parser that works for random access iterator
	template <typename IteratorType>
	void ParseValueIntoHelper(std::string &obj,
	    typename std::enable_if<
	        std::is_base_of<
	            std::random_access_iterator_tag,
	            typename std::iterator_traits<IteratorType>::iterator_category
	        >::value
	    >::type * = 0)
	{
		obj.clear();

		// Reserve just enough space
		auto begin = this->it;
		this->SkipString(); QUANTUMJSON_CHECK_ERROR_AND_PROPAGATE;
		// TODO FIXME this size logic does not account for escapes in the string
		size_t strSize = this->it - begin - 2;
		this->it = begin;
		obj.reserve( strSize );

		ParseValueIntoStringImpl(obj); QUANTUMJSON_CHECK_ERROR_AND_PROPAGATE;
	}

	// Slower parser that works for any iterator
	template <typename IteratorType>
	void ParseValueIntoHelper(std::string &obj,
	    typename std::enable_if<
	        ! std::is_base_of<
	            std::random_access_iterator_tag,
	            typename std::iterator_traits<IteratorType>::iterator_category
	        >::value
	    >::type * = 0)
	{
		obj.clear();
		ParseValueIntoStringImpl(obj); QUANTUMJSON_CHECK_ERROR_AND_PROPAGATE;
	}

	void ParseValueInto(std::string &obj)
	{
		ParseValueIntoHelper< InputIteratorType >(obj); QUANTUMJSON_CHECK_ERROR_AND_PROPAGATE;
	}

	// TODO FIXME it == end checks are not done here
	void ParseValueInto(double &obj)
	{
		if (QUANTUMJSON_UNLIKELY(*(this->it) != '-' && (*(this->it) < '0' || *(this->it) > '9')))
		{
			this->errorCode = ErrorCode::UnexpectedToken;
			return;
		}

		InputIteratorType begin = this->it;

		// skip_negative_sign:
		if (*(this->it) == '-') ++this->it;

		// skip_digits:
		if (*(this->it) == '0')
		{
			++this->it;
			goto skip_decimal_fractions;
		}
		else if (*(this->it) > '0' && *(this->it) <= '9')
		{
			++this->it;
			while (*(this->it) >= '0' && *(this->it) <= '9')
			{
				++this->it;
			}
		}
		else
		{
			this->errorCode = ErrorCode::UnexpectedToken;
			return;
		}

		skip_decimal_fractions:
		if (*(this->it) == '.')
		{
			++this->it;

			if (*(this->it) < '0' || *(this->it) > '9')
			{
				// There must be a number after separator
				this->errorCode = ErrorCode::UnexpectedToken;
				return;
			}
			++this->it;

			while (*(this->it) >= '0' && *(this->it) <= '9')
			{
				++this->it;
			}
		}

		// skip_exponent:
		if (*(this->it) == 'e' || *(this->it) == 'E')
		{
			++this->it;

			if (*(this->it) == '-' || *(this->it) == '+')
				++this->it;

			while (*(this->it) >= '0' && *(this->it) <= '9')
				++this->it;
		}

		// TODO FIXME strtod only works with c-strings?
		std::string doubleStr(begin, this->it);
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
		while (this->it != this->end)
		{
			if ((*(this->it) >= '0' && *(this->it) <= '9') || *(this->it) == '-' || *(this->it) == '+')
			{
				num.push_back(*(this->it));
				++this->it;
				continue;
			}
			break;
		}


		if (num.size() == 0)
		{
			this->errorCode = ErrorCode::UnexpectedToken;
			return;
		}

		obj = stoi(num);
	}

	template <typename ArrayElemType>
	void ParseValueInto(std::vector<ArrayElemType> &obj)
	{
		obj.clear();
		this->SkipChar('['); QUANTUMJSON_CHECK_ERROR_AND_PROPAGATE;
		this->SkipWhitespace();

		while (1)
		{
			QUANTUMJSON_CHECK_EOF_AND_PROPAGATE;

			if (QUANTUMJSON_UNLIKELY(*(this->it) == ']'))
			{
				++this->it;
				return;
			}

			if (obj.size() > 0)
			{
				this->SkipChar(','); QUANTUMJSON_CHECK_ERROR_AND_PROPAGATE;
				this->SkipWhitespace();
			}

			ArrayElemType elem;
			ParseValueInto(elem); QUANTUMJSON_CHECK_ERROR_AND_PROPAGATE;
			obj.push_back( std::move(elem) );

			this->SkipWhitespace();
		}
	}

	template <typename MapElemType>
	void ParseValueInto(std::map<std::string, MapElemType> &obj)
	{
		obj.clear();
		this->SkipChar('{'); QUANTUMJSON_CHECK_ERROR_AND_PROPAGATE;
		this->SkipWhitespace();

		while (1)
		{
			QUANTUMJSON_CHECK_EOF_AND_PROPAGATE;

			if (QUANTUMJSON_UNLIKELY(*(this->it) == '}'))
			{
				++this->it;
				return;
			}

			if (obj.size() > 0)
			{
				this->SkipChar(','); QUANTUMJSON_CHECK_ERROR_AND_PROPAGATE;
				this->SkipWhitespace();
			}

			std::string key;
			ParseValueInto(key); QUANTUMJSON_CHECK_ERROR_AND_PROPAGATE;
			this->SkipWhitespace();

			this->SkipChar(':'); QUANTUMJSON_CHECK_ERROR_AND_PROPAGATE;
			this->SkipWhitespace();

			MapElemType value;
			ParseValueInto(value); QUANTUMJSON_CHECK_ERROR_AND_PROPAGATE;
			this->SkipWhitespace();

			obj.insert( std::pair<std::string,MapElemType>(std::move(key), std::move(value)) );
		}

		this->errorCode = ErrorCode::UnexpectedEOF;
	}

	template <typename ObjectType>
	void ParseObject(ObjectType &obj)
	{
		this->SkipWhitespace();

		this->SkipChar('{'); QUANTUMJSON_CHECK_ERROR_AND_PROPAGATE;
		this->SkipWhitespace();

		if (this->it != this->end && *(this->it) != '}')
		{
			obj.ParseNextField(*this);
			this->SkipWhitespace();
		}

		while (this->it != this->end && *(this->it) != '}')
		{
			this->SkipChar(','); QUANTUMJSON_CHECK_ERROR_AND_PROPAGATE;
			this->SkipWhitespace();

			obj.ParseNextField(*this);
			this->SkipWhitespace();
		}

		this->SkipChar('}'); QUANTUMJSON_CHECK_ERROR_AND_PROPAGATE;
	}

	template <typename ObjectType>
	void ParseValueInto(ObjectType &obj)
	{
		ParseObject(obj);
	}
};

template <typename OutputIteratorType>
struct Serializer
{
	Serializer(OutputIteratorType out)
	  : out(out)
	{
	}

	void SerializeValue(bool b)
	{
		if (b)
		{
			*(out++) = 't';
			*(out++) = 'r';
			*(out++) = 'u';
			*(out++) = 'e';
		}
		else
		{
			*(out++) = 'f';
			*(out++) = 'a';
			*(out++) = 'l';
			*(out++) = 's';
			*(out++) = 'e';
		}
	}

	void SerializeValue(int64_t a)
	{
		// Enought to hold -9223372036854775807 (int64 min)
		char buf[21];
		sprintf(buf, "%" PRId64, a);

		for (const char *it = buf; *it; ++it)
		{
			*(out++) = *it;
		}
	}

	void SerializeValue(double num)
	{
		if (std::isnan(num) || std::isinf(num))
		{
			*(out++) = 'n';
			*(out++) = 'u';
			*(out++) = 'l';
			*(out++) = 'l';
			return;
		}
		// 10 decimal significant digits are used
		char buf[30];
		sprintf(buf, "%.10g", num);

		for (const char *it = buf; *it; ++it)
		{
			*(out++) = *it;
		}
	}

	// Input should be a valit UTF-8 string
	void SerializeValue(const std::string &s)
	{
		*(out++) = '"';

		for (char c : s)
		{
			switch (c)
			{
			case 0:
				*(out++) = '\\';
				*(out++) = 'u';
				*(out++) = '0';
				*(out++) = '0';
				*(out++) = '0';
				*(out++) = '0';
				break;
			case '"':
				*(out++) = '\\';
				*(out++) = '"';
				break;
			case '\\':
				*(out++) = '\\';
				*(out++) = '\\';
				break;
			case '\b':
				*(out++) = '\\';
				*(out++) = 'b';
				break;
			case '\f':
				*(out++) = '\\';
				*(out++) = 'f';
				break;
			case '\n':
				*(out++) = '\\';
				*(out++) = 'n';
				break;
			case '\r':
				*(out++) = '\\';
				*(out++) = 'r';
				break;
			case '\t':
				*(out++) = '\\';
				*(out++) = 't';
				break;
			default:
				*(out++) = c;
				break;
			}
		}

		*(out++) = '"';
	}

	template <typename ObjectType>
	void SerializeValue(const ObjectType &obj)
	{
		obj.SerializeTo(*this);
	}

	template <typename ArrayElemType>
	void SerializeValue(const std::vector<ArrayElemType> &obj)
	{
		*(out++) = '[';

		for (size_t i = 0; i < obj.size(); ++i)
		{
			if (i > 0)
			{
				*(out++) = ',';
			}

			this->SerializeValue(obj[i]);
		}

		*(out++) = ']';
	}

	template <typename MapElemType>
	void SerializeValue(const std::map<std::string, MapElemType> &obj)
	{
		*(out++) = '{';

		bool firstEntry = true;
		for (auto it : obj)
		{
			if (!firstEntry)
			{
				*(out++) = ',';
			}
			firstEntry = false;

			this->SerializeValue(it.first);
			*(out++) = ':';
			this->SerializeValue(it.second);
		}

		*(out++) = '}';
	}

	OutputIteratorType out;
};


}  // namespace JsonDef

#undef QUANTUMJSON_CHECK_ERROR_AND_PROPAGATE
#undef QUANTUMJSON_CHECK_EOF_AND_PROPAGATE

#endif  // QUANTUMJSON_LIB_IMPL_
