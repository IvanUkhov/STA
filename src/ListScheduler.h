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
	const processor_vector_t &processors;
	const size_t processor_count;

	const task_vector_t &tasks;
	const size_t task_count;

	public:

	ListScheduler(const Platform &platform, const Graph &graph) :
		processors(platform.processors), processor_count(processors.size()),
		tasks(graph.tasks), task_count(tasks.size()) {}

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
