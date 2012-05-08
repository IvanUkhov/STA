#ifndef __POOL_H__
#define __POOL_H__

#include "System.h"

class Pool: public std::list<task_id_t>
{
	template<class PT>
	friend class ListScheduler;

	protected:

	const size_t _processor_count;
	const size_t _task_count;

	vector_t _processor_time;
	vector_t _task_time;

	bit_string_t _processed;
	bit_string_t _scheduled;

	const layout_t &_layout;
	const priority_t &_priority;

	public:

	Pool(const processor_vector_t &processors, const task_vector_t &tasks,
		const layout_t &layout, const priority_t &priority, void *data = NULL) :

		_processor_count(processors.size()), _task_count(tasks.size()),
		_processor_time(_processor_count, 0), _task_time(_task_count, 0),
		_processed(_task_count, false), _scheduled(_task_count, false),
		_layout(layout), _priority(priority)
	{
	}

	virtual void push(task_id_t id) = 0;
	virtual void pull(processor_id_t &pid, task_id_t &id) = 0;
};

class EarliestProcessorPool: public Pool
{
	public:

	EarliestProcessorPool(const processor_vector_t &processors,
		const task_vector_t &tasks, const layout_t &layout,
		const priority_t &priority, void *data = NULL) :

		Pool(processors, tasks, layout, priority, data) {}

	virtual inline void push(task_id_t id)
	{
		iterator it;
		rank_t new_priority = _priority[id];

		for (it = begin(); it != end(); it++)
			if (new_priority < _priority[*it]) break;

		insert(it, id);
	}

	virtual void pull(processor_id_t &pid, task_id_t &id)
	{
		processor_id_t earliest = 0;

		for (size_t i = 1; i < _processor_count; i++)
			if (_processor_time[earliest] > _processor_time[i])
				earliest = i;

		id = front();
		pid = earliest;

		pop_front();
	}
};

#endif
