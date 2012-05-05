#ifndef __PRIORITY_H__
#define __PRIORITY_H__

#include "System.h"

class Priority
{
	public:

	static priority_t mobile(const Platform &platform,
		const Graph &graph, const mapping_t &mapping = mapping_t());
};

#endif
