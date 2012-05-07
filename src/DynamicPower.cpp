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
	double deadline, double _sampling_interval) :

	processors(platform.get_processors()), processor_count(processors.size()),
	sampling_interval(_sampling_interval),
	step_count(NUMBER_OF_STEPS(deadline, _sampling_interval))
{
	if (step_count == 0)
		throw std::runtime_error("The number of steps is zero.");

	const task_vector_t &tasks = graph.get_tasks();
	size_t task_count = tasks.size();

	types.resize(task_count);

	for (size_t i = 0; i < task_count; i++)
		types[i] = tasks[i]->type;
}

void DynamicPower::compute(const Schedule &schedule, matrix_t &_dynamic_power) const
{
	_dynamic_power.resize(step_count, processor_count);
	_dynamic_power.nullify();

	double *dynamic_power = _dynamic_power;

	/* Here we build a profile for the whole time period of the graph
	 * including its actual duration (only tasks) plus the gap to
	 * the deadline.
	 */

	for (processor_id_t pid = 0; pid < processor_count; pid++) {
		const LocalSchedule &local_schedule = schedule[pid];
		size_t task_count = local_schedule.size();
		const Processor *processor = processors[pid];

		for (size_t i = 0; i < task_count; i++) {
			const ScheduleItem &item = local_schedule[i];

			size_t start = STEP_NUMBER(item.start, sampling_interval);
			size_t end = STEP_NUMBER(item.start + item.duration, sampling_interval);
			end = std::min(end, step_count);

			double power = processor->get_dynamic_power(types[item.id]);

			for (size_t j = start; j < end; j++)
				dynamic_power[j * processor_count + pid] = power;
		}
	}
}
