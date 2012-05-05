#ifndef __POOL_H__
#define __POOL_H__

#include "System.h"

class Pool: public std::list<task_id_t>
{
	template<class PT>
	friend class ListScheduler;

	protected:

	const size_t processor_count;
	const size_t task_count;

	vector_t processor_time;
	vector_t task_time;

	bit_string_t processed;
	bit_string_t scheduled;

	const layout_t &layout;
	const priority_t &priority;

	public:

	Pool(const processor_vector_t &_processors, const task_vector_t &_tasks,
		const layout_t &_layout, const priority_t &_priority, void *_data = NULL) :

		processor_count(_processors.size()), task_count(_tasks.size()),
		processor_time(processor_count, 0), task_time(task_count, 0),
		processed(task_count, false), scheduled(task_count, false),
		layout(_layout), priority(_priority)
	{
	}

	virtual void push(task_id_t id) = 0;
	virtual void pull(processor_id_t &pid, task_id_t &id) = 0;
};

class EarliestProcessorPool: public Pool
{
	public:

	EarliestProcessorPool(const processor_vector_t &_processors, const task_vector_t &_tasks,
		const layout_t &_layout, const priority_t &_priority, void *_data = NULL) :

		Pool(_processors, _tasks, _layout, _priority, _data) {}

	virtual inline void push(task_id_t id)
	{
		iterator it;
		rank_t new_priority = priority[id];

		for (it = begin(); it != end(); it++)
			if (new_priority < priority[*it]) break;

		insert(it, id);
	}

	virtual void pull(processor_id_t &pid, task_id_t &id)
	{
		processor_id_t earliest = 0;

		for (size_t i = 1; i < processor_count; i++)
			if (processor_time[earliest] > processor_time[i])
				earliest = i;

		id = front();
		pid = earliest;

		pop_front();
	}
};

#endif
