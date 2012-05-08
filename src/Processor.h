#ifndef __PROCESSOR_H__
#define __PROCESSOR_H__

#include "System.h"

class Processor
{
	const processor_id_t _id;
	const size_t _type_count;

	vector_t _dynamic_power;
	vector_t _execution_time;

	public:

	Processor(processor_id_t id, size_t type_count) :
		_id(id), _type_count(type_count),
		_dynamic_power(_type_count), _execution_time(_type_count) {}

	void set_type(task_type_t type, double power, double time)
	{
		if (type >= _type_count)
			throw std::runtime_error("The type is out of bounds.");

		_dynamic_power[type] = power;
		_execution_time[type] = time;
	}

	inline processor_id_t id() const
	{
		return _id;
	}

	inline size_t type_count() const
	{
		return _type_count;
	}

	inline double dynamic_power(task_type_t type) const
	{
#ifndef SHALLOW_CHECK
		if (type >= _type_count)
			throw std::runtime_error("The type is out of bounds.");
#endif

		return _dynamic_power[type];
	}

	inline double execution_time(unsigned int type) const
	{
#ifndef SHALLOW_CHECK
		if (type >= _type_count)
			throw std::runtime_error("The type is out of bounds.");
#endif

		return _execution_time[type];
	}
};

std::ostream &operator<< (std::ostream &o, const Processor *processor);

#endif
