#ifndef __PROCESSOR_H__
#define __PROCESSOR_H__

#include <vector>
#include <ostream>

#include "matrix.h"
#include "Task.h"

typedef size_t processor_id_t;

class Processor;

typedef std::vector<Processor *> processor_vector_t;

class Processor
{
	const processor_id_t id;
	const size_t type_count;

	vector_t dynamic_power;
	vector_t execution_time;

	public:

	Processor(processor_id_t _id, size_t _type_count) :
		id(_id), type_count(_type_count) {}

	void set_type(task_type_t type, double power, double time)
	{
		if (type >= type_count)
			throw std::runtime_error("The type is out of bounds.");

		dynamic_power[type] = power;
		execution_time[type] = time;
	}

	inline processor_id_t get_id() const
	{
		return id;
	}

	inline size_t get_size() const
	{
		return type_count;
	}

	inline double get_dynamic_power(task_type_t type) const
	{
#ifndef SHALLOW_CHECK
		if (type >= type_count)
			throw std::runtime_error("The type is out of bounds.");
#endif

		return dynamic_power[type];
	}

	inline double get_execution_time(unsigned int type) const
	{
#ifndef SHALLOW_CHECK
		if (type >= type_count)
			throw std::runtime_error("The type is out of bounds.");
#endif

		return execution_time[type];
	}
};

std::ostream &operator<< (std::ostream &o, const Processor *processor);

#endif
