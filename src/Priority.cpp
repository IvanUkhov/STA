#include <algorithm>

#include "Priority.h"
#include "Graph.h"
#include "Platform.h"
#include "GraphAnalysis.h"

template<class T>
class Comparator
{
	public:

	static bool pair(const std::pair<double, T> &first,
		const std::pair<double, T> &second)
	{
		return first.first < second.first;
	}
};

priority_t Priority::mobile(const Platform &platform,
	const Graph &graph, const mapping_t &mapping)
{
	size_t i, task_count = graph.size();

	priority_t priority(task_count);
	vector_t mobility;

	if (mapping.empty())
		mobility = GraphAnalysis::average_mobility(
			platform.processors(), graph.tasks());
	else
		mobility = GraphAnalysis::precise_mobility(
			platform.processors(), graph.tasks(), mapping);

	if (mobility.size() != task_count)
		throw std::runtime_error("The mobility vector is invalid.");

	std::vector<std::pair<double, task_id_t> > pairs(task_count);

	for (i = 0; i < task_count; i++) {
		pairs[i].first = mobility[i];
		pairs[i].second = i;
	}

	std::stable_sort(pairs.begin(), pairs.end(),
		Comparator<task_id_t>::pair);

	for (i = 0; i < task_count; i++)
		priority[pairs[i].second] = i;

	return priority;
}
