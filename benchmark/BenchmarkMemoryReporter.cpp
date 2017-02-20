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

// See https://stackoverflow.com/a/6083624

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include <dlfcn.h>

#include <stdio.h>
#include <stdlib.h>

#include <cstddef>

static void* (*real_calloc)(size_t nmemb, size_t size);
static void* (*real_malloc)(size_t size);
static void  (*real_free)(void *ptr);
static void* (*real_realloc)(void *ptr, size_t size);

static size_t current_memory_used = 0;
static size_t max_memory_used = 0;
static int alignment = 0;

static void update_memory_used(int diff)
{
	current_memory_used += diff;
	if (current_memory_used > max_memory_used)
	{
		max_memory_used = current_memory_used;
	}
}

static int64_t get_size(char *buf)
{
	return reinterpret_cast<int64_t*>(buf)[0];
}
static void set_size(char *buf, int64_t size)
{
	reinterpret_cast<int64_t*>(buf)[0] = size;
}

// Should be called before any use of malloc etc.
void memory_benchmark_init()
{
	alignment = alignof(std::max_align_t);

	real_malloc  = reinterpret_cast<void* (*)(size_t)>         (dlsym(RTLD_NEXT, "malloc"));
	real_realloc = reinterpret_cast<void* (*)(void*, size_t)>  (dlsym(RTLD_NEXT, "realloc"));
	real_calloc  = reinterpret_cast<void* (*)(size_t, size_t)> (dlsym(RTLD_NEXT, "calloc"));
	real_free    = reinterpret_cast<void  (*)(void*)>          (dlsym(RTLD_NEXT, "free"));

	if (!real_malloc || !real_realloc || !real_calloc || !real_free)
	{
		fprintf(stderr, "Error in `dlsym`: %s\n", dlerror());
		exit(1);
	}
}

void* malloc(size_t size)
{
	if (!real_malloc)
	{
		// Malloc gets called even before first line in main function.
		// Therefore this is the easiest way to initialize stuff.
		memory_benchmark_init();
	}
	update_memory_used(size);

	char *buf = reinterpret_cast< char* >( real_malloc(size + alignment) );
	set_size(buf, size);
	return reinterpret_cast< void* >( buf + alignment );
}

void free(void *ptr)
{
	if (ptr == nullptr)
	{
		return;
	}

	char *buf = reinterpret_cast< char* >(ptr) - alignment;
	update_memory_used( -get_size(buf) );
	return real_free( reinterpret_cast< void* >( buf ) );
}

void* calloc(size_t nmemb, size_t size)
{
	if (nmemb == 999999)
	{
		switch (size)
		{
			case 1000001:
				return reinterpret_cast<void*>( &current_memory_used );
			case 1000002:
				return reinterpret_cast<void*>( &max_memory_used );
			case 1000003:
				max_memory_used = current_memory_used;
				return nullptr;
			default:
				;
		}
	}

	fprintf(stderr, "Unimplemented: calloc\n");
	exit(1);
}

void* realloc(void *ptr, size_t new_size)
{
	if (ptr == nullptr)
	{
		return malloc( new_size );
	}

	char *orig_buf = reinterpret_cast< char* >(ptr) - alignment;
	int64_t old_size = get_size( orig_buf );

	char *new_buf = reinterpret_cast< char* >( real_realloc( orig_buf, new_size + alignment ) );
	set_size( new_buf, new_size );

	update_memory_used( new_size - old_size );

	return reinterpret_cast< void* >( new_buf + alignment );
}
