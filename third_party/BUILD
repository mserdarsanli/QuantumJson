# The MIT License (MIT)
#
# Copyright (c) 2016 Mustafa Serdar Sanli
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.

package(
	default_visibility = [
		"//visibility:public",
	],
)

cc_library(
	name = "catch",
	hdrs = [
		"catch.hpp",
	],
)

cc_library(
	name = "nlohmann_json",
	srcs = [
		"nlohmann_json_2_0_2.hpp",
	],
)

cc_library(
	name = "jsoncpp",
	srcs = [
		"jsoncpp_1_7_3/jsoncpp.cpp",
		"jsoncpp_1_7_3/json/json.h",
		"jsoncpp_1_7_3/json/json-forwards.h",
	],
)

cc_library(
	name = "rapidjson",
	srcs = [
		"rapidjson_1_1_0/rapidjson/rapidjson.h",
		"rapidjson_1_1_0/rapidjson/ostreamwrapper.h",
		"rapidjson_1_1_0/rapidjson/filereadstream.h",
		"rapidjson_1_1_0/rapidjson/msinttypes/stdint.h",
		"rapidjson_1_1_0/rapidjson/msinttypes/inttypes.h",
		"rapidjson_1_1_0/rapidjson/schema.h",
		"rapidjson_1_1_0/rapidjson/istreamwrapper.h",
		"rapidjson_1_1_0/rapidjson/prettywriter.h",
		"rapidjson_1_1_0/rapidjson/stringbuffer.h",
		"rapidjson_1_1_0/rapidjson/encodedstream.h",
		"rapidjson_1_1_0/rapidjson/stream.h",
		"rapidjson_1_1_0/rapidjson/memorybuffer.h",
		"rapidjson_1_1_0/rapidjson/encodings.h",
		"rapidjson_1_1_0/rapidjson/allocators.h",
		"rapidjson_1_1_0/rapidjson/reader.h",
		"rapidjson_1_1_0/rapidjson/error/en.h",
		"rapidjson_1_1_0/rapidjson/error/error.h",
		"rapidjson_1_1_0/rapidjson/filewritestream.h",
		"rapidjson_1_1_0/rapidjson/fwd.h",
		"rapidjson_1_1_0/rapidjson/document.h",
		"rapidjson_1_1_0/rapidjson/pointer.h",
		"rapidjson_1_1_0/rapidjson/internal/regex.h",
		"rapidjson_1_1_0/rapidjson/internal/itoa.h",
		"rapidjson_1_1_0/rapidjson/internal/meta.h",
		"rapidjson_1_1_0/rapidjson/internal/ieee754.h",
		"rapidjson_1_1_0/rapidjson/internal/stack.h",
		"rapidjson_1_1_0/rapidjson/internal/strtod.h",
		"rapidjson_1_1_0/rapidjson/internal/swap.h",
		"rapidjson_1_1_0/rapidjson/internal/dtoa.h",
		"rapidjson_1_1_0/rapidjson/internal/pow10.h",
		"rapidjson_1_1_0/rapidjson/internal/strfunc.h",
		"rapidjson_1_1_0/rapidjson/internal/biginteger.h",
		"rapidjson_1_1_0/rapidjson/internal/diyfp.h",
		"rapidjson_1_1_0/rapidjson/writer.h",
		"rapidjson_1_1_0/rapidjson/memorystream.h",
	],
)

cc_library(
	name = "staticjson",
	srcs = [
		"staticjson_0_4_1/basic.hpp",
		"staticjson_0_4_1/document.hpp",
		"staticjson_0_4_1/error.hpp",
		"staticjson_0_4_1/forward_declarations.hpp",
		"staticjson_0_4_1/io.hpp",
		"staticjson_0_4_1/primitive_types.hpp",
		"staticjson_0_4_1/staticjson.cpp",
		"staticjson_0_4_1/staticjson.hpp",
		"staticjson_0_4_1/stl_types.hpp",
	],
	deps = [
		"//third_party:rapidjson",
	],
)
