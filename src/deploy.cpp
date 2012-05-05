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

		vector_t types;
		matrix_t arcs, dynamic_power, execution_time;

		size_t size = parameters.size();

		for (size_t i = 0; i < size; i++) {
			const parameter_t &param = parameters[i];

			if (param.name == "types") {
				param.to_vector(types);
			}
			else if (param.name == "arcs") {
				param.to_matrix(arcs);
			}
			else if (param.name == "dynamic_power") {
				param.to_matrix(dynamic_power);
			}
			else if (param.name == "execution_time") {
				param.to_matrix(execution_time);
			}
		}
	}
	catch (std::exception &e) {
		std::cerr << e.what() << std::endl;
		arguments.usage();
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
