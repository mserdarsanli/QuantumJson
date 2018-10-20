#! /usr/bin/env python3

# The MIT License (MIT)
#
# Copyright (c) 2018 Mustafa Serdar Sanli
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

import subprocess
import sys

NinjaContents = '''
cppflags = -std=c++17 -fPIC -O3 -I out -I . -I third_party
extra_cppflags =
linkflags =

rule compile
    depfile = $out.d
    command = g++ -MMD -MF $out.d $cppflags $extra_cppflags -c $in -o $out

rule link
    command = g++ $linkflags $in -o $out

build out/lib/HexDump.o: compile lib/HexDump.cpp
build out/lib/HexDump: link out/lib/HexDump.o

rule hexdump
    command = out/lib/HexDump < $in > $out

build out/lib/LibCommonDef.hpp: hexdump lib/Common.hpp | out/lib/HexDump

build out/src/Attributes.o: compile src/Attributes.cpp
build out/src/CodeGenerator.o: compile src/CodeGenerator.cpp | out/lib/LibCommonDef.hpp
build out/src/FieldParser.o: compile src/FieldParser.cpp
build out/src/Main.o: compile src/Main.cpp
build out/src/Parser.o: compile src/Parser.cpp
build out/src/Tokenizer.o: compile src/Tokenizer.cpp

build out/jc: link out/src/Attributes.o out/src/CodeGenerator.o out/src/FieldParser.o out/src/Main.o out/src/Parser.o out/src/Tokenizer.o

# Tests
build out/tests/CatchMain.o: compile tests/CatchMain.cpp

build out/src/TestAttributes.o    : compile src/TestAttributes.cpp
build out/src/TestFieldParser.o   : compile src/TestFieldParser.cpp
build out/src/TestTemplateArgs.o  : compile src/TestTemplateArgs.cpp
build out/src/TestUtil.o          : compile src/TestUtil.cpp

build out/src/TestAttributes   : link out/tests/CatchMain.o out/src/TestAttributes.o    out/src/Attributes.o out/src/CodeGenerator.o out/src/FieldParser.o out/src/Parser.o out/src/Tokenizer.o
build out/src/TestFieldParser  : link out/tests/CatchMain.o out/src/TestFieldParser.o   out/src/Attributes.o out/src/CodeGenerator.o out/src/FieldParser.o out/src/Parser.o out/src/Tokenizer.o
build out/src/TestTemplateArgs : link out/tests/CatchMain.o out/src/TestTemplateArgs.o  out/src/Attributes.o out/src/CodeGenerator.o out/src/FieldParser.o out/src/Parser.o out/src/Tokenizer.o
build out/src/TestUtil         : link out/tests/CatchMain.o out/src/TestUtil.o          out/src/Attributes.o out/src/CodeGenerator.o out/src/FieldParser.o out/src/Parser.o out/src/Tokenizer.o


rule jc
    command = out/jc --in $in --out $out

build out/tests/Schema1.gen.hpp: jc tests/Schema1.json.hpp | out/jc
build out/tests/Schema2.gen.hpp: jc tests/Schema2.json.hpp | out/jc
build out/tests/SchemaNS.gen.hpp: jc tests/SchemaNS.json.hpp | out/jc


build out/tests/Main.o: compile tests/Main.cpp
build out/tests/Main: link out/tests/CatchMain.o out/tests/Main.o

build out/tests/ReservedObjectSizes.o: compile tests/ReservedObjectSizes.cpp
build out/tests/ReservedObjectSizes: link out/tests/CatchMain.o out/tests/ReservedObjectSizes.o

build out/tests/MultipleSchemas.o: compile tests/MultipleSchemas.cpp
build out/tests/MultipleSchemas2.o: compile tests/MultipleSchemas2.cpp
build out/tests/MultipleSchemas: link out/tests/CatchMain.o out/tests/MultipleSchemas.o out/tests/MultipleSchemas2.o

build out/tests/SerializeObjectTests.o: compile tests/SerializeObjectTests.cpp
build out/tests/SerializeObjectTests: link out/tests/CatchMain.o out/tests/SerializeObjectTests.o

build out/tests/NamespaceTests.o: compile tests/NamespaceTests.cpp
build out/tests/NamespaceTests: link out/tests/CatchMain.o out/tests/NamespaceTests.o

build out/tests/LibTests.o: compile tests/LibTests.cpp
build out/tests/LibTests: link out/tests/CatchMain.o out/tests/LibTests.o

build out/tests/SerializeTests.o: compile tests/SerializeTests.cpp
build out/tests/SerializeTests: link out/tests/CatchMain.o out/tests/SerializeTests.o

build out/tests/ConformanceDouble.o: compile tests/ConformanceDouble.cpp
build out/tests/ConformanceDouble: link out/tests/CatchMain.o out/tests/ConformanceDouble.o

build out/tests/StringAllocations.o: compile tests/StringAllocations.cpp
build out/tests/StringAllocations: link out/tests/CatchMain.o out/tests/StringAllocations.o

build out/tests/ReservedSizes.o: compile tests/ReservedSizes.cpp
build out/tests/ReservedSizes: link out/tests/CatchMain.o out/tests/ReservedSizes.o

build out/tests/Utf8StressTest.o: compile tests/Utf8StressTest.cpp
build out/tests/Utf8StressTest: link out/tests/CatchMain.o out/tests/Utf8StressTest.o

build out/tests/JsonSkipConformanceChecker.o: compile tests/JsonSkipConformanceChecker.cpp
build out/tests/JsonSkipConformanceChecker: link out/tests/JsonSkipConformanceChecker.o

build out/benchmark/schema/RedditListing.gen.hpp: jc benchmark/schema/RedditListing.json.hpp | out/jc
build out/benchmark/schema/OMDB.gen.hpp: jc benchmark/schema/OMDB.json.hpp | out/jc
build out/benchmark/schema/StackOverflow.gen.hpp: jc benchmark/schema/StackOverflow.json.hpp | out/jc
build out/benchmark/schema/TFL.gen.hpp: jc benchmark/schema/TFL.json.hpp | out/jc


rule compile_so
    command = g++ -fPIC -shared -g -o $out $in -ldl

build out/benchmark/memory_reporter.so: compile_so benchmark/BenchmarkMemoryReporter.cpp

build out/benchmark/Executor.o: compile benchmark/Executor.cpp
build out/third_party/jsoncpp.o: compile third_party/jsoncpp.cpp
build out/third_party/staticjson/staticjson.o: compile third_party/staticjson/staticjson.cpp
'''

BENCHMARK_SCENARIOS = [
    {
        "id": "scenario1",
        "input": "data/reddit_front.json",
        "repeat_cnt": 5000,
    },
    {
        "id": "scenario2",
        "input": "data/omdb_frozen.json",
        "repeat_cnt": 300000,
    },
    {
        "id": "scenario3",
        "input": "data/stackoverflow_users.json",
        "repeat_cnt": 14000,
    },
    {
        "id": "scenario4",
        "input": "data/tfl_accident_stats_2015.json",
        "repeat_cnt": 15,
    },
]

BENCHMARK_LIBRARIES = [
    'nlohmann_json',
    'quantumjson',
    'jsoncpp',
    'rapidjson',
    'staticjson',
    'boost_pt',
]


def configure_main():

    with open( 'build.ninja', 'w' ) as ninja:
        ninja.write( NinjaContents )

        for sc in BENCHMARK_SCENARIOS:
            for lib in BENCHMARK_LIBRARIES:
                extra_objs = ''
                if lib == 'jsoncpp':
                    extra_objs = 'out/third_party/jsoncpp.o'
                elif lib == 'staticjson':
                    extra_objs = 'out/third_party/staticjson/staticjson.o'

                extra_deps = ''
                if lib == 'quantumjson':
                    extra_deps = ' | out/benchmark/schema/RedditListing.gen.hpp out/benchmark/schema/OMDB.gen.hpp out/benchmark/schema/StackOverflow.gen.hpp out/benchmark/schema/TFL.gen.hpp'
                # Check Correctness
                ninja.write( f"build out/benchmark/bin_{lib}_{sc['id']}_check.o: compile benchmark/{sc['id']}/impl_{lib}.cpp {extra_deps}\n"
                           + f"    extra_cppflags = -DBENCHMARK_CHECK_CORRECTNESS\n"
                           + f"build out/benchmark/bin_{lib}_{sc['id']}_check: link out/benchmark/bin_{lib}_{sc['id']}_check.o out/benchmark/Executor.o {extra_objs}\n" )
                # Speed benchmarks
                ninja.write( f"build out/benchmark/bin_{lib}_{sc['id']}_speed.o: compile benchmark/{sc['id']}/impl_{lib}.cpp {extra_deps}\n"
                           + f"    extra_cppflags = -DBENCHMARK_SPEED\n"
                           + f"build out/benchmark/bin_{lib}_{sc['id']}_speed: link out/benchmark/bin_{lib}_{sc['id']}_speed.o out/benchmark/Executor.o {extra_objs}\n" )

                # Memory benchmarks
                ninja.write( f"build out/benchmark/bin_{lib}_{sc['id']}_memory.o: compile benchmark/{sc['id']}/impl_{lib}.cpp {extra_deps}\n"
                           + f"    extra_cppflags = -DBENCHMARK_MEMORY\n"
                           + f"build out/benchmark/bin_{lib}_{sc['id']}_memory: link out/benchmark/bin_{lib}_{sc['id']}_memory.o out/benchmark/Executor.o {extra_objs}\n" )

        print( f'Build rules written to {ninja.name}' )

def run_tests_main():

    subprocess.run( 'out/src/TestAttributes' )
    subprocess.run( 'out/src/TestFieldParser' )
    subprocess.run( 'out/src/TestTemplateArgs' )
    subprocess.run( 'out/src/TestUtil' )

    subprocess.run( 'out/tests/Main' )
    subprocess.run( 'out/tests/ReservedObjectSizes' )
    subprocess.run( 'out/tests/MultipleSchemas' )
    subprocess.run( 'out/tests/SerializeObjectTests' )
    subprocess.run( 'out/tests/NamespaceTests' )

    subprocess.run( 'out/tests/LibTests' )
    subprocess.run( 'out/tests/SerializeTests' )
    subprocess.run( 'out/tests/ConformanceDouble' )
    subprocess.run( 'out/tests/StringAllocations' )
    subprocess.run( 'out/tests/ReservedSizes' )
    subprocess.run( 'out/tests/Utf8StressTest' )

    for i in range( 1, 4 ):
        subprocess.run( [ 'out/tests/JsonSkipConformanceChecker', '--pass', f'tests/json_checker_tests/pass{i}.json' ] )

    for i in range( 1, 34 ):
        subprocess.run( [ 'out/tests/JsonSkipConformanceChecker', '--fail', f'tests/json_checker_tests/fail{i}.json' ] )

def run_benchmarks_main():

    for lib in BENCHMARK_LIBRARIES:
        for sc in BENCHMARK_SCENARIOS:

            res = subprocess.run( f"out/benchmark/bin_{lib}_{sc['id']}_check 1 < benchmark/{sc['input']}", shell = True, check = True, stdout = subprocess.PIPE )
            print( 'Correctness', lib, sc['id'], str( res.stdout, 'utf-8' ) )

    for lib in BENCHMARK_LIBRARIES:
        for sc in BENCHMARK_SCENARIOS:

            res = subprocess.run( f"out/benchmark/bin_{lib}_{sc['id']}_speed {sc['repeat_cnt'] // 15} < benchmark/{sc['input']}", shell = True, check = True, stdout = subprocess.PIPE )
            print( 'Speed', lib, sc['id'], str( res.stdout, 'utf-8' ) )

    for lib in BENCHMARK_LIBRARIES:
        for sc in BENCHMARK_SCENARIOS:

            res = subprocess.run( f"LD_PRELOAD=./out/benchmark/memory_reporter.so out/benchmark/bin_{lib}_{sc['id']}_memory 1 < benchmark/{sc['input']}", shell = True, check = True, stdout = subprocess.PIPE )
            print( 'Memory', lib, sc['id'], str( res.stdout, 'utf-8' ) )


def die_with_usage():
    print( 'Usage: ./build.py <configure|run-tests|run-benchmarks>' )
    sys.exit( 1 )

if __name__ == "__main__":
    if len( sys.argv ) != 2:
        die_with_usage()

    if sys.argv[ 1 ] == 'configure':
        configure_main()
    elif sys.argv[ 1 ] == 'run-tests':
        run_tests_main()
    elif sys.argv[ 1 ] == 'run-benchmarks':
        run_benchmarks_main()
    else:
        die_with_usage()
