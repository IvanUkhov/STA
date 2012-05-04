#include <iostream>
#include <sstream>

#include "File.h"
#include "CommandLine.h"

using namespace std;

class AnalyzeCommandLine: public CommandLine
{
	public:

	string floorplan;
	string hotspot;
	stringstream param_stream;

	AnalyzeCommandLine() : CommandLine() {}

	void usage() const
	{
		cout
			<< "Usage: analyze [-<param name> <param value>]" << endl
			<< endl
			<< "  Available parameters:" << endl
			<< "  * f, floorplan   - a floorplan for the PEs" << endl
			<< "  * h, hotspot     - the Hotspot configuration" << endl
			<< endl
			<< "  (* required parameters)" << endl;
	}

	protected:

	void verify() const
	{
		if (floorplan.empty())
			throw runtime_error("The floorplan configuration file is not specified.");

		if (!File::exist(floorplan))
			throw runtime_error("The floorplan configuration file does not exist.");

		if (hotspot.empty())
			throw runtime_error("The configuration file of HotSpot is not specified.");

		if (!File::exist(hotspot))
			throw runtime_error("The configuration file of HotSpot does not exist.");
	}

	void process(const string &name, const string &value)
	{
		if (name == "f" || name == "floorplan") floorplan = value;
		else if (name == "h" || name == "hotspot") hotspot = value;
		else param_stream << name << " " << value << endl;
	}
};

int main(int argc, char **argv)
{
	AnalyzeCommandLine arguments;

	try {
		arguments.parse(argc, (const char **)argv);
	}
	catch (exception &e) {
		cerr << e.what() << endl;
		arguments.usage();
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
