#ifndef __PLATFORM_H__
#define __PLATFORM_H__

#include "System.h"

class Platform
{
	processor_vector_t _processors;
	size_t _processor_count;

	public:

	Platform(const matrix_t &dynamic_power,
		const matrix_t &execution_time);
	~Platform();

	inline size_t size() const
	{
		return _processor_count;
	}

	inline const Processor *operator[] (processor_id_t id) const
	{
		return _processors[id];
	}

	inline const processor_vector_t &processors() const
	{
		return _processors;
	}
};

std::ostream &operator<< (std::ostream &, const Platform &);
std::ostream &operator<< (std::ostream &, const Platform *);

#endif
