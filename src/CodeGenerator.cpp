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
	out << IncludeGuard();

	GenerateCommonParserDefinitions(out);

	// Header declerations
	for (const StructDef &s : file.structs)
	{
		out << StructDefBegin( s.name );

		for (const VariableDef &var : s.variables)
		{
			out << "\t";
			RenderVariableType(out, var.type);
			out << " " << var.name << ";\n";
		}

		out << MemberFunctionDeclarations();

		out << StructDefEnd();
	}

	// Function definitions
	for (const StructDef &s : file.structs)
	{
		GenerateParserForStructDef(out, s);

		out << MergeFromJsonDefImpl(s.name);

		out << SerializeToJsonDefinitionBegin(s.name);

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
		out << ParseValueIntoField(varsBegin->cppName);
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
	out << ParseNextFieldBegin(s.name);

	vector<Variable> vars;
	for (const VariableDef &var : s.variables)
	{
		vars.push_back(Variable(var));
	}
	sort(vars.begin(), vars.end());

	out << ParserCommonStuff();

	int stateCounter = 0;
	MatchState initialState = {"", Labels::Start};
	GenerateMatchers(out, initialState, stateCounter, vars.begin(), vars.end());

	out << ParseNextFieldEnd();
}
