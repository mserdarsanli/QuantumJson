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
#include <cstdarg>
#include <stack>
#include <sstream>
#include <utility>

#include <boost/format.hpp>

#include "Attributes.hpp"
#include "CodeGenerator.hpp"
#include "FieldParser.hpp"
#include "JsonParserLibrary.hpp"

using namespace std;
using boost::format;

struct Variable
{
	string cppName;
	string jsonName;

	VariableTypeDef type;

	bool skipNull = false;
	bool isReservable = false;

	int reservableFieldTag = -1;

	explicit Variable(const VariableDef &var)
	  : cppName(var.name)
	  , jsonName(var.name)
	  , type(var.type)
	{
		if (var.attributes.find(VarAttributes::JsonFieldName.name) != var.attributes.end())
		{
			this->jsonName = var.attributes.at(VarAttributes::JsonFieldName.name).args[0];
		}

		if (var.attributes.find(VarAttributes::OnNull.name) != var.attributes.end() &&
		    var.attributes.at(VarAttributes::OnNull.name).args[0] == "skip")
		{
			this->skipNull = true;
		}

		isReservable = ( type.typeName != "int"
		              && type.typeName != "double"
		              && type.typeName != "bool" );
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

// TODO move this somewhere else
#include "CodeGeneratorSnippets.hpp"

struct Struct
{
	Struct(const StructDef &structDef)
	{
		int reservableFieldTag = 0;
		name = structDef.name;
		for (const VariableDef &vDef : structDef.variables)
		{
			Variable v(vDef);
			if (v.isReservable)
			{
				v.reservableFieldTag = (++reservableFieldTag);
			}

			allVars.push_back(v);
		}
	}

	string name;
	vector<Variable> allVars;
};

void GenerateParserForStruct(ostream &out, const Struct &s);
void GenerateAllocatorForStruct(ostream &out, const Struct &s);
void GenerateReserverForStruct(ostream &out, const Struct &s);

void GenerateHeaderForFile(ostream &out, const ParsedFile &file)
{
	vector<Struct> allStructs;
	for (const StructDef &s : file.structs)
	{
		allStructs.emplace_back(s);
	}

	out << IncludeGuard();

	GenerateCommonParserDefinitions(out);

	// Header declerations
	for (const Struct &s : allStructs)
	{
		out << StructDefBegin( s.name );

		// Field tag enum
		out << StructTagEnumBegin();
		for (const Variable &var : s.allVars)
		{
			if (var.isReservable)
			{
				out << StructTagEnumValue(var.cppName, to_string(var.reservableFieldTag));
			}
			else
			{
				out << "\t\t// Skipped non-reservable field " << var.cppName << "\n";
			}
		}
		out << StructTagEnumEnd();

		// Member fields
		for (const Variable &var : s.allVars)
		{
			out << VariableDefinition(var.type.Render(), var.cppName);
		}

		out << MemberFunctionDeclarations();

		out << StructDefEnd();
	}

	// Function definitions
	for (const Struct &s : allStructs)
	{
		GenerateParserForStruct(out, s);
		GenerateAllocatorForStruct(out, s);
		GenerateReserverForStruct(out, s);

		out << MergeFromJsonDefImpl(s.name);

		out << SerializeToJsonDefinitionBegin(s.name);

		bool putSeparator = false;
		for (const Variable &v : s.allVars)
		{
			out << RenderFieldBegin(v.cppName);

			if (putSeparator)
			{
				out << PutFieldSeperator();
			}
			putSeparator = true;

			out << PutCharacter('"');
			for (char c : v.jsonName)
			{
				out << PutCharacter(c);
			}
			out << PutCharacter('"');
			out << RenderFieldNameEnd();
			out << SerializeFieldValue(v.cppName);
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

void GenerateParserForStruct(ostream &out, const Struct &s)
{
	FieldParser fp;
	for (const Variable &var : s.allVars)
	{
		stringstream action;
		if (var.skipNull)
		{
			action << MaybeSkipNullValue();
		}
		action << "parser.ParseValueInto(this->" << var.cppName << ");";
		fp.addField(var.jsonName, action.str());
	}

	// TODO refactor `EmitLine` functions to a shared helper class?
	auto EmitLine = [&out](const char *fmt, ...)
	{
		char line_buffer[2000];
		va_list ap;
		va_start(ap, fmt);
		// TODO check size?
		vsnprintf(line_buffer, 2000, fmt, ap);
		va_end(ap);

		out << line_buffer << '\n';
	};


	EmitLine("template <typename InputIteratorType>");
	EmitLine("inline");
	EmitLine("void %s::ParseNextField(QuantumJsonImpl__::Parser<InputIteratorType> &parser)", s.name.c_str());
	EmitLine("{");
	out << fp.generateFieldParserCode();
	EmitLine("}");
}

void GenerateAllocatorForStruct(ostream &out, const Struct &s)
{
	FieldParser fp;
	for (const Variable &var : s.allVars)
	{
		if (var.isReservable)
		{
			fp.addField(var.jsonName,
			    ReserveValueIntoField(s.name, var));
		}
	}

	// TODO refactor `EmitLine` functions to a shared helper class?
	auto EmitLine = [&out](const char *fmt, ...)
	{
		char line_buffer[1000];
		va_list ap;
		va_start(ap, fmt);
		// TODO check size?
		vsnprintf(line_buffer, 1000, fmt, ap);
		va_end(ap);

		out << line_buffer << '\n';
	};


	EmitLine("template <typename InputIteratorType>");
	EmitLine("inline");
	EmitLine("void %s::ReserveNextField(QuantumJsonImpl__::PreAllocator<InputIteratorType> &parser)",
	     s.name.c_str());
	EmitLine("{");
	out << fp.generateFieldParserCode();
	EmitLine("}");
}

void GenerateReserverForStruct(ostream &out, const Struct &s)
{
	out << ReserveCalculatedSpaceBegin(s.name);

	for (const Variable &var : s.allVars)
	{
		if (var.isReservable)
		{
			out << "\t\tcase __QuantumJsonFieldTag::__QUANTUMJSON_FIELD_TAG_" << var.cppName << ":\n";
			out << "\t\t\tallocator.ReserveCalculatedSpaceIn(this->" << var.cppName << ");\n";
			out << "\t\t\tbreak;\n";
		}
	}

	out << ReserveCalculatedSpaceEnd();
}
