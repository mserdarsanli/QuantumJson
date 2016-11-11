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

cc_library(
	name = "src_lib",
	srcs = [
		"Attributes.cpp",
		"CodeGenerator.cpp",
		"JsonParserLibrary.cpp",
		"Parser.cpp",
		"Tokenizer.cpp",

		"Attributes.hpp",
		"CodeGenerator.hpp",
		"JsonParserLibrary.hpp",
		"Parser.hpp",
		"Tokenizer.hpp",
	],
	deps = [
		"//lib",
	],
)

cc_binary(
	name = "jc",
	srcs = [
		"Main.cpp",
		"CommandLineFlags.cpp",
		"CommandLineFlags.hpp",
	],
	deps = [
		":src_lib",
	],
	visibility = [
		"//visibility:public",
	],
)

genrule(
	name = "flags",
	srcs = [
		"CommandLineFlags.ggo",
	],
	outs = [
		"CommandLineFlags.cpp",
		"CommandLineFlags.hpp",
	],
	cmd = """gengetopt --input="$(<)" \
	    --c-extension=cpp --header-extension=hpp \
	    --file-name="CommandLineFlags"  --output-dir="$(@D)" """,
)

cc_test(
	name = "test_attributes",
	srcs = [
		"TestAttributes.cpp",
	],
	deps = [
		":src_lib",
	],
)

cc_test(
	name = "test_template_args",
	srcs = [
		"TestTemplateArgs.cpp",
	],
	deps = [
		":src_lib",
	],
)
