#include <iostream>
#include <sstream>
#include <stdexcept>

#include "File.h"
#include "CommandLine.h"
#include "Configuration.h"
#include "Platform.h"
#include "Graph.h"
#include "Priority.h"
#include "ListScheduler.h"
#include "DynamicPower.h"

class MyCommandLine: public CommandLine
{
	public:

	std::string system_config;
	bool relative_deadline;
	double deadline;
	double sampling_interval;
	std::string label;
	bool pretend;

	MyCommandLine() : CommandLine(),
		relative_deadline(true), deadline(105),
		sampling_interval(1e-3), label("core"),
		pretend(false) {}

	void usage() const
	{
		std::cout
			<< "Usage: deploy <arguments>" << std::endl
			<< std::endl
			<< "Required arguments:" << std::endl
			<< "  -system,   -s <value>       -- the configuration file of the system." << std::endl
			<< std::endl
			<< "Optional arguments:" << std::endl
			<< "  -deadline, -d <value=105%>  -- the deadline either relative to the schedule or absolute," << std::endl
			<< "  -sample,   -i <value=1e-3>  -- the sampling interval of the power profile," << std::endl
			<< "  -label,    -l <value=core>  -- the name prefix of the processors," << std::endl
			<< "  -pretend,  -0               -- display diagnostic information instead of computing the power profile." << std::endl;
	}

	protected:

	void process(const std::string &name, const std::string &value)
	{
		if (name == "system" || name == "s") system_config = value;
		else if (name == "deadline" || name == "d") {
			size_t size = value.length();

			if (size == 0) {
				deadline = 0;
				return;
			}

			std::istringstream(value) >> deadline;

			relative_deadline = value[size - 1] == '%';
		}
		else if (name == "sample" || name == "i")
			std::istringstream(value) >> sampling_interval;
		else if (name == "label" || name == "l")
			label = value;
		else if (name == "pretend" || name == "0")
			pretend = true;
	}

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
};

int main(int argc, char **argv)
{
	MyCommandLine arguments;

	try {
		arguments.parse(argc, (const char **)argv);

		Configuration config(arguments.system_config);

		vector_t types;
		matrix_t arcs, dynamic_power, execution_time;

		size_t size = config.size();

		for (size_t i = 0; i < size; i++) {
			const Parameter &param = config[i];

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

		if (arguments.pretend)
			std::cout << platform << std::endl
				<< graph << std::endl;

		priority_t priority = Priority::mobile(platform, graph);

		EarliestProcessorListScheduler scheduler(platform, graph);
		Schedule schedule = scheduler.process(layout_t(), priority);

		if (arguments.pretend)
			std::cout << schedule;

		if (!arguments.pretend) {
			double deadline = arguments.deadline;

			if (arguments.relative_deadline)
				deadline = (deadline / 100.0) * schedule.duration();

			DynamicPower heater(platform, graph, deadline, arguments.sampling_interval);

			matrix_t dynamic_power;
			heater.compute(schedule, dynamic_power);

			File::store(dynamic_power, std::cout, arguments.label);
		}
	}
	catch (std::exception &e) {
		std::cerr << e.what() << std::endl << std::endl;
		arguments.usage();
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
