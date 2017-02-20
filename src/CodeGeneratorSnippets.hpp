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

#include <map>
#include <string>

#include <boost/algorithm/string/replace.hpp>

// Helper functions to generate pieces of code by CoodeGenerator.cpp

using std::map;
using std::string;
using boost::algorithm::replace_all;

class Template
{
public:
	Template(const string &tmpl)
	  : tmpl(tmpl)
	{
	}

	string format(const map<string, string> &vars)
	{
		string res = tmpl;
		for (auto it : vars)
		{
			replace_all(res, it.first, it.second);
		}
		return res;
	}

private:
	const string tmpl;
};

string StructDefBegin(const string &className)
{
	Template tmpl(
	    "struct ${className}\n"
	    "{\n"
	    "	${className}() = default;\n"
	    "	${className}(const ${className} &) = default;\n"
	    "	${className}(${className} &&) = default;\n"
	    "\n"
	    "	${className}& operator=(const ${className} &) = default;\n"
	);
	return tmpl.format({
	    {"${className}", className},
	});
}

string StructDefEnd()
{
	Template tmpl(
	    "\t"    "template <typename T> friend struct QuantumJsonImpl__::Parser;\n"
	    "\t"    "template <typename T> friend struct QuantumJsonImpl__::PreAllocator;\n"
	    "};\n"
	    "\n"
	);
	return tmpl.format({
	});
}

string IncludeGuard()
{
	// TODO provide an option to generate #ifndef style guards
	Template tmpl(
	    "#pragma once\n"
	);
	return tmpl.format({
	});
}

string SerializeToJsonDefinitionBegin(const string &className)
{
	Template tmpl(
	    "template <typename OutputIteratorType>\n"
	    "void ${className}::SerializeTo(\n"
	    "    QuantumJsonImpl__::Serializer<OutputIteratorType> &s\n"
	    "    ) const\n"
	    "{\n"
	    "\t"    "*(s.out++) = '{';\n"
	);
	return tmpl.format({
	    {"${className}", className},
	});
}

string SerializeToJsonDefinitionEnd()
{
	Template tmpl(
	    "\t"    "*(s.out++) = '}';\n"
	    "}\n"
	);
	return tmpl.format({
	});
}

string GotoLabel(const string &gotoLabel, const string &matched)
{
	Template tmpl(
	    "\n"
	    "\t"    "${gotoLabel}: // [${matched}] has been matched\n"
	);
	return tmpl.format({
	    {"${gotoLabel}", gotoLabel},
	    {"${matched}", matched},
	});
}

string MemberFunctionDeclarations()
{
	Template tmpl(
	    "\t"    "// Parsing functions\n"
	    "\n"
	    "\t"    "void MergeFromJson(const std::string &json)\n"
	    "\t"    "{\n"
	    "\t"    "\t"    "MergeFromJson(json.begin(), json.end());\n"
	    "\t"    "}\n"
	    "\t"    "\n"
	    "\t"    "template <typename InputIteratorType>\n"
	    "\t"    "void MergeFromJson(InputIteratorType it, InputIteratorType end);\n"
	    "\n"
	    "\t"    "// Serialization functions\n"
	    "\n"
	    "\t"    "template <typename OutputIteratorType>\n"
	    "\t"    "void SerializeTo(OutputIteratorType out) const\n"
	    "\t"    "{\n"
	    "\t"    "\t"    "QuantumJsonImpl__::Serializer<OutputIteratorType> s(out);\n"
	    "\t"    "\t"    "this->SerializeTo(s);\n"
	    "\t"    "}\n"
	    "\n"
	    "\t"    "template <typename OutputIteratorType>\n"
	    "\t"    "void SerializeTo(QuantumJsonImpl__::Serializer<OutputIteratorType> &s) const;\n"
	    "\n"
	    "private:\n"
	    "\t"    "// Private methods that are called by parser\n"
	    "\n"
	    "\t"    "// Function that parses one field only\n"
	    "\t"    "// If the object is as follows:\n"
	    "\t"    "// {\"a\": \"sadsadsa\", \"b\": 123}\n"
	    "\t"    "//  <------------->  <------>\n"
	    "\t"    "// marked regions would map to ParseNextField calls.\n"
	    "\t"    "template <typename InputIteratorType>\n"
	    "\t"    "void ParseNextField(QuantumJsonImpl__::Parser<InputIteratorType> &parser);\n"
	    "\n"
	    "\t"    "// Allocator that works on random access input, not to rely on string/vector\n"
	    "\t"    "// growth performance\n"
	    "\t"    "template <typename InputIteratorType>\n"
	    "\t"    "static\n"
	    "\t"    "// TODO rename this function to something more descriptive\n"
	    "\t"    "void ReserveNextField(QuantumJsonImpl__::PreAllocator<InputIteratorType> &allocator);\n"
	    "\n"
	    "\t"    "template <typename InputIteratorType>\n"
	    "\t"    "void ReserveCalculatedSpace(QuantumJsonImpl__::PreAllocator<InputIteratorType> &allocator);\n"
	    "\n"
	);
	return tmpl.format({
	});
}

string FieldNameMatched(const string &cppFieldName)
{
	Template tmpl(
	    "\t"    "// Matched field [${cppFieldName}]\n"
	    "\t"    "parser.SkipWhitespace();\n"
	    "\t"    "parser.SkipChar(':'); // Field Separator\n"
	    "\t"    "parser.SkipWhitespace();\n"
	    "\n"
	);
	return tmpl.format({
	    {"${cppFieldName}", cppFieldName},
	});
}

string ReserveValueIntoField(const string &className, const Variable &v)
{
	Template tmpl(
	    "\t"    "// Reserve space in field\n"
	    "\t"    "{\n"
	    "\t"    "\t"    "size_t fieldSizeIdx = parser.VisitingField(\n"
	    "\t"    "\t"    "    static_cast<int>(${fieldTag}));\n"
	    "\t"    "\t"    "parser.CalculateSpaceToReserveIn(fieldSizeIdx,\n"
	    "\t"    "\t"    "    static_cast<decltype(${className}::${cppFieldName})*>(nullptr));\n"
	    "\t"    "\t"    "return;\n"
	    "\t"    "}\n"
	);

	return tmpl.format({
	    {"${className}", className},
	    {"${cppFieldName}", v.cppName},
	    {"${fieldTag}", "__QuantumJsonFieldTag::__QUANTUMJSON_FIELD_TAG_" + v.cppName},
	});
}

string ParseValueIntoField(const Variable &v)
{
	Template tmpl(
	    "\t"    "// Parse the actual value\n"
	    "\t"    "parser.ParseValueInto(this->${cppFieldName});\n"
	    "\t"    "return;\n"
	);
	return tmpl.format({
	    {"${cppFieldName}", v.cppName},
	});
}

string MaybeSkipNullValue()
{
	Template tmpl(
	    "\t"    "// Skip null values for this field.\n"
	    "\t"    "{\n"
	    "\t"    "\t"    "bool skipped = false;\n"
	    "\t"    "\t"    "parser.MaybeSkipNull(&skipped);\n"
	    "\t"    "\t"    "if (skipped)\n"
	    "\t"    "\t"    "{\n"
	    "\t"    "\t"    "\t"    "return;\n"
	    "\t"    "\t"    "}\n"
	    "\t"    "}\n"
	);
	return tmpl.format({
	});
}

string MergeFromJsonDefImpl(const string className)
{
	Template tmpl(
	    "template <typename InputIteratorType>\n"
	    "inline\n"
	    "void ${className}::MergeFromJson(InputIteratorType it,\n"
	    "     ${alignName}                InputIteratorType end)\n"
	    "{\n"
	    "\t"    "QuantumJsonImpl__::Parser<InputIteratorType> parser(it, end);\n"
	    "\t"    "parser.ParseObject(*this);\n"
	    "\n"
	    "\t"    "// Throw when parsing fails\n"
	    "\t"    "if (parser.errorCode != QuantumJsonImpl__::ErrorCode::NoError)\n"
	    "\t"    "{\n"
	    "\t"    "\t"    "throw QuantumJsonImpl__::JsonError(parser.errorCode);\n"
	    "\t"    "}\n"
	    "}\n"
	);
	return tmpl.format({
	    {"${className}", className},
	    {"${alignName}", string(className.size(), ' ')},
	});
}

string ParseNextFieldBegin(const string className)
{
	Template tmpl(
	    "template <typename InputIteratorType>\n"
	    "inline\n"
	    "void ${className}::ParseNextField(QuantumJsonImpl__::Parser<InputIteratorType> &parser)\n"
	    "{\n"
	    "\n"
	);
	return tmpl.format({
	    {"${className}", className},
	});
}

string ReserveNextFieldBegin(const string className)
{
	Template tmpl(
	    "template <typename InputIteratorType>\n"
	    "inline\n"
	    "void ${className}::ReserveNextField(QuantumJsonImpl__::PreAllocator<InputIteratorType> &parser)\n"
	    "{\n"
	    "\n"
	);
	return tmpl.format({
	    {"${className}", className},
	});
}

string ParseNextFieldEnd()
{
	Template tmpl(
	    "\t"    "// Should be unreachable\n"
	    "}\n"
	);
	return tmpl.format({
	});
}

string ReserveCalculatedSpaceBegin(const string className)
{
	Template tmpl(
	    "template <typename InputIteratorType>\n"
	    "inline\n"
	    "void ${className}::ReserveCalculatedSpace(QuantumJsonImpl__::PreAllocator<InputIteratorType> &allocator)\n"
	    "{\n"
	    "\t"    "size_t objectFieldsEnd = allocator.GetObjectSize();\n"
	    "\t"    "allocator.PopObject();\n"
	    "\t"    "while (allocator.GetCurIdx() != objectFieldsEnd)\n"
	    "\t"    "{\n"
	    "\t"    "\t"    "int fieldTag = allocator.GetFieldTag();\n"
	    "\t"    "\t"    "switch(static_cast<__QuantumJsonFieldTag>(fieldTag))\n"
	    "\t"    "\t"    "{\n"
	);
	return tmpl.format({
	    {"${className}", className},
	});
}

string ReserveCalculatedSpaceEnd()
{
	Template tmpl(
	    "\t"    "\t"    "default:\n"
	    "\t"    "\t"    "\t"    "; // Should not happen\n"
	    "\t"    "\t"    "}\n"
	    "\t"    "}\n"
	    "}\n"
	);
	return tmpl.format({
	});
}

string ReserveNextFieldEnd()
{
	Template tmpl(
	    "\t"    "// Should be unreachable\n"
	    "}\n"
	);
	return tmpl.format({
	});
}

string ParserCommonStuff(bool varsEmpty)
{
	Template tmpl(
	    "\t"    "auto &it = parser.it;\n"
	    "\t"    "auto &end = parser.end;\n"
	    "\n"
	    "\t"    "goto _Start; // _Start is the entry point for parser\n"
	    "\n"
	    "\t"    "_UnknownField:\n"
	    "\t"    "// Field name is not known\n"
	    "\t"    "// Skip the field completely.\n"
	    "\t"    "// TODO make this an exception if flag is given to\n"
	    "\t"    "while (it != end && *it != '\"')\n"
	    "\t"    "\t"    "++it;\n"
	    "\t"    "if (it == end)\n"
	    "\t"    "{\n"
	    "\t"    "\t"    "parser.errorCode = QuantumJsonImpl__::ErrorCode::UnexpectedEOF;\n"
	    "\t"    "\t"    "return;\n"
	    "\t"    "}\n"
	    "\n"
	    "\t"    "// Closing quote\n"
	    "\t"    "++it;\n"
	    "\n"
	    "\t"    "parser.SkipWhitespace();\n"
	    "\t"    "parser.SkipChar(':'); // Field Separator\n"
	    "\t"    "parser.SkipWhitespace();\n"
	    "\n"
	    "\t"    "parser.SkipValue();\n"
	    "\n"
	    "\t"    "return; // Done parsing this field\n"
	    "${varsEmpty}"
	);
	// TODO FIXME clean up this mess
	return tmpl.format({
	    {"${varsEmpty}",
	        ( varsEmpty ? "\t"    "_Start: // [] has been matched\n"
	                      "\t"    "// Matching common prefix: [\"]\n"
	                      "\t"    "if (QUANTUMJSON_UNLIKELY(*it != '\"')) goto _UnknownField;\n"
	                      "\t"    "\t"    "++it;\n"
	                      "\t"    "goto _UnknownField;\n"
	                    : "" ) },
	});
}

string MatchKnownFieldChar(char c)
{
	Template tmpl(
	    "\t"    "if (QUANTUMJSON_UNLIKELY(*it != '${expectedChar}')) goto _UnknownField;\n"
	    "\t"    "++it;\n"
	);
	return tmpl.format({
	    {"${expectedChar}", string(1, c)},
	});
}

string CaseCharGotoLabel(char c, const string &label)
{
	Template tmpl(
	    "\t"    "\t"    "case '${expectedChar}': goto ${label};\n"
	);
	return tmpl.format({
	    {"${expectedChar}", string(1, c)},
	    {"${label}", label},
	});
}

string CaseDefaultGotoUnknownField()
{
	Template tmpl(
	    "\t"    "\t"    "default: goto _UnknownField;\n"
	);
	return tmpl.format({
	});
}

string SwitchOnNextChar()
{
	Template tmpl(
	    "\t"    "// Create jump table\n"
	    "\t"    "switch (*it)\n"
	    "\t"    "{\n"
	);
	return tmpl.format({
	});
}

string SwitchEnd()
{
	Template tmpl(
	    "\t"    "}\n"
	);
	return tmpl.format({
	});
}

string VariableDefinition(const string &varType, const string &varName)
{
	Template tmpl(
	    "\t"    "${varType} ${varName};\n"
	);
	return tmpl.format({
	    {"${varType}", varType},
	    {"${varName}", varName},
	});
}

string MatchCommonPrefixBegin(const string &prefix)
{
	Template tmpl(
	    "\t"    "// Matching common prefix: [${prefix}]\n"
	);
	return tmpl.format({
	    {"${prefix}", prefix},
	});
}

string MatchRangeBegin(const string &first, const string &last)
{
	Template tmpl(
	    "\t"    "\n"
	    "\t"    "// ###\n"
	    "\t"    "// Matching range [${first}, ${last}]\n"
	);
	return tmpl.format({
	    {"${first}", first},
	    {"${last}", last},
	});
}

string RenderFieldBegin(const string &fieldName)
{
	Template tmpl(
	    "\n"
	    "\t"    "// Render field ${fieldName}\n"
	);
	return tmpl.format({
	    {"${fieldName}", fieldName},
	});
}

string PutFieldSeperator()
{
	Template tmpl(
	    "\t"    "*(s.out++) = ',';\n"
	);
	return tmpl.format({
	});
}

string PutCharacter(char c)
{
	Template tmpl(
	    "\t"    "*(s.out++) = '${char}';\n"
	);
	return tmpl.format({
	    {"${char}", string(1, c)},
	});
}

string RenderFieldNameEnd()
{
	Template tmpl(
	    "\t"    "*(s.out++) = ':';\n"
	);
	return tmpl.format({
	});
}

string SerializeFieldValue(const string &varName)
{
	Template tmpl(
	    "\t"    "s.SerializeValue(this->${varName});\n"
	);
	return tmpl.format({
	    {"${varName}", varName},
	});
}

string StructTagEnumBegin()
{
	Template tmpl(
	    "\n"
	    "\t"    "// Field tag numbers\n"
	    "\t"    "enum class __QuantumJsonFieldTag\n"
	    "\t"    "{\n"
	);
	return tmpl.format({
	});
}

string StructTagEnumEnd()
{
	Template tmpl(
	    "\t"    "};\n"
	    "\n"
	);
	return tmpl.format({
	});
}

string StructTagEnumValue(const string &varName, const string &intValue)
{
	Template tmpl(
	    "\t"    "\t"    "__QUANTUMJSON_FIELD_TAG_${varName} = ${intValue},\n"
	);
	return tmpl.format({
	    {"${varName}", varName},
	    {"${intValue}", intValue},
	});
}
