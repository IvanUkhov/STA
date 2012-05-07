#include <iostream>
#include <sstream>
#include <stdexcept>

#include "File.h"
#include "CommandLine.h"
#include "configuration.h"
#include "Platform.h"
#include "Graph.h"
#include "Priority.h"
#include "ListScheduler.h"
#include "DynamicPower.h"

class MyCommandLine: public CommandLine
{
	public:

	std::string system_config;
	std::string power_profile;

	bool relative_deadline;
	double deadline;
	double sampling_interval;

	MyCommandLine() : CommandLine(),
		relative_deadline(true), deadline(105), sampling_interval(1e-3) {}

	void usage() const
	{
		std::cout
			<< "Usage: deploy [-<argument name> <argument value>]" << std::endl
			<< std::endl
			<< "Required arguments:" << std::endl
			<< "  - system (s)    -- a system configuration file." << std::endl
			<< std::endl
			<< "Optional arguments:" << std::endl
			<< "  - power (p)     -- an output file to dump the power profile," << std::endl
			<< "  - deadline (d)  -- a deadline relative to the schedule or absolute (105\% by default)," << std::endl
			<< "  - sample (i)    -- a sampling interval (1e-3 by default)," << std::endl;
	}

	protected:

	void verify() const
	{
		if (system_config.empty())
			throw std::runtime_error("The system configuration file is not specified.");
		else if (!File::exist(system_config))
			throw std::runtime_error("The system configuration file does not exist.");

		if (deadline <= 0)
			throw std::runtime_error("The deadline is invalid.");

		if (sampling_interval <= 0)
			throw std::runtime_error("The sampling interval is invalid.");
	}

	void process(const std::string &name, const std::string &value)
	{
		if (name == "s" || name == "system") system_config = value;
		else if (name == "p" || name == "power") power_profile = value;
		else if (name == "d" || name == "deadline") {
			size_t size = value.length();

			if (size == 0)
				throw std::runtime_error("The deadline is empty.");

			std::istringstream(value) >> deadline;

			relative_deadline = value[size - 1] == '%';
		}
		else if (name == "i" || name == "sample") {
			std::istringstream(value) >> sampling_interval;
		}
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

		Platform platform(dynamic_power, execution_time);
		Graph graph(types, arcs);

		std::cout << platform << std::endl;
		std::cout << graph << std::endl;

		priority_t priority = Priority::mobile(platform, graph);

		EarliestProcessorListScheduler scheduler(platform, graph);
		Schedule schedule = scheduler.process(layout_t(), priority);

		std::cout << schedule;

		if (!arguments.power_profile.empty()) {
			double deadline = arguments.deadline;

			if (arguments.relative_deadline)
				deadline = (deadline / 100.0) * schedule.get_duration();

			DynamicPower heater(platform, graph, deadline, arguments.sampling_interval);

			matrix_t dynamic_power;
			heater.compute(schedule, dynamic_power);

			File::dump(dynamic_power, arguments.power_profile);
		}
	}
	catch (std::exception &e) {
		std::cerr << e.what() << std::endl << std::endl;
		arguments.usage();
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
