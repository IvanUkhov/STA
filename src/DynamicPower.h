#ifndef __DYNAMIC_POWER_H__

#include "System.h"

class DynamicPower
{
	const processor_vector_t &processors;
	const size_t processor_count;
	const double sampling_interval;
	const size_t step_count;

	array_t<task_type_t> types;

	public:

	DynamicPower(const Platform &platform, const Graph &graph,
		double deadline, double sampling_interval);

	void compute(const Schedule &schedule, matrix_t &_dynamic_power) const;
};

#endif
