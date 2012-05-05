#ifndef __SCHEDULE_H__
#define __SCHEDULE_H__

#include "System.h"

struct ScheduleItem
{
	task_id_t id;
	double start;
	double duration;

	ScheduleItem(task_id_t _id, double _start, double _duration) :
		id(_id), start(_start), duration(_duration) {}
};

typedef std::vector<ScheduleItem> LocalSchedule;

class Schedule
{
	const size_t processor_count;
	const size_t task_count;

	size_t append_count;
	double duration;

	mapping_t mapping;

	std::vector<LocalSchedule> schedules;

	public:

	Schedule(size_t _processor_count, size_t _task_count) :
		processor_count(_processor_count), task_count(_task_count),
		append_count(0), duration(0), mapping(_task_count),
		schedules(std::vector<LocalSchedule>(processor_count)) {}

	inline size_t get_processor_count() const
	{
		return processor_count;
	}

	inline size_t get_task_count() const
	{
		return task_count;
	}

	inline void append(processor_id_t pid, task_id_t tid, double start, double duration)
	{
		schedules[pid].push_back(ScheduleItem(tid, start, duration));

		mapping[tid] = pid;

		double end = start + duration;
		if (this->duration < end) this->duration = end;
	}

	inline const LocalSchedule &operator[](processor_id_t pid) const
	{
		return schedules[pid];
	}

	inline processor_id_t map(task_id_t id) const
	{
		return mapping[id];
	}

	inline double get_duration() const
	{
		return duration;
	}
};

std::ostream &operator<< (std::ostream &o, const Schedule &schedule);

#endif
