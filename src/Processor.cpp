#include <iomanip>

#include "Processor.h"

std::ostream &operator<< (std::ostream &o, const Processor *processor)
{
	o	<< std::setw(4) << processor->id()
		<< " ( " << std::setw(8) << processor->type_count() << " )" << std::endl;

	return o;
}
