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
#include "CodeGeneratorSnippets.hpp"
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

void GenerateParserForStructDef(ostream &out, const StructDef &s);
void GenerateAllocatorForStructDef(ostream &out, const StructDef &s);

void GenerateHeaderForFile(ostream &out, const ParsedFile &file)
{
	out << IncludeGuard();

	GenerateCommonParserDefinitions(out);

	// Header declerations
	for (const StructDef &s : file.structs)
	{
		out << StructDefBegin( s.name );

		for (const VariableDef &var : s.variables)
		{
			out << VariableDefinition(var.type.Render(), var.name);
		}

		out << MemberFunctionDeclarations();

		out << StructDefEnd();
	}

	// Function definitions
	for (const StructDef &s : file.structs)
	{
		GenerateParserForStructDef(out, s);
		GenerateAllocatorForStructDef(out, s);

		out << MergeFromJsonDefImpl(s.name);

		out << SerializeToJsonDefinitionBegin(s.name);

		bool putSeparator = false;
		for (const VariableDef &var : s.variables)
		{
			Variable v(var);

			out << RenderFieldBegin(v.cppName);

			if (putSeparator)
			{
				out << PutFieldSeperator();
			}
			putSeparator = true;

			for (char c : v.jsonName)
			{
				out << PutCharacter(c);
			}
			out << RenderFieldNameEnd();
			out << SerializeFieldValue(var.name);
		}

		out << SerializeToJsonDefinitionEnd();
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

// When all known fields start with the same prefix
// Not matching will jump to unknown field label
void MatchOnlyPrefix(ostream &out, const string &prefix)
{
	out << MatchCommonPrefixBegin(prefix);
	for (char c : prefix)
	{
		out << MatchKnownFieldChar(c);
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
                      string (*fieldMatchedAction)(const string &cppFieldName),
                      size_t matchedChars = 0)
{
	if (varsBegin == varsEnd) return;

	out << MatchRangeBegin(varsBegin->jsonName, (varsEnd-1)->jsonName);

	out << GotoLabel(ms.gotoLabelName, ms.matched);

	string commonPrefix = CommonPrefix(varsBegin, varsEnd, matchedChars);

	if (commonPrefix.size())
	{
		// All elements share the common prefix, match it here
		MatchOnlyPrefix(out, commonPrefix);
	}

	if (varsBegin + 1 == varsEnd)
	{
		// There was only one group and we matched that already

		out << FieldNameMatched(varsBegin->cppName);
		if (varsBegin->skipNull)
		{
			out << MaybeSkipNullValue();
		}
		out << fieldMatchedAction(varsBegin->cppName);
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

	out << SwitchOnNextChar();
	for (const auto &g : unmatchedGroups)
	{
		out << CaseCharGotoLabel(g.begin->jsonName[g.matchedChars], g.matchState.gotoLabelName);
	}
	out << CaseDefaultGotoUnknownField();
	out << SwitchEnd();

	for (const auto &g : unmatchedGroups)
	{
		GenerateMatchers(out, g.matchState, stateCounter, g.begin, g.end,
		    fieldMatchedAction, g.matchedChars);
	}
}

void GenerateParserForStructDef(ostream &out, const StructDef &s)
{
	out << ParseNextFieldBegin(s.name);

	vector<Variable> vars;
	for (const VariableDef &var : s.variables)
	{
		vars.push_back(Variable(var));
	}
	sort(vars.begin(), vars.end());

	out << ParserCommonStuff();

	int stateCounter = 0;
	MatchState initialState = {"", "_Start"};
	GenerateMatchers(out, initialState, stateCounter, vars.begin(), vars.end(), &ParseValueIntoField);

	out << ParseNextFieldEnd();
}

void GenerateAllocatorForStructDef(ostream &out, const StructDef &s)
{
	out << ReserveNextFieldBegin(s.name);

	vector<Variable> vars;
	for (const VariableDef &var : s.variables)
	{
		if (var.IsReservableType())
		{
			vars.push_back(Variable(var));
		}
		else
		{
			out << "\t// Ignored non-reservable var: " << var.name
			    << " of type " << var.type.typeName << "\n";
		}
	}
	sort(vars.begin(), vars.end());

	out << ParserCommonStuff();

	int stateCounter = 0;
	MatchState initialState = {"", "_Start"};
	GenerateMatchers(out, initialState, stateCounter, vars.begin(), vars.end(), &ReserveValueIntoField);

	out << ReserveNextFieldEnd();
}
