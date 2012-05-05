#include <iostream>
#include <sstream>

#include "File.h"
#include "HotSpot.h"
#include "CommandLine.h"
#include "configuration.h"

class MyCommandLine: public CommandLine
{
	public:

	std::string floorplan;
	std::string power_profile;
	std::string hotspot_config;
	std::string variation_config;
	std::stringstream param_stream;

	MyCommandLine() : CommandLine() {}

	void usage() const
	{
		std::cout
			<< "Usage: analyze [-<argument name> <argument value>]" << std::endl
			<< std::endl
			<< "Required arguments:" << std::endl
			<< "  - floorplan (f)  - a floorplan of the die," << std::endl
			<< "  - power (p)      - a power profile," << std::endl
			<< "  - hotspot (h)    - a configuration file of HotSpot," << std::endl
			<< "  - variation (v)  - a configuration file of variations." << std::endl;
	}

	protected:

	void verify() const
	{
		if (floorplan.empty())
			throw std::runtime_error("The floorplan is not specified.");
		else if (!File::exist(floorplan))
			throw std::runtime_error("The floorplan does not exist.");

		if (power_profile.empty())
			throw std::runtime_error("The power profile is not specified.");
		else if (!File::exist(power_profile))
			throw std::runtime_error("The power profile does not exist.");

		if (hotspot_config.empty())
			throw std::runtime_error("The configuration file of HotSpot is not specified.");
		else if (!File::exist(hotspot_config))
			throw std::runtime_error("The configuration file of HotSpot does not exist.");

		if (variation_config.empty())
			throw std::runtime_error("The configuration of variations is not specified.");
		else if (!File::exist(variation_config))
			throw std::runtime_error("The configuration of variations does not exist.");
	}

	void process(const std::string &name, const std::string &value)
	{
		if (name == "f" || name == "floorplan") floorplan = value;
		else if (name == "p" || name == "power") power_profile = value;
		else if (name == "h" || name == "hotspot") hotspot_config = value;
		else if (name == "v" || name == "variation") variation_config = value;
		else param_stream << name << " " << value << std::endl;
	}
};

int main(int argc, char **argv)
{
	MyCommandLine arguments;

	try {
		arguments.parse(argc, (const char **)argv);

		HotSpot hotspot(arguments.floorplan, arguments.hotspot_config);

		parameters_t variations(arguments.variation_config);
	}
	catch (std::exception &e) {
		std::cerr << e.what() << std::endl << std::endl;
		arguments.usage();
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
