struct Vehicle
{
	string type;
	string data_type [[ json_field_name("$type") ]];
};

struct Casualty
{
	string ageBand;
	string mode;
	string severity;
	string cClass [[ json_field_name("class") ]];
	int age;
	string data_type [[ json_field_name("$type") ]];
};

struct Accident
{
	vector<Vehicle> vehicles;
	vector<Casualty> casualties;
	string data_type [[ json_field_name("$type") ]];

	double lat;
	double lon;
	string location;
	string date;
	string severity;
	string borough;
};
