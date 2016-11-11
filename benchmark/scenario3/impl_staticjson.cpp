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

struct BadgeCounts
{
	int bronze;
	int silver;
	int gold;

	void staticjson_init(staticjson::ObjectHandler* h)
	{
		h->add_property("bronze", &bronze);
		h->add_property("silver", &silver);
		h->add_property("gold", &gold);
	}
};

struct User
{
	BadgeCounts badge_counts;
	int account_id;
	bool is_employee;
	int last_modified_date;
	int last_access_date;
	int age;
	int reputation_change_year;
	int reputation_change_quarter;
	int reputation_change_month;
	int reputation_change_week;
	int reputation_change_day;
	int reputation;
	int creation_date;
	std::string user_type;
	int user_id;
	int accept_rate;
	std::string location;
	std::string website_url;
	std::string link;
	std::string profile_image;
	std::string display_name;

	void staticjson_init(staticjson::ObjectHandler* h)
	{
		h->add_property("badge_counts", &badge_counts);
		h->add_property("account_id", &account_id);
		h->add_property("is_employee", &is_employee);
		h->add_property("last_modified_date", &last_modified_date);
		h->add_property("last_access_date", &last_access_date);
		h->add_property("age", &age);
		h->add_property("reputation_change_year", &reputation_change_year);
		h->add_property("reputation_change_quarter", &reputation_change_quarter);
		h->add_property("reputation_change_month", &reputation_change_month);
		h->add_property("reputation_change_week", &reputation_change_week);
		h->add_property("reputation_change_day", &reputation_change_day);
		h->add_property("reputation", &reputation);
		h->add_property("creation_date", &creation_date);
		h->add_property("user_type", &user_type);
		h->add_property("user_id", &user_id);
		h->add_property("accept_rate", &accept_rate);
		h->add_property("location", &location);
		h->add_property("website_url", &website_url);
		h->add_property("link", &link);
		h->add_property("profile_image", &profile_image);
		h->add_property("display_name", &display_name);
	}
};

struct UsersResult
{
	std::vector<User> items;
	bool has_more;
	int quota_max;
	int quota_remaining;

	void staticjson_init(staticjson::ObjectHandler* h)
	{
		h->add_property("items", &items);
		h->add_property("has_more", &has_more);
		h->add_property("quota_max", &quota_max);
		h->add_property("quota_remaining", &quota_remaining);
	}
};

void Benchmark(int repeat, const std::string &input)
{
	BENCHMARK_BEGIN;

	for (int i = 0; i < repeat; ++i)
	{
		BENCHMARK_LOOP_BEGIN;

		UsersResult r;
		staticjson::from_json_string(input.c_str(), &r, nullptr);

		BENCHMARK_LOOP_END;
	}

	BENCHMARK_END;
}
