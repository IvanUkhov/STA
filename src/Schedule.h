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
	const size_t _processor_count;
	const size_t _task_count;

	size_t _append_count;
	double _duration;

	mapping_t _mapping;

	std::vector<LocalSchedule> _schedules;

	public:

	Schedule(size_t processor_count, size_t task_count) :
		_processor_count(processor_count), _task_count(task_count),
		_append_count(0), _duration(0), _mapping(task_count),
		_schedules(std::vector<LocalSchedule>(processor_count)) {}

	inline size_t processor_count() const
	{
		return _processor_count;
	}

	inline size_t task_count() const
	{
		return _task_count;
	}

	inline void append(processor_id_t pid, task_id_t tid, double start, double duration)
	{
		_schedules[pid].push_back(ScheduleItem(tid, start, duration));

		_mapping[tid] = pid;

		double end = start + duration;
		if (_duration < end) _duration = end;
	}

	inline const LocalSchedule &operator[](processor_id_t pid) const
	{
		return _schedules[pid];
	}

	inline processor_id_t map(task_id_t id) const
	{
		return _mapping[id];
	}

	inline double duration() const
	{
		return _duration;
	}
};

std::ostream &operator<< (std::ostream &o, const Schedule &schedule);

#endif
