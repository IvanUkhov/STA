#include <iomanip>

#include "Task.h"

std::ostream &operator<< (std::ostream &o, const Task *task)
{
	o << std::setw(4) << task->get_id() << " -> [ ";

	const task_vector_t &children = task->get_children();

	size_t children_count = children.size();
	for (size_t i = 0; i < children_count; i++) {
		if (i > 0) o << ", ";
		o << children[i]->get_id();
	}

	o << " ]" << std::endl;

	return o;
}
