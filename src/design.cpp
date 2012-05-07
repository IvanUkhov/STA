#include <iostream>
#include <sstream>
#include <stdexcept>
#include <cmath>

#include "CommandLine.h"

class MyCommandLine: public CommandLine
{
	public:

	size_t count;
	double area;
	std::string label;

	MyCommandLine() : CommandLine(), count(0), area(4e-6), label("core") {}

	void usage() const
	{
		std::cout
			<< "Usage: floorplan <arguments>" << std::endl
			<< std::endl
			<< "Required arguments:" << std::endl
			<< "  -count, -c <value>       -- the number of processors on the die." << std::endl
			<< std::endl
			<< "Optional arguments:" << std::endl
			<< "  -area,  -a <value=4e-6>  -- the area of a single processor," << std::endl
			<< "  -label, -l <value=core>  -- the label of the processors." << std::endl;
	}

	protected:

	void verify() const
	{
		if (count == 0)
			throw std::runtime_error("The number of processors is zero.");
		else if (area <= 0)
			throw std::runtime_error("The area is invalid.");
		else if (label.empty())
			throw std::runtime_error("The label is invalid.");
	}

	void process(const std::string &name, const std::string &value)
	{
		if (name == "c" || name == "count")
			std::istringstream(value) >> count;
		else if (name == "a" || name == "area")
			std::istringstream(value) >> area;
		else if (name == "l" || name == "label")
			label = value;
	}
};

int main(int argc, char *argv[])
{
	MyCommandLine arguments;

	try {
		arguments.parse(argc, (const char **)argv);

		double processor_width = sqrt(arguments.area);
		size_t cols = ceil(sqrt(arguments.count));

		double x, y;

		for (size_t i = 0; i < arguments.count; i++) {
			x = (i % cols) * processor_width;
			y = (int)(i / cols) * processor_width;

			std::cout
				<< arguments.label << i + 1 << '\t'
				<< processor_width << '\t'
				<< processor_width << '\t'
				<< x << '\t'
				<< y << std::endl;
		}
	}
	catch (std::exception &e) {
		std::cerr << e.what() << std::endl << std::endl;
		arguments.usage();
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
