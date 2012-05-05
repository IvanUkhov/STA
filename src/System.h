#ifndef __SYSTEM_H__
#define __SYSTEM_H__

#include <stdexcept>
#include <ostream>
#include <vector>
#include <list>

#include "array.h"
#include "matrix.h"

typedef size_t task_id_t;
typedef size_t task_type_t;

class Graph;
class Task;
typedef std::vector<Task *> task_vector_t;

typedef size_t processor_id_t;

class Platform;
class Processor;
typedef std::vector<Processor *> processor_vector_t;

typedef int rank_t;
typedef array_t<rank_t> priority_t;
typedef array_t<processor_id_t> layout_t, mapping_t;

template <class PT>
class ListScheduler;

class Schedule;

#endif
