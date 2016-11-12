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

#ifndef BENCHMARK_HPP
#define BENCHMARK_HPP

#include <iostream>

// This variable is used to prevent GCC from optimizing tracing calls.
// As tracing is done by benchmark_memory.cpp and it is loaded using `LD_PRELOAD`
// it is harder to define another function to report results. Therefore currently
// a call to calloc is used with magic values.
extern volatile void* __do_not_omit;

#if defined BENCHMARK_MEMORY && !defined BENCHMARK_SPEED && !defined BENCHMARK_CHECK_CORRECTNESS

	// Benchmarking Memory

	#define BENCHMARK_BEGIN \
		size_t __memory_used_before_parsing = -1; \
		size_t __max_memory_used_during_parsing = -1

	#define BENCHMARK_END

	// Report what has been allocated before parsing begins
	#define BENCHMARK_LOOP_BEGIN \
	{ \
		__do_not_omit = calloc(999999, 1000001); \
		__memory_used_before_parsing = *reinterpret_cast<volatile size_t*>(__do_not_omit); \
		__do_not_omit = calloc(999999, 1000003); \
		/* std::cerr << "memory used before parsing: " << __memory_used_before_parsing << "\n"; */ \
	}

	// Report memory usage
	#define BENCHMARK_LOOP_END \
	{ \
		__do_not_omit = calloc(999999, 1000002); \
		__max_memory_used_during_parsing = *reinterpret_cast<volatile size_t*>(__do_not_omit); \
		/* std::cerr << "memory used during parsing: " << __max_memory_used_during_parsing << "\n"; */ \
		std::cout << __max_memory_used_during_parsing - __memory_used_before_parsing << "\n"; \
	}

#elif !defined BENCHMARK_MEMORY && defined BENCHMARK_SPEED && !defined BENCHMARK_CHECK_CORRECTNESS

	#include <time.h>
	#include <stdio.h>

	// Benchmarking Speed

	#define BENCHMARK_BEGIN \
	    clock_t start_time = clock();

	#define BENCHMARK_END \
	    clock_t end_time = clock(); \
	    printf("%.6f\n", double(end_time - start_time) / CLOCKS_PER_SEC );

	#define BENCHMARK_LOOP_BEGIN
	#define BENCHMARK_LOOP_END

#elif !defined BENCHMARK_MEMORY && !defined BENCHMARK_SPEED && defined BENCHMARK_CHECK_CORRECTNESS

	#define BENCHMARK_BEGIN \
		bool _check_done = false;

	#define BENCHMARK_END \
		if (!_check_done) \
		{ \
			std::cerr << "No correctness check was performed\n"; \
			std::exit(1); \
		}

	#define BENCHMARK_LOOP_BEGIN
	#define BENCHMARK_LOOP_END

	#define CHECK(expr) \
	{ \
		_check_done = true; \
		if (!(expr)) \
		{ \
			std::cerr << "Check failed: [" << #expr << "]\n"; \
			std::exit(1); \
		} \
	}

#else

	#error "Specify one of BENCHMARK_MEMORY | BENCHMARK_SPEED | BENCHMARK_CHECK_CORRECTNESS"

#endif

#endif // BENCHMARK_HPP
