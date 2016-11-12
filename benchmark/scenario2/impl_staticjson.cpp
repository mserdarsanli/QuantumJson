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
#include <vector>

#include "benchmark/Benchmark.hpp"

#include <staticjson/staticjson.hpp>

struct Movie
{
	std::string Response;
	std::string Type;
	std::string imdbID;
	std::string imdbVotes;
	std::string Writer;
	std::string Director;
	std::string Genre;
	std::string Runtime;
	std::string Released;
	std::string Rated;
	std::string Year;
	std::string Title;
	std::string Actors;
	std::string Plot;
	std::string Language;
	std::string Country;
	std::string Awards;
	std::string Poster;
	std::string Metascore;
	std::string imdbRating;

	void staticjson_init(staticjson::ObjectHandler* h)
	{
		h->add_property("Response", &Response);
		h->add_property("Type", &Type);
		h->add_property("imdbID", &imdbID);
		h->add_property("imdbVotes", &imdbVotes);
		h->add_property("Writer", &Writer);
		h->add_property("Director", &Director);
		h->add_property("Genre", &Genre);
		h->add_property("Runtime", &Runtime);
		h->add_property("Released", &Released);
		h->add_property("Rated", &Rated);
		h->add_property("Year", &Year);
		h->add_property("Title", &Title);
		h->add_property("Actors", &Actors);
		h->add_property("Plot", &Plot);
		h->add_property("Language", &Language);
		h->add_property("Country", &Country);
		h->add_property("Awards", &Awards);
		h->add_property("Poster", &Poster);
		h->add_property("Metascore", &Metascore);
		h->add_property("imdbRating", &imdbRating);
	}
};

void Benchmark(int repeat, const std::string &input)
{
	BENCHMARK_BEGIN;

	for (int i = 0; i < repeat; ++i)
	{
		BENCHMARK_LOOP_BEGIN;

		Movie m;
		staticjson::from_json_string(input.c_str(), &m, nullptr);

		#ifdef BENCHMARK_CHECK_CORRECTNESS

		std::string title = m.Title;
		std::string awards = m.Awards;
		std::string poster = m.Poster;
		std::string writer = m.Writer;
		std::string imdbRating = m.imdbRating;

		CHECK(title == "Frozen");
		CHECK(awards == "Won 2 Oscars. Another 72 wins & 57 nominations.");
		CHECK(poster == "http://ia.media-imdb.com/images/M/MV5BMTQ1MjQwMTE5OF5BMl5BanBnXkFtZTgwNjk3MTcyMDE@._V1_SX300.jpg");
		CHECK(writer == "Jennifer Lee (screenplay), Hans Christian Andersen (story inspired by \"The Snow Queen\" by), Chris Buck (story by), Jennifer Lee (story by), Shane Morris (story by)");
		CHECK(imdbRating == "7.6");

		#endif

		BENCHMARK_LOOP_END;
	}

	BENCHMARK_END;
}
