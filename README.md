# QuantumJson (In Beta)

QuantumJson is an attempt to create the fastest JSON parser for C++.

It is a parser generator, using a `schema` file. The result is type-safe
and fast json parser for data of known schemas.

## Schema Definition

Schema file uses a similar syntax to C++, and `.json.hpp` extension is used.

The file must be processed by `jc` tool to produce `.gen.hpp` file, which is
a header only parser.

Following is a valid schema definition for StackOverflow API

`StackOverflow.json.hpp`:

```cpp
struct BadgeCounts
{
	int bronze;
	int silver;
	int gold;
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
	string user_type;
	int user_id;
	int accept_rate;
	string location;
	string website_url;
	string link;
	string profile_image;
	string display_name;
};

struct UsersResult
{
	vector<User> items;
	bool has_more;
	int quota_max;
	int quota_remaining;
};
```

And it can be used in your C++ program as follows:

`main.cpp`:

```cpp
#include "StackOverflow.gen.hpp"

...
UsersResult m;
m.MergeFromJson(input);
...
```


## Requirements

Currently g++-6 needs to be installed for benchmarks (required by nlohmann/json).

Bazel is used as build system.

TODO extend this section.

## Native Types

Native types that are already defined are:

 * bool
 * double
 * int
 * string
 * vector<Smth> ( JSON list )

# License

The MIT License. See COPYING file.
