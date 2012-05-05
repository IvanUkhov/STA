#include <iostream>
#include <sstream>

#include "File.h"
#include "CommandLine.h"
#include "configuration.h"

class MyCommandLine: public CommandLine
{
	public:

	std::string system_config;

	MyCommandLine() : CommandLine() {}

	void usage() const
	{
		std::cout
			<< "Usage: deploy [-<argument name> <argument value>]" << std::endl
			<< std::endl
			<< "Required arguments:" << std::endl
			<< "  - system (s) -- a system configuration file." << std::endl;
	}

	protected:

	void verify() const
	{
		if (system_config.empty())
			throw std::runtime_error("The system configuration file is not specified.");
		else if (!File::exist(system_config))
			throw std::runtime_error("The system configuration file does not exist.");
	}

	void process(const std::string &name, const std::string &value)
	{
		if (name == "s" || name == "system") system_config = value;
	}
};

int main(int argc, char **argv)
{
	MyCommandLine arguments;

	try {
		arguments.parse(argc, (const char **)argv);

		parameters_t parameters(arguments.system_config);
	}
	catch (std::exception &e) {
		std::cerr << e.what() << std::endl;
		arguments.usage();
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
