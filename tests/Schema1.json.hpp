struct Item
{
	string name;
};

struct Items
{
	Item i1;
	Item i2;
};

struct Listing
{
	string kind;

	Item item;

	string before;
	string after;

	vector<string> arrayField [[ json_field_name("array-field") ]];
};

struct FieldNameFSMTester
{
	string abcdef;
	string aaaaaa;
	string bbcdef;
	string abcdqq;
};

struct AttributeTester
{
	string attr1;
	string attr2 [[ json_field_name("attr-2") ]];
};

struct SkipNullTester
{
	string attr_accepting_null [[ on_null("skip") ]];
	string attr_regular;
};

// =================
// This is a comment
// -----------------
struct AAAAAA
{
	/**
	 * This is another comment
	 */
	vector< string /* vector of strings */ > field; // is equivalent to list of strings
};
