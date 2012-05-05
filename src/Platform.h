#ifndef __PLATFORM_H__
#define __PLATFORM_H__

#include "System.h"

class Platform
{
	template<class PT> friend class ListScheduler;

	processor_vector_t processors;
	size_t processor_count;

	public:

	Platform(const matrix_t &dynamic_power,
		const matrix_t &execution_time);
	~Platform();

	inline size_t get_size() const
	{
		return processor_count;
	}

	inline const Processor *operator[] (processor_id_t id) const
	{
		return processors[id];
	}

	inline const processor_vector_t &get_processors() const
	{
		return processors;
	}
};

std::ostream &operator<< (std::ostream &, const Platform &);
std::ostream &operator<< (std::ostream &, const Platform *);

#endif
