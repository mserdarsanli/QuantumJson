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

#include <string>

#include "Attributes.hpp"

const AttributeInfo VarAttributes::JsonFieldName = {
    "json_field_name",
    1,
};
const AttributeInfo VarAttributes::OnNull = {
    "on_null",
    1,
};
const AttributeInfo VarAttributes::Deprecated = {
    "deprecated",
    0,
};

const AttributeInfo* GetKnownAttribute(const std::string &attr_name)
{
	if (attr_name == "json_field_name") return &VarAttributes::JsonFieldName;
	if (attr_name == "on_null")         return &VarAttributes::OnNull;
	if (attr_name == "deprecated")      return &VarAttributes::Deprecated;

	// For testing multi arg attributes only
	if (attr_name == "three_arg_attr")
	{
		static AttributeInfo ThreeArgAttr = {
		    "three_arg_attr",
		    3,
		};
		return &ThreeArgAttr;
	}

	return nullptr;
}
