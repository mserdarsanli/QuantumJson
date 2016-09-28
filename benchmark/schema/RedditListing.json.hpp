
struct RedditPostData
{
	int score;
	string url;
};

struct RedditPost
{
	RedditPostData data;
};

struct RedditListingData
{
	vector<RedditPost> children;
};

struct RedditListing
{
	RedditListingData data;
};
