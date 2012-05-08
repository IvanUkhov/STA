#ifndef __GRAPH_H__
#define __GRAPH_H__

#include "System.h"

class Graph
{
	task_vector_t _tasks;
	size_t _task_count;

	public:

	Graph(const vector_t &types, const matrix_t &arcs);
	~Graph();

	inline size_t size() const
	{
		return _task_count;
	}

	inline const Task *operator[] (task_id_t id) const
	{
		return _tasks[id];
	}

	inline const task_vector_t &tasks() const
	{
		return _tasks;
	}
};

std::ostream &operator<< (std::ostream &, const Graph &);
std::ostream &operator<< (std::ostream &, const Graph *);

#endif
