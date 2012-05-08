#include "DynamicPower.h"
#include "Processor.h"
#include "Platform.h"
#include "Task.h"
#include "Graph.h"
#include "Schedule.h"

#define NUMBER_OF_STEPS(deadline, sample) \
	floor((double)(deadline) / (double)(sample))
#define STEP_NUMBER(time, sample) \
	floor((double)(time) / (double)(sample))

DynamicPower::DynamicPower(const Platform &platform, const Graph &graph,
	double deadline, double sampling_interval) :

	_processors(platform.processors()), _processor_count(platform.size()),
	_sampling_interval(sampling_interval),
	_step_count(NUMBER_OF_STEPS(deadline, _sampling_interval))
{
	if (_step_count == 0)
		throw std::runtime_error("The number of steps is zero.");

	const task_vector_t &tasks = graph.tasks();
	size_t task_count = tasks.size();

	_types.resize(task_count);

	for (size_t i = 0; i < task_count; i++)
		_types[i] = tasks[i]->type;
}

void DynamicPower::compute(const Schedule &schedule, matrix_t &_dynamic_power) const
{
	_dynamic_power.resize(_step_count, _processor_count);
	_dynamic_power.nullify();

	double *dynamic_power = _dynamic_power;

	/* Here we build a profile for the whole time period of the graph
	 * including its actual duration (only tasks) plus the gap to
	 * the deadline.
	 */

	for (processor_id_t pid = 0; pid < _processor_count; pid++) {
		const LocalSchedule &local_schedule = schedule[pid];
		size_t task_count = local_schedule.size();
		const Processor *processor = _processors[pid];

		for (size_t i = 0; i < task_count; i++) {
			const ScheduleItem &item = local_schedule[i];

			size_t start = STEP_NUMBER(item.start, _sampling_interval);
			size_t end = STEP_NUMBER(item.start + item.duration, _sampling_interval);
			end = std::min(end, _step_count);

			double power = processor->dynamic_power(_types[item.id]);

			for (size_t j = start; j < end; j++)
				dynamic_power[j * _processor_count + pid] = power;
		}
	}
}
