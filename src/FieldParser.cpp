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
	stringstream out;
	int curIndent = 1;

	auto EmitLine = [&out,&curIndent](const char *fmt, ...)
	{
		char line_buffer[2000];
		va_list ap;
		va_start(ap, fmt);
		// TODO check size?
		vsnprintf(line_buffer, 2000, fmt, ap);
		va_end(ap);

		out << string(curIndent, '\t') << line_buffer << '\n';
	};

	const vector<FieldInfo> fields(fieldInfos.begin(), fieldInfos.end());

	EmitLine("parser.SkipChar('\"')");

	int nextMatchStateId = 100;

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

		if (st.matchComplete)
		{
			EmitLine("state_%d:", st.stateId);
			EmitLine("// Matched field [%s]", fields[st.firstFieldIdx].fieldName.c_str());

			EmitLine("parser.SkipWhitespace();");
			EmitLine("parser.SkipChar(':'); // Field Separator");
			EmitLine("parser.SkipWhitespace();");

			EmitLine("%s", fields[st.firstFieldIdx].matchedAction.c_str());
			EmitLine("return;");

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

		// Generate code for current state
		EmitLine("state_%d:", st.stateId);
		EmitLine("// Currently matched prefix [%s]",
		             fields[st.firstFieldIdx].fieldName.substr(0, st.matchedCharCnt).c_str());
		EmitLine("if (parser.it == parser.end)");
		EmitLine("{");
		{
			++curIndent;
			EmitLine("parser.errorCode = QuantumJsonImpl__::ErrorCode::UnexpectedEOF;");
			EmitLine("return;");
			--curIndent;
		}
		EmitLine("}");

		EmitLine("switch (*(parser.it++))");
		EmitLine("{");
		for (auto it : nextStates)
		{
			EmitLine("case '%c': goto state_%d;", it.first, it.second.stateId);
		}
		if (nextStates.count('"') == 0)
		{
			EmitLine("case '\"': goto state_unknown_field_matched;");
		}
		EmitLine("default: goto state_unknown_field_matching;");
		EmitLine("}");

		// Push the next states in reverse order so lexicographically first one
		// would be popped first.
		for (auto it = nextStates.rbegin(); it != nextStates.rend(); ++it)
		{
			matchStack.push(it->second);
		}
	}

	EmitLine("state_unknown_field_matching:");
	EmitLine("switch (*(parser.it++))");
	EmitLine("{");
	EmitLine("case '\"': goto state_unknown_field_matched;");
	EmitLine("default: goto state_unknown_field_matching;");
	EmitLine("}");

	EmitLine("state_unknown_field_matched:");
	EmitLine("parser.SkipWhitespace();");
	EmitLine("parser.SkipChar(':'); // Field Separator");
	EmitLine("parser.SkipWhitespace();");
	EmitLine("parser.SkipValue();");
	EmitLine("return;");


	return out.str();
}
