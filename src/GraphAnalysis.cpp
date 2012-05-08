#include "GraphAnalysis.h"
#include "Processor.h"
#include "Graph.h"
#include "Task.h"
#include "float.h"

vector_t GraphAnalysis::precise_mobility(const processor_vector_t &processors,
	const task_vector_t &tasks, const mapping_t &mapping)
{
	size_t processor_count = processors.size();
	size_t task_count = tasks.size();

	vector_t duration(task_count);

	if (task_count != mapping.size())
		throw std::runtime_error("The mapping is invalid.");

	for (size_t i = 0; i < task_count; i++) {
		if (mapping[i] >= processor_count)
			throw std::runtime_error("The processor is invalid.");

		duration[i] = processors[mapping[i]]->execution_time(tasks[i]->type);
	}

	return calculate_mobility(processors, tasks, duration);
}

vector_t GraphAnalysis::average_mobility(const processor_vector_t &processors,
	const task_vector_t &tasks)
{
	vector_t duration = average_duration(processors, tasks);
	return calculate_mobility(processors, tasks, duration);
}

vector_t GraphAnalysis::calculate_mobility(const processor_vector_t &processors,
	const task_vector_t &tasks, const vector_t &duration)
{
	size_t task_count = tasks.size();

	if (task_count != duration.size())
		throw std::runtime_error("The duration vector is invalid.");

	vector_t asap = compute_asap(tasks, duration);
	double asap_duration = compute_asap_duration(tasks, asap, duration);
	vector_t alap = compute_alap(tasks, duration, asap_duration);
	vector_t mobility(task_count);

	for (size_t i = 0; i < task_count; i++) {
		mobility[i] = alap[i] - asap[i];
		if (mobility[i] < DBL_EPSILON) mobility[i] = 0;
	}

	return mobility;
}

vector_t GraphAnalysis::average_duration(
	const processor_vector_t &processors, const task_vector_t &tasks)
{
	size_t processor_count = processors.size();
	size_t task_count = tasks.size();

	vector_t duration(task_count, 0);

	size_t i, j;

	for (i = 0; i < task_count; i++)
		for (j = 0; j < processor_count; j++)
			duration[i] += processors[j]->execution_time(tasks[i]->type);

	for (i = 0; i < task_count; i++)
		duration[i] /= double(processor_count);

	return duration;
}

vector_t GraphAnalysis::compute_asap(const task_vector_t &tasks, const vector_t &duration)
{
	size_t task_count = tasks.size();

	vector_t asap(task_count, -1);

	/* Calculate ASAP */
	for (size_t i = 0; i < task_count; i++)
		if (tasks[i]->is_root())
			collect_asap(tasks[i], duration, asap, 0);

	return asap;
}

vector_t GraphAnalysis::compute_alap(const task_vector_t &tasks,
	const vector_t &duration, double asap_duration)
{
	size_t task_count = tasks.size();

	vector_t alap(task_count, DBL_MAX);

	/* Calculate ALAP */
	for (size_t i = 0; i < task_count; i++)
		if (tasks[i]->is_leaf())
			collect_alap(tasks[i], duration, alap, asap_duration);

	return alap;
}

double GraphAnalysis::compute_asap_duration(const task_vector_t &tasks,
	const vector_t &asap, const vector_t &duration)
{
	size_t task_count = tasks.size();

	double asap_duration = 0;

	/* Calculate the overall duration according to ASAP */
	for (size_t i = 0; i < task_count; i++)
		if (tasks[i]->is_leaf())
			asap_duration = std::max(asap_duration, asap[i] + duration[i]);

	return asap_duration;
}

void GraphAnalysis::collect_asap(const Task *task, const vector_t &duration,
	vector_t &asap, double time)
{
	double &my_asap = asap[task->id];

	/* We might already have an assigned ASAP time with a larger value */
	if (!(my_asap < time)) return;

	my_asap = time;
	time = time + duration[task->id];

	/* Shift data dependent tasks */
	size_t size = task->children.size();
	for (size_t i = 0; i < size; i++)
		collect_asap(task->children[i], duration, asap, time);
}

void GraphAnalysis::collect_alap(const Task *task, const vector_t &duration,
	vector_t &alap, double time)
{
	double &my_alap = alap[task->id];

	time = time - duration[task->id];

	/* We might already have an assigned ALAP time with a smaller value */
	if (!(time < my_alap)) return;

	my_alap = time;

	/* Shift data dependent tasks */
	size_t size = task->parents.size();
	for (size_t i = 0; i < size; i++)
		collect_alap(task->parents[i], duration, alap, time);
}
