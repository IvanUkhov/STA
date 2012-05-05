#include "ListScheduler.h"

template<class PT>
Schedule ListScheduler<PT>::process(const layout_t &layout,
	const priority_t &priority, void *data) const
{
	PT pool(processors, tasks, layout, priority, data);

	bit_string_t &processed = pool.processed;
	bit_string_t &scheduled = pool.scheduled;

	vector_t &processor_time = pool.processor_time;
	vector_t &task_time = pool.task_time;

	Schedule schedule(processor_count, task_count);

	size_t i, count;
	double start, duration, finish;

	for (i = 0; i < task_count; i++)
		if (tasks[i]->is_root()) {
			pool.push(i);
			processed[i] = true;
		}

	task_id_t id, cid;
	processor_id_t pid;

	const Task *task, *child;
	const Processor *processor;

	while (!pool.empty()) {
		/* Get the next task */
		pool.pull(pid, id);

		task = tasks[id];
		processor = processors[pid];

		/* Calculate its start time and duration */
		start = std::max(processor_time[pid], task_time[id]);
		duration = processor->get_execution_time(task->type);
		finish = start + duration;

		processor_time[pid] = finish;

		/* Append to the schedule */
		schedule.append(pid, id, start, duration);
		scheduled[id] = true;

		/* Append children, but only those which are ready,
		 * and ensure absence of repetitions.
		 */
		count = task->children.size();
		for (i = 0; i < count; i++) {
			child = task->children[i];
			cid = child->id;

			/* Shift the child in time with respect to the parent */
			task_time[cid] = std::max(task_time[cid], finish);

			/* Prevent from considering the child once again */
			if (processed[cid]) continue;

			/* All parents should be scheduled */
			if (!ready(child, scheduled)) continue;

			pool.push(cid);
			processed[cid] = true;
		}
	}

	return schedule;
}
