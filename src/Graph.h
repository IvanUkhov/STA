#ifndef __GRAPH_H__
#define __GRAPH_H__

#include <ostream>

#include "matrix.h"
#include "Task.h"

class Graph
{
	task_vector_t tasks;
	size_t task_count;

	public:

	Graph(const vector_t &types, const matrix_t &arcs);
	~Graph();

	inline size_t get_size() const
	{
		return task_count;
	}

	inline const Task *operator[] (task_id_t id) const
	{
		return tasks[id];
	}

	inline const task_vector_t &get_tasks() const
	{
		return tasks;
	}
};

std::ostream &operator<< (std::ostream &, const Graph *);

#endif
