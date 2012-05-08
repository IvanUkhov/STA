#ifndef __LIST_SCHEDULER_H__
#define __LIST_SCHEDULER_H__

#include "System.h"
#include "Task.h"
#include "Processor.h"
#include "Schedule.h"
#include "Pool.h"

template<class PT>
class ListScheduler
{
	const processor_vector_t &_processors;
	const size_t _processor_count;

	const task_vector_t &_tasks;
	const size_t _task_count;

	public:

	ListScheduler(const Platform &platform, const Graph &graph) :
		_processors(platform.processors()), _processor_count(platform.size()),
		_tasks(graph.tasks()), _task_count(graph.size()) {}

	Schedule process(const layout_t &layout, const priority_t &priority,
		void *data = NULL) const;

	protected:

	inline bool ready(const Task *task, const bit_string_t &scheduled) const
	{
		size_t parent_count = task->parents.size();

		for (size_t i = 0; i < parent_count; i++)
			if (!scheduled[task->parents[i]->id]) return false;

		return true;
	}
};

typedef ListScheduler<EarliestProcessorPool> EarliestProcessorListScheduler;

#include "ListScheduler.hpp"

#endif
