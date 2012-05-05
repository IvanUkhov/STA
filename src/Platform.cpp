#include <iomanip>

#include "Platform.h"
#include "Processor.h"

Platform::Platform(const matrix_t &dynamic_power,
	const matrix_t &execution_time) : processor_count(0)
{
	size_t processor_count = dynamic_power.cols();
	size_t type_count = dynamic_power.rows();

	if (processor_count == 0)
		throw std::runtime_error("There are no processors.");

	if (processor_count != execution_time.cols() ||
		type_count != execution_time.rows())

		throw std::runtime_error("The platform data is not consistent.");

	size_t i, j;

	for (i = 0; i < processor_count; i++) {
		Processor *processor = new Processor(processor_count, type_count);
		processors.push_back(processor);
		this->processor_count++;

		for (j = 0; j < type_count; j++)
			processor->set_type(j, dynamic_power[i][j], execution_time[i][j]);
	}
}

Platform::~Platform()
{
	for (size_t i = 0; i < processor_count; i++)
		delete processors[i];
}

std::ostream &operator<< (std::ostream &o, const Platform &platform)
{
	const processor_vector_t &processors = platform.get_processors();
	size_t size = processors.size();

	o	<< "Platform:" << std::endl
		<< "  Number of processors: " << size << std::endl
		<< "  " << std::setw(4) << "id" << " ( " << std::setw(8) << "types" << " )" << std::endl;

	for (size_t i = 0; i < size; i++)
		o << "  " << processors[i];

	return o;
}

std::ostream &operator<< (std::ostream &o, const Platform *platform)
{
	return o << &platform;
}
