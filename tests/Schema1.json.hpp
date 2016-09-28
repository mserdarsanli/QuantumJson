struct Item
{
	string name;
};

struct Listing
{
	string kind;

	Item item;

	string before;
	string after;

	vector<string> arrayField [[jsonFieldName("array-field")]];
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
