// The MIT License (MIT)
//
// Copyright (c) 2017 Mustafa Serdar Sanli
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

#include <cstdarg>
#include <map>
#include <sstream>
#include <stack>
#include <vector>

#include "FieldParser.hpp"

using namespace std;


void FieldParser::addField(const string fieldName,
                           const string matchedAction)
{
	FieldInfo fieldInfo(fieldName, matchedAction);

	if (fieldInfos.find(fieldInfo) != fieldInfos.end())
	{
		throw runtime_error( "Field already present: " + fieldName );
	}

	fieldInfos.insert(fieldInfo);
}

string FieldParser::generateFieldParserCode()
{
	CodeFormatter code;
	this->generateFieldParserCode(code);
	return code.getFormattedCode();
}

void FieldParser::generateFieldParserCode(CodeFormatter &code)
{
	const vector<FieldInfo> fields(fieldInfos.begin(), fieldInfos.end());

	code.EmitLine("parser.SkipChar('\"');");

	int nextMatchStateId = 100;
	int firstStateId = nextMatchStateId + 1;

	struct MatchState
	{
		// Unique id for current state, used to generate jump labels
		int stateId = -1;

		// Field that can be matched from current state
		int firstFieldIdx = -1;
		int lastFieldIdx = -1;

		// Currently matched prefix
		int matchedCharCnt = -1;

		// True if field is matched completely, including the
		// string quote
		bool matchComplete = false;

		MatchState() = default;
	};

	std::stack<MatchState> matchStack;

	if (fields.size())
	{
		MatchState init;
		init.stateId = ++nextMatchStateId;
		init.firstFieldIdx = 0;
		init.lastFieldIdx = fields.size() - 1;
		init.matchedCharCnt = 0;
		matchStack.push(init);
	}

	while (matchStack.size())
	{
		MatchState st = matchStack.top();
		matchStack.pop();

		// Generate code for current state

		if (st.stateId != firstStateId) // First label emits unused warnings
		{
			code.EmitLine("state_%d:", st.stateId);
		}

		if (st.matchComplete)
		{
			code.EmitLine("// Matched field [%s]", fields[st.firstFieldIdx].fieldName.c_str());

			code.EmitLine("parser.SkipWhitespace();");
			code.EmitLine("parser.SkipChar(':'); // Field Separator");
			code.EmitLine("parser.SkipWhitespace();");

			code.EmitLine("%s", fields[st.firstFieldIdx].matchedAction.c_str());
			code.EmitLine("return;");

			continue;
		}

		// Map from next consumed char to the transitioned state
		map<char, MatchState> nextStates;

		for (int i = st.firstFieldIdx; i <= st.lastFieldIdx; ++i)
		{
			const std::string &fieldName = fields[i].fieldName;

			// Helper lambda
			auto GenMatchState = [&]() -> MatchState
			{
				MatchState ms;
				ms.stateId = ++nextMatchStateId;
				ms.matchedCharCnt = st.matchedCharCnt + 1;
				ms.firstFieldIdx = i;
				ms.lastFieldIdx = i;
				return ms;
			};

			if (st.matchedCharCnt == (int)fields[i].fieldName.size())
			{
				// Want to match '"' for this field
				MatchState nextState = GenMatchState();
				nextState.matchComplete = true;
				nextStates[ '"' ] = nextState;
			}
			else
			{
				char nextChar = fieldName[st.matchedCharCnt];
				if (nextStates.find(nextChar) == nextStates.end())
					nextStates[nextChar] = GenMatchState();

				MatchState &nextState = nextStates[nextChar];
				if (nextState.firstFieldIdx == -1 || nextState.firstFieldIdx > i)
					nextState.firstFieldIdx = i;
				if (nextState.lastFieldIdx == -1 || nextState.lastFieldIdx < i)
					nextState.lastFieldIdx = i;
			}
		}

		code.EmitLine("// Currently matched prefix [%s]",
		             fields[st.firstFieldIdx].fieldName.substr(0, st.matchedCharCnt).c_str());
		code.EmitLine("if (parser.it == parser.end)");
		code.EmitLine("{");
			code.EmitLine("parser.errorCode = QuantumJsonImpl__::ErrorCode::UnexpectedEOF;");
			code.EmitLine("return;");
		code.EmitLine("}");

		code.EmitLine("switch (*(parser.it++))");
		code.EmitLine("{");
		for (auto it : nextStates)
		{
			code.EmitLine("case '%c': goto state_%d;", it.first, it.second.stateId);
		}
		if (nextStates.count('"') == 0)
		{
			code.EmitLine("case '\"': goto state_unknown_field_matched;");
		}
		code.EmitLine("default: goto state_unknown_field_matching;");
		code.EmitLine("}");

		// Push the next states in reverse order so lexicographically first one
		// would be popped first.
		for (auto it = nextStates.rbegin(); it != nextStates.rend(); ++it)
		{
			matchStack.push(it->second);
		}
	}

	code.EmitLine("state_unknown_field_matching:");
	code.EmitLine("switch (*(parser.it++))");
	code.EmitLine("{");
	code.EmitLine("case '\"': goto state_unknown_field_matched;");
	code.EmitLine("default: goto state_unknown_field_matching;");
	code.EmitLine("}");

	code.EmitLine("state_unknown_field_matched:");
	code.EmitLine("parser.SkipWhitespace();");
	code.EmitLine("parser.SkipChar(':'); // Field Separator");
	code.EmitLine("parser.SkipWhitespace();");
	code.EmitLine("parser.SkipValue();");
	code.EmitLine("return;");
}
