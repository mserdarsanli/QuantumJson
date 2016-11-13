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

#include <algorithm>
#include <stack>
#include <utility>

#include <boost/format.hpp>

#include "Attributes.hpp"
#include "CodeGenerator.hpp"
#include "JsonParserLibrary.hpp"

using namespace std;
using boost::format;

struct Variable
{
	string cppName;
	string jsonName;
	bool skipNull = false;

	explicit Variable(const VariableDef &var)
	  : cppName(var.name)
	  , jsonName("\"" + var.name + "\"")
	{
		if (var.attributes.find(VarAttributes::JsonFieldName.name) != var.attributes.end())
		{
			this->jsonName = "\"" + var.attributes.at(VarAttributes::JsonFieldName.name).args[0] + "\"";
		}

		if (var.attributes.find(VarAttributes::OnNull.name) != var.attributes.end() &&
		    var.attributes.at(VarAttributes::OnNull.name).args[0] == "skip")
		{
			this->skipNull = true;
		}
	}

	bool operator<(const Variable &o) const
	{
		return jsonName < o.jsonName;
	}
	bool operator==(const Variable &o) const
	{
		return jsonName == o.jsonName;
	}
};

static
void RenderVariableType(ostream &out, const VariableTypeDef &vt)
{
	out << vt.typeName;

	if (vt.of.size())
	{
		out << "< ";

		for (size_t i = 0; i < vt.of.size(); ++i)
		{
			if (i != 0)
			{
				out << " , ";
			}
			RenderVariableType(out, vt.of[i]);
		}

		out << " >";
	}
}

char SerializeToJsonDeclarations[] = R"(
	template <typename OutputIteratorType>
	void SerializeTo(OutputIteratorType out) const
	{
		QuantumJsonImpl__::Serializer<OutputIteratorType> s(out);
		this->SerializeTo(s);
	}

	template <typename OutputIteratorType>
	void SerializeTo(QuantumJsonImpl__::Serializer<OutputIteratorType> &s) const;
)";

format SerializeToJsonDefinitionBegin(R"(
template <typename OutputIteratorType>
void %1%::SerializeTo(QuantumJsonImpl__::Serializer<OutputIteratorType> &s) const
{
	*(s.out++) = '{';
)");

char SerializeToJsonDefinitionEnd[] = R"(
	*(s.out++) = '}';
}
)";

char MergeFromJsonDeclarations[] = R"(
	void MergeFromJson(const std::string &json)
	{
		MergeFromJson(json.begin(), json.end());
	}

	template <typename InputIteratorType>
	void MergeFromJson(InputIteratorType it, InputIteratorType end);
)";

// For aligning bind struct name to %1%, and bind spaces of the same length to %2%
format MergeFromJsonDefImpl(R"(
template <typename InputIteratorType>
inline
void %1%::MergeFromJson(InputIteratorType it,
     %2%                InputIteratorType end)
{
	QuantumJsonImpl__::Parser<InputIteratorType> parser(it, end);
	parser.ParseObject(*this);

	// Throw when parsing fails
	if (parser.errorCode != QuantumJsonImpl__::ErrorCode::NoError)
	{
		throw QuantumJsonImpl__::JsonError(parser.errorCode);
	}
}
)");

format StructDefBegin(R"(
struct %1%
{
	%1%() = default;
	%1%(const %1% &) = default;
	%1%(%1% &&) = default;

	%1%& operator=(const %1% &) = default;

)");

// Methods that are called by parser
char ParserExtensionMethodDeclarations[] = R"(
	// Function that parses one field only
	// If the object is as follows:
	// {"a": "sadsadsa", "b": 123}
	//  <------------->  <------>
	// marked regions would map to ParseNextField calls.
	template <typename InputIteratorType>
	void ParseNextField(QuantumJsonImpl__::Parser<InputIteratorType> &parser);

)";

format ParseNextFieldBegin(R"(
template <typename InputIteratorType>
inline
void %1%::ParseNextField(QuantumJsonImpl__::Parser<InputIteratorType> &parser)
{
)");

format ParseNextFieldEnd(R"(
	// Should be unreachable
}
)");

char IncludeStandardHeaders[] = R"(
#include <functional>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

)";

char ParserCommonStuff[] = R"(
	auto &it = parser.it;
	auto &end = parser.end;

	goto _Start; // _Start is the entry point for perser

	_UnknownField:
	// Field name is not known
	// Skip the field completely.
	// TODO make this an exception if flag is given to
	while (it != end && *it != '"')
		++it;
	if (it == end)
	{
		parser.errorCode = QuantumJsonImpl__::ErrorCode::UnexpectedEOF;
		return;
	}

	// Closing quote
	++it;

	parser.SkipWhitespace();
	parser.SkipChar(':'); // Field Separator
	parser.SkipWhitespace();

	parser.SkipValue();

	return; // Done parsing this field
)";

format FieldNameParsed(R"(
	// Matched field [%1%]
	parser.SkipWhitespace();
	parser.SkipChar(':'); // Field Separator
	parser.SkipWhitespace();

%2%
	// Parse the actual value
	parser.ParseValueInto(this->%1%);
	return;
)");

char SkipNullValue[] = R"(
	// Field skip null values for this field.
	{
		bool skipped = false;
		parser.MaybeSkipNull(&skipped);
		if (skipped)
		{
			return;
		}
	}
)";

char PeekNextChar[] = R"(*it)";
char ConsumeNextChar[] = R"(++it)";

// Base gooto labels for generated parser state machine
struct Labels
{
	static constexpr const char *Start = "_Start";
	static constexpr const char *UnknownField = "_UnknownField";
};

void GenerateParserForStructDef(ostream &out, const StructDef &s);

void GenerateHeaderForFile(ostream &out, const ParsedFile &file)
{
	// TODO use include guards
	out << "#pragma once\n\n";

	out << IncludeStandardHeaders;

	GenerateCommonParserDefinitions(out);

	// Header declerations
	for (const StructDef &s : file.structs)
	{
		out << StructDefBegin % s.name;

		for (const VariableDef &var : s.variables)
		{
			out << "\t";
			RenderVariableType(out, var.type);
			out << " " << var.name << ";\n";
		}

		out << ParserExtensionMethodDeclarations;
		out << MergeFromJsonDeclarations;
		out << SerializeToJsonDeclarations;
		out << "};\n";
	}

	// Function definitions
	for (const StructDef &s : file.structs)
	{
		GenerateParserForStructDef(out, s);

		out << MergeFromJsonDefImpl % s.name % string(s.name.size(), ' ');

		out << SerializeToJsonDefinitionBegin % s.name;

		bool putSeparator = false;
		for (const VariableDef &var : s.variables)
		{
			Variable v(var);

			out << "\n";
			out << "\t// Render field " << v.cppName << "\n";

			if (putSeparator)
			{
				out << "\t*(s.out++) = ',';\n";
			}
			putSeparator = true;

			for (char c : v.jsonName)
			{
				out << "\t*(s.out++) = '" << c << "';\n";
			}
			out << "\t*(s.out++) = ':';\n";
			out << "\ts.SerializeValue(this->" << var.name << ");\n";
		}

		out << SerializeToJsonDefinitionEnd;
	}
}

// A state for the matcher state machine
// Keeps track of what has been matched until now
// Only used in code generation, and in comment of output.
struct MatchState
{
	string matched;
	string gotoLabelName;
};

void PrintGotoLabel(ostream &out, const MatchState &ms)
{
	out << "\n"
	    << "\t" << ms.gotoLabelName << ": // [" << ms.matched << "] has been matched\n";
}

// When all known fields start with the same prefix
// Not matching will jump to unknown field label
void MatchOnlyPrefix(ostream &out, const string &prefix)
{
	out << "\t// Matching common prefix: [" << prefix << "]\n";
	for (char c : prefix)
	{
		out << "\tif (QUANTUMJSON_UNLIKELY(" << PeekNextChar << " != '" << c << "')) goto " << Labels::UnknownField << ";\n";
		out << "\t" << ConsumeNextChar << ";\n";
	}
}

// Returns common prefix for a range of sorted strings
string CommonPrefix(const vector<Variable>::iterator varsBegin,
                    const vector<Variable>::iterator varsEnd,
                    int matchedChars)
{
	string first = varsBegin->jsonName;
	string last = (varsEnd - 1)->jsonName;

	return string(first.begin() + matchedChars,
	              mismatch(first.begin() + matchedChars, first.end(), last.begin() + matchedChars).first);
}

void GenerateMatchers(ostream &out,
                      const MatchState &ms,
                      int &stateCounter,
                      const vector<Variable>::iterator varsBegin,
                      const vector<Variable>::iterator varsEnd,
                      size_t matchedChars = 0)
{
	if (varsBegin == varsEnd) return;

	out << "\t\n";
	out << "\t// ###\n";
	out << "\t// Matching range [" << varsBegin->jsonName << ", " << (varsEnd-1)->jsonName << "]\n";

	PrintGotoLabel(out, ms);

	string commonPrefix = CommonPrefix(varsBegin, varsEnd, matchedChars);

	if (commonPrefix.size())
	{
		// All elements share the common prefix, match it here
		MatchOnlyPrefix(out, commonPrefix);
	}

	if (varsBegin + 1 == varsEnd)
	{
		// There was only one group and we matched that already

		out << FieldNameParsed % varsBegin->cppName
		                       % (varsBegin->skipNull ? SkipNullValue : "");
		return;
	}

	matchedChars += commonPrefix.size();

	struct GroupToMatch
	{
		MatchState matchState;
		vector<Variable>::iterator begin, end;
		size_t matchedChars;
	};
	vector<GroupToMatch> unmatchedGroups;

	// Branch out now
	auto groupBegin = varsBegin;
	for (auto it = varsBegin + 1; it != varsEnd; ++it)
	{
		char initial = (it->jsonName)[matchedChars];
		if (initial != (groupBegin->jsonName)[matchedChars])
		{
			MatchState nextState = {
			    groupBegin->jsonName.substr(0, matchedChars),
			    "state_" + to_string(++stateCounter)
			};
			unmatchedGroups.emplace_back(
			    GroupToMatch{nextState, groupBegin, it, matchedChars});
			groupBegin = it;
		}
	}
	MatchState nextState = {
	    groupBegin->jsonName.substr(0, matchedChars),
	    "state_" + to_string(++stateCounter)
	};
	unmatchedGroups.emplace_back(GroupToMatch{nextState, groupBegin, varsEnd, matchedChars});

	// Create jump table
	out << "\tswitch (" << PeekNextChar << ")\n"
	    << "\t{\n";
	for (const auto &g : unmatchedGroups)
	{
		out << "\t\tcase '" << (g.begin->jsonName)[g.matchedChars] << "': "
		    << "goto " << g.matchState.gotoLabelName << ";\n";
	}
	out << "\t\tdefault: goto " << Labels::UnknownField << ";\n"
	    << "\t}\n";

	for (const auto &g : unmatchedGroups)
	{
		GenerateMatchers(out, g.matchState, stateCounter, g.begin, g.end, g.matchedChars);
	}
}

void GenerateParserForStructDef(ostream &out, const StructDef &s)
{
	out << ParseNextFieldBegin % s.name;

	vector<Variable> vars;
	for (const VariableDef &var : s.variables)
	{
		vars.push_back(Variable(var));
	}
	sort(vars.begin(), vars.end());

	out << ParserCommonStuff;

	int stateCounter = 0;
	MatchState initialState = {"", Labels::Start};
	GenerateMatchers(out, initialState, stateCounter, vars.begin(), vars.end());

	out << ParseNextFieldEnd;
}
