#include <iomanip>

#include "Graph.h"
#include "Task.h"

Graph::Graph(const vector_t &types, const matrix_t &arcs) : task_count(0)
{
	size_t task_count = types.size();

	if (task_count == 0)
		throw std::runtime_error("There are no tasks.");

	if (arcs.rows() != task_count ||
		arcs.cols() != task_count)

		throw std::runtime_error("The graph data is not consistent.");

	size_t i, j;

	for (i = 0; i < task_count; i++) {
		Task *task = new Task(this->task_count, types[i]);
		tasks.push_back(task);
		this->task_count++;
	}

	for (i = 0; i < task_count; i++)
		for (j = 0; j < task_count; j++)
			if (arcs[i][j]) {
				Task *parent = tasks[i];
				Task *child = tasks[j];

				parent->add_child(child);
				child->add_parent(parent);
			}
}

Graph::~Graph()
{
	for (size_t i = 0; i < task_count; i++)
		delete tasks[i];
}

std::ostream &operator<< (std::ostream &o, const Graph &graph)
{
	const task_vector_t &tasks = graph.get_tasks();
	size_t size = tasks.size();

	o	<< "Task graph:" << std::endl
		<< "  Number of tasks: " << size << std::endl
		<< "  " << std::setw(4) << "id" << " -> [ children ]" << std::endl;

	for (size_t i = 0; i < size; i++)
		o << "  " << tasks[i];

	return o;
}

std::ostream &operator<< (std::ostream &o, const Graph *graph)
{
	return o << &graph;
}
