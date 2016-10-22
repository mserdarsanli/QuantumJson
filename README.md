# QuantumJson (In Beta)

QuantumJson is an attempt to create the fastest JSON parser for C++.

It is a parser generator, using a `schema` file. The result is type-safe
and fast json parser for data of known schemas.

View [Installation](#installation) section for instructions.

## Rationale

When talking to a JSON API, the JSON values are often in a well defined format.
Parsing them using a generic JSON library can be an overkill for memory and
CPU time, also it may make the code more verbose for type checking.

A reason generic JSON parsers are slower is they need to store object field
names and value types.

All generic JSON parsers have an API similar to:

```
JsonValue v;
v.parse("{...input json...}")

// Runtime key lookup
// Returns a generic JsonValue or null or throws an exception.
field = v.get_field("status")

// To check if the input is the expected type
field.is_string()

// Returns value, or throws an exception.
field.get_value< std::string >()
```

QuantumJson instead generates a header only library, having structs and parser
code. The parser is able to directly parse into struct elements while
validating the types.

With QuantumJson, the code would look like:

```
// A struct type defined in the schema file.
DataType1 v;

// Parses a valid json, ignores unknown fields, throws if defined fields
// have different types.
v.parse("{...input json...}")

// Accessing a field is same as accessing a struct member.
v.status
```

Therefore the code would not be cluttered with type checks, and performance
will be better.

### Drawbacks

Such rationale allows QuantumJson to be faster. But for that, it *can not* do
following:

* Accessing fields by name.
* Fields having multiple value types.
* Lists of generic values (JSON lists are implemented via `std::vector<T>` and
value types must be defined too.
* ...

So if your task requires processing JSON values of unknown types, QuantumJson
is not the right tool.

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

## Native Types

Native types that are already defined are:

 * bool
 * double
 * int
 * string
 * vector<Smth> ( JSON list )

## Installation

Easiest way to install is to checkout a release commit and use provided Makefile.

```
make
sudo make install
```

### Usage

To generate parser for the schema file, use following command.
Generated parser is header-only.

```
jc --in Foo.json.hpp --out Foo.gen.hpp
```

### Requirements

* A GNU/Linux environment (or similar)
* `g++` or `clang++` (any version supporting --std=c++1y flag)
* `gengetopt`
* `xxd`

For development of QuantumJson only:

* `bazel` is used as the build system.
* `g++-6` needs to be installed for benchmarks (required by nlohmann/json).


# License

The MIT License. See COPYING file.
