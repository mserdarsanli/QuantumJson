
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

