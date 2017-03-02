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

#include <set>
#include <string>

#include "Util.hpp"

// State machine generator used to match known field names while parsing json
// objects. This allows preventing unnecessary allocations, which is one of
// the main reasons making QuantumJson fast and memory efficient.
//
// Usage:
//     StateMachine sm;
//     sm.addField("id",   "parser.ParseValueInto(this->id);"   );
//     sm.addField("name", "parser.ParseValueInto(this->name);" );
//
//     sm.generateFieldParserCode();
//
// Generates a state machine parser function which works as:
//     "id":123              // ParseValueInto should parse 123
//     "name":"Serdar"       // ParseValueInto should parse "Serdar"
//     "anotherField": 123   // value skipped as there are no matchers for it
class FieldParser
{
public:
	// Adds field to the state machine, and emits `matchedAction` code
	// where it is parsed. `matchedAction` should consume the following
	// value.
	void addField(const std::string fieldName,
	              const std::string matchedAction);
	std::string generateFieldParserCode();
	void generateFieldParserCode(CodeFormatter &code);

private:
	struct FieldInfo
	{
		std::string fieldName;
		std::string matchedAction;

		FieldInfo(const std::string &fieldName, const std::string &matchedAction)
		  : fieldName(fieldName), matchedAction(matchedAction)
		{
		}

		bool operator<(const FieldInfo &ot) const
		{
			return fieldName < ot.fieldName;
		}

		bool operator==(const FieldInfo &ot) const
		{
			return fieldName == ot.fieldName;
		}
	};

	std::set<FieldInfo> fieldInfos;
};
