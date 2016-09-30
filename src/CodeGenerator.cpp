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
	string jsonName;
	string cppName;
	bool skipNull = false;

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

// TODO make another function accepting const char *
char MergeFromJsonDeclarations[] = R"(
	void MergeFromJson(const std::string &json)
	{
		MergeFromJson(json.begin(), json.end());
	}

	void MergeFromJson(std::string::const_iterator it, std::string::const_iterator end);
)";

// For aligning bind struct name to %1%, and bind spaces of the same length to %2%
format MergeFromJsonDefImpl(R"(
inline
void %1%::MergeFromJson(std::string::const_iterator it,
     %2%                std::string::const_iterator end)
{
	using namespace std::placeholders;
	QuantumJsonImpl__::ParseObject(it, end, *this);
}
)");

// Methods that are used by parser
// These are strictly implementation detail, so header output does not have them.
// TODO verify adding functions in struct only in source file does not affect
// ABI compatibility.
// String being parsed should be null terminated, this prevents extra checks for
// it == end, since when it == end, nothing will be matched. TODO improve this.
char ParserExtensionMethodDeclarations[] = R"(
	// Function that parses one field only
	// If the object is as follows:
	// {"a": "sadsadsa", "b": 123}
	//  <------------->  <------>
	// marked regions would map to ParseNextField calls.
	std::string::const_iterator ParseNextField(std::string::const_iterator it,
	                                           std::string::const_iterator end);

)";

format ParseNextFieldBegin(R"(
inline
std::string::const_iterator %1%::ParseNextField(std::string::const_iterator it,
                            %2%                 std::string::const_iterator end)
{
	using namespace QuantumJsonImpl__; // TODO remove
)");

format ParseNextFieldEnd(R"(
	// Should be unreachable
	throw "wqewewqewq"; // TODO find better type
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
	goto _Start; // _Start is the entry point for perser

	_UnknownField:
	// Field name is not known
	// Skip the field completely.
	// TODO make this an exception if flag is given to
	while (it != end && *it != '"')
		++it;
	if (it == end)
		throw std::runtime_error("Unexpected EOF");

	// Closing quote
	++it;

	it = SkipWhitespace(it, end);
	it = SkipChar(it, end, ':'); // Field Separator
	it = SkipWhitespace(it, end);

	it = SkipValue(it, end);

	return it; // Fone parsing this field
)";

format FieldNameParsed(R"(
	// Matched field [%1%]
	it = SkipWhitespace(it, end);
	it = SkipChar(it, end, ':'); // Field Separator
	it = SkipWhitespace(it, end);

%2%
	// Parse the actual value
	it = ParseValueInto(it, end, this->%1%);
	return it;
)");

char SkipNullValue[] = R"(
	// Field skip null values for this field.
	{
		bool skipped = false;
		it = MaybeSkipNull(it, end, &skipped);
		if (skipped)
		{
			return it;
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
void GenerateParseValueIntoSpecialization(ostream &out, const StructDef &s);

void GenerateHeaderForFile(ostream &out, const ParsedFile &file)
{
	// TODO use include guards
	out << "#pragma once\n\n";

	out << IncludeStandardHeaders;

	GenerateCommonParserDefinitions(out);

	// Header declerations
	for (const StructDef &s : file.structs)
	{
		out << "struct " << s.name << "\n"
		    << "{\n";

		for (const VariableDef &var : s.variables)
		{
			out << "\t";
			RenderVariableType(out, var.type);
			out << " " << var.name << ";\n";
		}

		out << ParserExtensionMethodDeclarations;
		out << MergeFromJsonDeclarations;
		out << "};\n";
	}

	// Function definitions
	for (const StructDef &s : file.structs)
	{
		GenerateParseValueIntoSpecialization(out, s);
		GenerateParserForStructDef(out, s);

		out << MergeFromJsonDefImpl % s.name % string(s.name.size(), ' ');
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
		out << "\tif (" << PeekNextChar << " != '" << c << "') goto " << Labels::UnknownField << ";\n";
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

		bool skipNull = false;

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

void GenerateParseValueIntoSpecialization(ostream &out, const StructDef &s)
{
	out << R"(
// TODO shitty format here
// Specialization for struct type
inline
std::string::const_iterator ParseValueInto(std::string::const_iterator it,
                                           std::string::const_iterator end,
                                           )" << s.name <<  R"( &obj)
{
	return QuantumJsonImpl__::ParseObject(it, end, obj);
}
)";
}

void GenerateParserForStructDef(ostream &out, const StructDef &s)
{
	out << ParseNextFieldBegin % s.name % string(s.name.size(), ' ');

	vector<Variable> vars;
	for (const VariableDef &var : s.variables)
	{
		Variable v;
		v.cppName = var.name;
		v.jsonName = "\"" + var.name + "\"";

		if (var.attributes.find(VarAttributes::JsonFieldName) != var.attributes.end())
		{
			v.jsonName = "\"" + var.attributes.at(VarAttributes::JsonFieldName) + "\"";
		}

		if (var.attributes.find(VarAttributes::OnNull) != var.attributes.end() &&
		    var.attributes.at(VarAttributes::OnNull) == "skip")
		{
			v.skipNull = true;
		}

		vars.push_back(v);
	}
	sort(vars.begin(), vars.end());

	out << ParserCommonStuff;

	int stateCounter = 0;
	MatchState initialState = {"", Labels::Start};
	GenerateMatchers(out, initialState, stateCounter, vars.begin(), vars.end());

	out << ParseNextFieldEnd;
}