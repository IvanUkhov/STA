#ifndef __TASK_H__
#define __TASK_H__

#include "System.h"
#include "GraphAnalysis.h"

class Task
{
	template<class PT> friend class ListScheduler;
	friend class GraphAnalysis;

	public:

	const task_id_t id;
	const task_type_t type;

	private:

	task_vector_t parents;
	task_vector_t children;

	public:

	Task(task_id_t _id, task_type_t _type) : id(_id), type(_type) {}

	inline void add_parent(Task *task)
	{
		parents.push_back(task);
	}

	inline void add_child(Task *task)
	{
		children.push_back(task);
	}

	inline bool is_leaf() const
	{
		return children.empty();
	}

	inline bool is_root() const
	{
		return parents.empty();
	}

	inline const task_vector_t &get_children() const
	{
		return children;
	}
};

std::ostream &operator<< (std::ostream &o, const Task *task);

#endif
