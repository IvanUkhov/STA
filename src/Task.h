#ifndef __TASK_H__
#define __TASK_H__

#include <vector>
#include <ostream>

typedef size_t task_id_t;
typedef size_t task_type_t;

class Task;

typedef std::vector<Task *> task_vector_t;

class Task
{
	const task_id_t id;
	const task_type_t type;

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

	inline task_id_t get_id() const
	{
		return id;
	}

	inline task_type_t get_type() const
	{
		return type;
	}

	inline const task_vector_t &get_children() const
	{
		return children;
	}
};

std::ostream &operator<< (std::ostream &o, const Task *task);

#endif
