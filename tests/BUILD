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

load("/build/jc", "cc_json_schema_lib")

cc_json_schema_lib(
	name = "schema_lib",
	srcs = [
		"Schema1.json.hpp",
		"Schema2.json.hpp",
	],
)

cc_test(
	name = "main",
	srcs = [
		"Main.cpp",
	],
	deps = [
		":schema_lib",
	],
)

cc_test(
	name = "lib_tests",
	srcs = [
		"LibTests.cpp",
	],
	deps = [
		"//lib:lib_impl",
	],
)

cc_test(
	name = "serialize_tests",
	srcs = [
		"SerializeTests.cpp",
	],
	deps = [
		"//lib:lib_impl",
	],
)

cc_test(
	name = "serialize_object_tests",
	srcs = [
		"SerializeObjectTests.cpp",
	],
	deps = [
		":schema_lib",
	],
)

cc_test(
	name = "conformance_double",
	srcs = [
		"ConformanceDouble.cpp",
		"ConformanceDoubleData.hpp",
	],
	deps = [
		"//lib:lib_impl",
	],
)

cc_test(
	name = "conformance_string",
	srcs = [
		"ConformanceString.cpp",
		"ConformanceStringData.hpp",
	],
	deps = [
		"//lib:lib_impl",
	],
)

cc_test(
	name = "string_allocations",
	srcs = [
		"StringAllocations.cpp",
	],
	deps = [
		"//lib:lib_impl",
	],
)

cc_test(
	name = "reserved_sizes",
	srcs = [
		"ReservedSizes.cpp",
	],
	deps = [
		"//lib:lib_impl",
	],
)

cc_test(
	name = "utf8_stress_test",
	srcs = [
		"Utf8StressTest.cpp",
	],
	deps = [
		"//lib:lib_impl",
	],
)

cc_test(
	name = "multiple_schemas",
	srcs = [
		"MultipleSchemas.cpp",
		"MultipleSchemas2.cpp",
	],
	deps = [
		":schema_lib",
	],
)


# json_checker tests
JSON_CHECKER_DATA_PASS = [
	"json_checker_tests/pass1.json",
	"json_checker_tests/pass2.json",
	"json_checker_tests/pass3.json",
]

JSON_CHECKER_DATA_FAIL = [
	"json_checker_tests/fail1.json",
	"json_checker_tests/fail2.json",
	"json_checker_tests/fail3.json",
	"json_checker_tests/fail4.json",
	"json_checker_tests/fail5.json",
	"json_checker_tests/fail6.json",
	"json_checker_tests/fail7.json",
	"json_checker_tests/fail8.json",
	"json_checker_tests/fail9.json",
	"json_checker_tests/fail10.json",
	"json_checker_tests/fail11.json",
	"json_checker_tests/fail12.json",
	"json_checker_tests/fail13.json",
	"json_checker_tests/fail14.json",
	"json_checker_tests/fail15.json",
	"json_checker_tests/fail16.json",
	"json_checker_tests/fail17.json",
	"json_checker_tests/fail18.json",
	"json_checker_tests/fail19.json",
	"json_checker_tests/fail20.json",
	"json_checker_tests/fail21.json",
	"json_checker_tests/fail22.json",
	"json_checker_tests/fail23.json",
	"json_checker_tests/fail24.json",
	"json_checker_tests/fail25.json",
	"json_checker_tests/fail26.json",
	"json_checker_tests/fail27.json",
	"json_checker_tests/fail28.json",
	"json_checker_tests/fail29.json",
	"json_checker_tests/fail30.json",
	"json_checker_tests/fail31.json",
	"json_checker_tests/fail32.json",
	"json_checker_tests/fail33.json",
]

JSON_CHECKER_DATA = ( [ ("--pass", f) for f in JSON_CHECKER_DATA_PASS ]
                    + [ ("--fail", f) for f in JSON_CHECKER_DATA_FAIL ] )

filegroup(
	name = "json_checker_data",
	srcs = JSON_CHECKER_DATA_PASS + JSON_CHECKER_DATA_FAIL,
)

cc_test(
	name = "json_skip_conformance_test",
	srcs = [
		"JsonSkipConformanceChecker.cpp",
	],
	deps = [
		"//lib:lib_impl",
	],
	args = [
		arg
		for test_args in JSON_CHECKER_DATA
		for arg in [ test_args[0], "tests/" + test_args[1] ]
	],
	data = [
		":json_checker_data",
	],
)
