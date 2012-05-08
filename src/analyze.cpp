#include <iostream>
#include <sstream>
#include <iomanip>

#include "File.h"
#include "CommandLine.h"
#include "configuration.h"
#include "HotSpot.h"
#include "TemperatureAnalysis.h"

class MyCommandLine: public CommandLine
{
	public:

	std::string floorplan;
	std::string power_profile;
	std::string hotspot_config;
	std::string variation_config;
	bool pretend;
	std::stringstream param_stream;

	MyCommandLine() : CommandLine(), pretend(false) {}

	void usage() const
	{
		std::cout
			<< "Usage: analyze <arguments>" << std::endl
			<< std::endl
			<< "Required arguments:" << std::endl
			<< "  -floorplan, -f <value>  -- the floorplan of the die," << std::endl
			<< "  -power,     -p <value>  -- the power profile," << std::endl
			<< "  -hotspot,   -h <value>  -- the configuration file of HotSpot," << std::endl
			<< "  -variation, -v <value>  -- the configuration file of variations." << std::endl
			<< std::endl
			<< "Optional arguments:" << std::endl
			<< "  -pretend,   -0          -- display diagnostic information without computing the temperature profile." << std::endl;
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
		else if (name == "0" || name == "pretend") pretend = true;
		else param_stream << name << " " << value << std::endl;
	}
};

int main(int argc, char **argv)
{
	MyCommandLine arguments;

	try {
		arguments.parse(argc, (const char **)argv);

		HotSpot hotspot(arguments.floorplan, arguments.hotspot_config);

		if (arguments.pretend)
			std::cout << "Loaded floorplan:" << std::endl
				<< "  Cores: " << hotspot.processor_count() << std::endl
				<< "  Nodes: " << hotspot.node_count() << std::endl;

		matrix_t dynamic_power;

		File::load(dynamic_power, arguments.power_profile);

		if (arguments.pretend)
			std::cout << "Loaded power profile:" << std::endl
				<< "  Cores: " << dynamic_power.cols() << std::endl
				<< "  Steps: " << dynamic_power.rows() << std::endl;

		if (!arguments.pretend) {
			matrix_t temperature;

			TransientTemperatureAnalysis analysis(hotspot);
			analysis.perform(dynamic_power, temperature);

			std::cout << std::fixed << std::setprecision(2);
			File::store(temperature, std::cout);
		}
	}
	catch (std::exception &e) {
		std::cerr << e.what() << std::endl << std::endl;
		arguments.usage();
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
