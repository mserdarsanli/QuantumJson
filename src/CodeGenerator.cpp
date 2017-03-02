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
#include <string>
#include <utility>

#include <boost/format.hpp>

#include "Attributes.hpp"
#include "CodeGenerator.hpp"
#include "FieldParser.hpp"
#include "Util.hpp"

// Code that will be embedded to generated output
#include "lib/LibCommonDef.hpp"

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

void GenerateParserForStruct(CodeFormatter &code, const Struct &s);
void GenerateAllocatorForStruct(CodeFormatter &code, const Struct &s);
void GenerateReserverForStruct(CodeFormatter &code, const Struct &s);

void GenerateHeaderForFile(CodeFormatter &code, const ParsedFile &file)
{
	vector<Struct> allStructs;
	for (const StructDef &s : file.structs)
	{
		allStructs.emplace_back(s);
	}

	// TODO provide an option to generate #ifndef style guards
	code.EmitLine("#pragma once");

	code.EmitCode( string(reinterpret_cast<const char *>(&libCommonData[0]), libCommonData.size()) );

	// Header declerations
	for (const Struct &s : allStructs)
	{
		code.EmitLine("struct %s", s.name.c_str());
		code.EmitLine("{");
		code.EmitLine("%s() = default;", s.name.c_str());
		code.EmitLine("%s(const %s &) = default;", s.name.c_str(), s.name.c_str());
		code.EmitLine("%s(%s &&) = default;", s.name.c_str(), s.name.c_str());
		code.EmitLine("");
		code.EmitLine("%s& operator=(const %s &) = default;", s.name.c_str(), s.name.c_str());
		code.EmitLine("");

		code.EmitLine("// Field tag numbers");
		code.EmitLine("enum class __QuantumJsonFieldTag");
		code.EmitLine("{");
		for (const Variable &var : s.allVars)
		{
			if (var.isReservable)
			{
				code.EmitLine("__QUANTUMJSON_FIELD_TAG_%s = %d,",
				    var.cppName.c_str(), var.reservableFieldTag);
			}
			else
			{
				code.EmitLine("// Skipped non-reservable field %s", var.cppName.c_str());
			}
		}
		code.EmitLine("};");

		// Member fields
		for (const Variable &var : s.allVars)
		{
			code.EmitLine("%s %s;", var.type.Render().c_str(), var.cppName.c_str());
		}

		code.EmitLine("// Parsing functions");

		code.EmitLine("void MergeFromJson(const std::string &json)");
		code.EmitLine("{");
			code.EmitLine("MergeFromJson(json.begin(), json.end());");
		code.EmitLine("}");

		code.EmitLine("template <typename InputIteratorType>");
		code.EmitLine("void MergeFromJson(InputIteratorType it, InputIteratorType end);");

		code.EmitLine("// Serialization functions");

		code.EmitLine("template <typename OutputIteratorType>");
		code.EmitLine("void SerializeTo(OutputIteratorType out) const");
		code.EmitLine("{");
			code.EmitLine("QuantumJsonImpl__::Serializer<OutputIteratorType> s(out);");
			code.EmitLine("this->SerializeTo(s);");
		code.EmitLine("}");

		code.EmitLine("template <typename OutputIteratorType>");
		code.EmitLine("void SerializeTo(QuantumJsonImpl__::Serializer<OutputIteratorType> &s) const;");

		code.EmitLine("private:");
		code.EmitLine("// Private methods that are called by parser");

		code.EmitLine("// Function that parses one field only");
		code.EmitLine("// If the object is as follows:");
		code.EmitLine("// {\"a\": \"sadsadsa\", \"b\": 123}");
		code.EmitLine("//  <------------->  <------>");
		code.EmitLine("// marked regions would map to ParseNextField calls.");
		code.EmitLine("template <typename InputIteratorType>");
		code.EmitLine("void ParseNextField(QuantumJsonImpl__::Parser<InputIteratorType> &parser);");

		code.EmitLine("// Allocator that works on random access input, not to rely on string/vector");
		code.EmitLine("// growth performance");
		code.EmitLine("template <typename InputIteratorType>");
		code.EmitLine("static");
		code.EmitLine("// TODO rename this function to something more descriptive");
		code.EmitLine("void ReserveNextField(QuantumJsonImpl__::PreAllocator<InputIteratorType> &allocator);");

		code.EmitLine("template <typename InputIteratorType>");
		code.EmitLine("void ReserveCalculatedSpace(QuantumJsonImpl__::PreAllocator<InputIteratorType> &allocator);");


		code.EmitLine("template <typename T> friend struct QuantumJsonImpl__::Parser;");
		code.EmitLine("template <typename T> friend struct QuantumJsonImpl__::PreAllocator;");
		code.EmitLine("};");
	}

	// Function definitions
	for (const Struct &s : allStructs)
	{
		GenerateParserForStruct(code, s);
		GenerateAllocatorForStruct(code, s);
		GenerateReserverForStruct(code, s);

		code.EmitLine("template <typename InputIteratorType>");
		code.EmitLine("inline");
		code.EmitLine("void %s::MergeFromJson(InputIteratorType it,", s.name.c_str());
		code.EmitLine("     %s                InputIteratorType end)", string(s.name.size(), ' ').c_str());
		code.EmitLine("{");
			code.EmitLine("QuantumJsonImpl__::Parser<InputIteratorType> parser(it, end);");
			code.EmitLine("parser.ParseObject(*this);");
			code.EmitLine("");
			code.EmitLine("// Throw when parsing fails");
			code.EmitLine("if (parser.errorCode != QuantumJsonImpl__::ErrorCode::NoError)");
			code.EmitLine("{");
				code.EmitLine("throw QuantumJsonImpl__::JsonError(parser.errorCode);");
			code.EmitLine("}");
		code.EmitLine("}");

		code.EmitLine("template <typename OutputIteratorType>");
		code.EmitLine("void %s::SerializeTo(", s.name.c_str());
		code.EmitLine("    QuantumJsonImpl__::Serializer<OutputIteratorType> &s");
		code.EmitLine("    ) const");
		code.EmitLine("{");
			code.EmitLine("*(s.out++) = '{';");


		bool putSeparator = false;
		for (const Variable &v : s.allVars)
		{
			code.EmitLine("");
			code.EmitLine("// Render field %s", v.cppName.c_str());

			if (putSeparator)
			{
				code.EmitLine("*(s.out++) = ',';");
			}
			putSeparator = true;

			code.EmitLine("*(s.out++) = '\"';");
			for (char c : v.jsonName)
			{
				code.EmitLine("*(s.out++) = '%c';", c);
			}
			code.EmitLine("*(s.out++) = '\"';");
			code.EmitLine("*(s.out++) = ':';");

			code.EmitLine("s.SerializeValue(this->%s);", v.cppName.c_str());
		}

			code.EmitLine("*(s.out++) = '}';");
		code.EmitLine("}");
	}
}

void GenerateParserForStruct(CodeFormatter &code, const Struct &s)
{
	FieldParser fp;
	for (const Variable &var : s.allVars)
	{
		CodeFormatter action;
		if (var.skipNull)
		{
			action.EmitLine("// Skip null values for this field.");
			action.EmitLine("{");
				action.EmitLine("bool skipped = false;");
				action.EmitLine("parser.MaybeSkipNull(&skipped);");
				action.EmitLine("if (skipped)");
				action.EmitLine("{");
					action.EmitLine("return;");
				action.EmitLine("}");
			action.EmitLine("}");
		}
		action.EmitLine("parser.ParseValueInto(this->%s);", var.cppName.c_str());
		fp.addField(var.jsonName, action.getFormattedCode());
	}


	code.EmitLine("template <typename InputIteratorType>");
	code.EmitLine("inline");
	code.EmitLine("void %s::ParseNextField(QuantumJsonImpl__::Parser<InputIteratorType> &parser)", s.name.c_str());
	code.EmitLine("{");
	fp.generateFieldParserCode(code);
	code.EmitLine("}");
}

void GenerateAllocatorForStruct(CodeFormatter &code, const Struct &s)
{
	FieldParser fp;
	for (const Variable &var : s.allVars)
	{
		if (var.isReservable)
		{
			CodeFormatter action;
			action.EmitLine("// Reserve space in field");
			action.EmitLine("{");
				action.EmitLine("size_t fieldSizeIdx = parser.VisitingField(");
				action.EmitLine("    static_cast<int>(__QuantumJsonFieldTag::__QUANTUMJSON_FIELD_TAG_%s));", var.cppName.c_str());
				action.EmitLine("parser.CalculateSpaceToReserveIn(fieldSizeIdx,");
				action.EmitLine("    static_cast<decltype(%s::%s)*>(nullptr));", s.name.c_str(), var.cppName.c_str());
				action.EmitLine("return;");
			action.EmitLine("}");


			fp.addField(var.jsonName, action.getFormattedCode());
		}
	}


	code.EmitLine("template <typename InputIteratorType>");
	code.EmitLine("inline");
	code.EmitLine("void %s::ReserveNextField(QuantumJsonImpl__::PreAllocator<InputIteratorType> &parser)",
	     s.name.c_str());
	code.EmitLine("{");
	fp.generateFieldParserCode(code);
	code.EmitLine("}");
}

void GenerateReserverForStruct(CodeFormatter &code, const Struct &s)
{
	code.EmitLine("template <typename InputIteratorType>");
	code.EmitLine("inline");
	code.EmitLine("void %s::ReserveCalculatedSpace(QuantumJsonImpl__::PreAllocator<InputIteratorType> &allocator)", s.name.c_str());
	code.EmitLine("{");
		code.EmitLine("size_t objectFieldsEnd = allocator.GetObjectSize();");
		code.EmitLine("allocator.PopObject();");
		code.EmitLine("while (allocator.GetCurIdx() != objectFieldsEnd)");
		code.EmitLine("{");
			code.EmitLine("int fieldTag = allocator.GetFieldTag();");
			code.EmitLine("switch(static_cast<__QuantumJsonFieldTag>(fieldTag))");
			code.EmitLine("{");

	for (const Variable &var : s.allVars)
	{
		if (var.isReservable)
		{
			code.EmitLine("case __QuantumJsonFieldTag::__QUANTUMJSON_FIELD_TAG_%s:", var.cppName.c_str());
			code.EmitLine("{");
				code.EmitLine("allocator.ReserveCalculatedSpaceIn(this->%s);", var.cppName.c_str());
				code.EmitLine("break;");
			code.EmitLine("}");
		}
	}

				code.EmitLine("default:");
				code.EmitLine("; // Should not happen");
			code.EmitLine("}");
		code.EmitLine("}");
	code.EmitLine("}");
}
