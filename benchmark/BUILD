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

# Data files
filegroup(
	name = "data",
	srcs = [
		"data/reddit_front.json",
		"data/omdb_frozen.json",
		"data/stackoverflow_users.json",
	],
)

# Library to be loaded with LD_PRELOAD for memory benchmarks
# which overrides malloc and reports memory usage.
cc_library(
	name = "memory_reporter",
	srcs = [
		"BenchmarkMemoryReporter.cpp",
	],
	linkopts = [
		"-ldl",
	],
)

BENCHMARK_MODES = [
	{
		"id": "benchmark_speed",
		"name": "Speed Benchmark",
		"copts": ["-DBENCHMARK_SPEED"],
		"runtime_env": "",
		"tools": [],
		"repeat": 1000,
		"table_header": "| Library | Speed (secs) |\n"
		              + "| ------- | ------------ |\n",
	},
	{
		"id": "benchmark_memory",
		"name": "Memory Benchmark",
		"copts": ["-DBENCHMARK_MEMORY"],
		# load modified malloc to report memory usage
		"runtime_env": "LD_PRELOAD=bazel-out/host/bin/benchmark/libmemory_reporter.so",
		"tools": [
			":memory_reporter",
		],
		"repeat": 1,
		"table_header": "| Library | Memory (bytes) |\n"
		              + "| ------- | -------------- |\n",
	},
]

BENCHMARK_SCENARIOS = [
	{
		"id": "scenario1",
		"name": "Scenario 1 (Reddit)",
		"input": ":data/reddit_front.json",
	},
	{
		"id": "scenario2",
		"name": "Scenario 2 (OMDB)",
		"input": ":data/omdb_frozen.json",
	},
	{
		"id": "scenario3",
		"name": "Scenario 3 (StackOverflow)",
		"input": ":data/stackoverflow_users.json",
	},
]

BENCHMARK_LIBRARIES = [
	{
		"id": "nlohmann_json",
		"url": "https://github.com/nlohmann/json",
		"bazel_deps": [
		],
		"linkopts": [],
	},
	{
		"id": "quantumjson",
		"url": "https://github.com/mserdarsanli/QuantumJson",
		"bazel_deps": [
			":quantumjson_schemas",
		],
		"linkopts": [],
	},
	{
		"id": "jsoncpp",
		"url": "https://github.com/open-source-parsers/jsoncpp",
		"bazel_deps": [
		],
		"linkopts": ["-ljsoncpp"],
	},
	{
		"id": "rapidjson",
		"url": "https://github.com/miloyip/rapidjson",
		"bazel_deps": [
		],
		"linkopts": [],
	},
	{
		# TODO benchmark against a known version
		"id": "boost_pt",
		"url": "http://www.boost.org/doc/libs/release/libs/property_tree/",
		"bazel_deps": [
		],
		"linkopts": [],
	},
	{
		"id": "staticjson",
		"url": "https://github.com/netheril96/StaticJSON",
		"bazel_deps": [
		],
		"linkopts": ["-lstaticjson"],
	},
]

cc_json_schema_lib(
	name = "quantumjson_schemas",
	srcs = [
		"schema/RedditListing.json.hpp",
		"schema/OMDB.json.hpp",
		"schema/StackOverflow.json.hpp",
	],
)

[
	[
		# Binary to run for the benchmark
		cc_binary(
			name = "bin_%s_%s_%s" % (lib["id"], sc["id"], mode["id"]),
			srcs = [
				"%s/impl_%s.cpp" % (sc["id"], lib["id"]),
				"Benchmark.hpp",
				"Executor.cpp",
			],
			copts = mode["copts"],
			deps = lib["bazel_deps"],
			linkopts = lib["linkopts"],
		),

		# Rule to run and save the benchmark
		genrule(
			name = "run_%s_%s_%s" % (lib["id"], sc["id"], mode["id"]),
			tools = mode["tools"] + [
				":bin_%s_%s_%s" % (lib["id"], sc["id"], mode["id"]),
			],
			srcs = [
				sc["input"],
			],
			outs = [
				":result_%s_%s_%s.txt" % (lib["id"], sc["id"], mode["id"]),
			],
			cmd = """ echo "{scenario}\t{lib}\t{mode}\t$$({ENV} {bin} {repeat} < {input})" > {output}
			        """.format(
			    ENV = mode["runtime_env"],
			    lib = lib["id"],
			    mode = mode["id"],
			    scenario = sc["id"],
			    bin = "$(location :bin_%s_%s_%s)" % (lib["id"], sc["id"], mode["id"]),
			    repeat = str(mode["repeat"]),
			    input = "$(location %s)" % sc["input"],
			    output = "$(OUTS)",
			),
		),
	]
	for sc in BENCHMARK_SCENARIOS
	for lib in BENCHMARK_LIBRARIES
	for mode in BENCHMARK_MODES
]

genrule(
	name = "results",
	srcs = [
		"result_%s_%s_%s.txt" % (lib["id"], sc["id"], mode["id"])
		for sc in BENCHMARK_SCENARIOS
		for lib in BENCHMARK_LIBRARIES
		for mode in BENCHMARK_MODES
	],
	outs = [
		"results.tsv",
	],
	cmd = "cat $(SRCS) > $(OUTS)",
)
